//
//  Application.cc
//  avara3d
//
//  Created by Morgan Davis on 7/17/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#include "a3d/Application.h"

#include <stdexcept>

#include "a3d/visual/VisualWorld.h"

#ifdef A3D_WEB
    #include <emscripten/emscripten.h>
    #include <emscripten/html5.h>
#endif

#include "a3d/BuildInfo.h"
#include "a3d/Runner.h"
#include "a3d/log/Log.h"
#include "a3d/log/sink/FileLogSink.h"
#include "a3d/log/sink/StdOutLogSink.h"
#include "a3d/scene/Scene.h"
#include "a3d/util/Chrono.h"
#include "a3d/util/Filesystem.h"

using namespace std;

namespace a3d {
namespace {
    // [Private Non-Member Prototypes]

#ifdef A3D_WEB
    void    RegisterEmscriptenVisibilityCallbacks(Runner& runner);
    void    UnregisterEmscriptenVisibilityCallbacks();
    EM_BOOL EmscriptenVisibilityChangeCallback(int,
                                               const EmscriptenVisibilityChangeEvent* event,
                                               void*                                  userData);
#endif
} // namespace

// [Public Static Member Functions]

int Application::Run(unique_ptr<Application> application) {

    if (!application) {
        throw invalid_argument("Application::Run() requires a non-null Application.");
    }

    application->prepare();

#ifdef A3D_WEB

    RegisterEmscriptenVisibilityCallbacks(application->runner());

    auto* context = application.release();

    emscripten_set_main_loop_arg(
        [](void* opaque) {
            auto* application = static_cast<Application*>(opaque);

            if (!application->update()) {
                emscripten_cancel_main_loop();

                UnregisterEmscriptenVisibilityCallbacks();

                application->shutdown();
                delete application;
            }
        },
        context, 0, false);

    return 0;

#else

    while (application->update())
        ;

    application->shutdown();

    return 0;

#endif
}

// [Public Lifecycle Functions]

Application::Application(int argc, char* argv[], log::Level logLevel):
    _args(argv + 1, argv + argc),
    _scene {},
    _runner {},
    _scenePreStepQueue {},
    _scenePostStepQueue {},
    _didShutdown {false},
    _startupTimer {true} {
    initLog(logLevel);
}

Application::~Application() = default;

// [Protected Member Functions]

SimulationConfig Application::simulationConfig() const {
    return {};
}

bool Application::shouldContinue(const Scene&) {
    return true;
}

void Application::didShutdown() {}

Runner& Application::runner() {

    if (!_runner) {
        throw logic_error("Application::runner() requires an initialized Runner.");
    }

    return *_runner;
}

const Runner& Application::runner() const {

    if (!_runner) {
        throw logic_error("Application::runner() requires an initialized Runner.");
    }

    return *_runner;
}

Scene& Application::scene() {

    if (!_scene) {
        throw logic_error("Application::scene() requires an initialized Scene.");
    }

    return *_scene;
}

const Scene& Application::scene() const {

    if (!_scene) {
        throw logic_error("Application::scene() requires an initialized Scene.");
    }

    return *_scene;
}

void Application::queueScenePreStepCommand(SceneCommand command) {

    if (!command) {
        throw invalid_argument("Application pre-step Scene command must not be empty.");
    }

    _scenePreStepQueue.push_back(std::move(command));
}

void Application::queueScenePostStepCommand(SceneCommand command) {

    if (!command) {
        throw invalid_argument("Application post-step Scene command must not be empty.");
    }

    _scenePostStepQueue.push_back(std::move(command));
}

const vector<string>& Application::args() const {
    return _args;
}

void Application::runnerUpdate(Runner& runner, Scene& scene, const Runner::UpdateInfo& info) {}

void Application::inputDidUpdate(Runner&       runner,
                                 Scene&        scene,
                                 InputContext& inputContext,
                                 const InputContext::UpdateInfo&) {}

void Application::sceneWillStep(Runner& runner, Scene& scene, const Scene::StepInfo& info) {}

void Application::sceneDidStep(Runner& runner, Scene& scene, const Scene::StepInfo& info) {}

void Application::frameDidBegin(Runner&                        runner,
                                Scene&                         scene,
                                VisualWorld&                   visualWorld,
                                const VisualWorld::RenderInfo& info) {}

void Application::contactDidBegin(Runner&               runner,
                                  Scene&                scene,
                                  PhysicsWorld&         physicsWorld,
                                  const PhysicsContact& contact) {}

void Application::contactDidContinue(Runner&               runner,
                                     Scene&                scene,
                                     PhysicsWorld&         physicsWorld,
                                     const PhysicsContact& contact) {}

void Application::contactDidEnd(Runner&               runner,
                                Scene&                scene,
                                PhysicsWorld&         physicsWorld,
                                const PhysicsContact& contact) {}

// [Private Member Functions]

void Application::initLog(log::Level level) {

    log::MainLog().level(level);

    string executableName = *util::fs::ExecutableName();

    auto sinks = vector<unique_ptr<log::LogSink>>();

    auto nativeSink = make_unique<log::StdOutLogSink>();
    sinks.push_back(std::move(nativeSink));

#ifndef A3D_WEB
    auto fileSink =
        make_unique<log::FileLogSink>(*(util::fs::ExecutableDirectory()) / (executableName + string(".log")));
    sinks.push_back(std::move(fileSink));
#endif

    log::AppLog(make_unique<log::Log>(executableName, std::move(sinks), level));

    const auto& buildInfo = BuildInfo::Info();
    log::app::i()("A3D version: {}", BuildInfo::VersionString(buildInfo.version()));
    log::app::i()("Build: {}", buildInfo.number());
    log::app::i()("Type: {}", BuildInfo::TypeString(buildInfo.type()));
    log::app::i()("Origin: {}", BuildInfo::OriginString(buildInfo.origin()));
}

void Application::prepare() {

    _scene = init();

    if (!_scene) {
        throw runtime_error("Application::init() returned a null Scene.");
    }

    _runner = make_unique<Runner>(*_scene, simulationConfig());

    registerCallbacks();

    _runner->start();
}

bool Application::update() {

    if (!_runner || !_scene) {
        return false;
    }

    if (!shouldContinue(*_scene)) {
        _runner->stop();
        return false;
    }

    return _runner->update();
}

void Application::shutdown() noexcept {

    if (_didShutdown) {
        return;
    }

    _didShutdown = true;

    if (_runner) {
        _runner->stop();
        _runner.reset();
    }

    // destroy Scene while the concrete Application's RenderContext & window still exist!
    _scene.reset();

    try {
        didShutdown();
    }
    catch (...) {
        // teardown paths must not throw.
    }
}

void Application::registerCallbacks() {

    using namespace std::placeholders;

    _runner->updateCallback(bind(&Application::dispatchRunnerUpdate, this, _1, _2));

    if (auto* inputContext = _scene->inputContext()) {
        inputContext->didUpdateCallback(bind(&Application::dispatchInputContextDidUpdate, this, _1, _2));
    }

    _scene->willStepCallback(bind(&Application::dispatchSceneWillStep, this, _1, _2));
    _scene->didStepCallback(bind(&Application::dispatchSceneDidStep, this, _1, _2));

    if (auto* visualWorld = _scene->visualWorld()) {
        visualWorld->didBeginFrameCallback(bind(&Application::dispatchDidBeginFrame, this, _1, _2));
    }

    if (auto* physicsWorld = _scene->physicsWorld()) {
        physicsWorld->didBeginContactCallback(bind(&Application::dispatchContactDidBegin, this, _1, _2));
        physicsWorld->didContinueContactCallback(bind(&Application::dispatchContactDidContinue, this, _1, _2));
        physicsWorld->didEndContactCallback(bind(&Application::dispatchContactDidEnd, this, _1, _2));
    }
}

void Application::executeSceneCommands(vector<SceneCommand>& queue, Scene& scene) {

    auto commands = exchange(queue, vector<SceneCommand> {});

    for (auto& command : commands) {
        command(scene);
    }
}

void Application::dispatchRunnerUpdate(Runner& runner, const Runner::UpdateInfo& info) {
    runnerUpdate(runner, *_scene, info);
}

void Application::dispatchInputContextDidUpdate(InputContext&                   inputContext,
                                                const InputContext::UpdateInfo& info) {
    inputDidUpdate(*_runner, *_scene, inputContext, info);
}

void Application::dispatchSceneWillStep(Scene& scene, const Scene::StepInfo& info) {

    executeSceneCommands(_scenePreStepQueue, scene);
    sceneWillStep(*_runner, scene, info);
}

void Application::dispatchSceneDidStep(Scene& scene, const Scene::StepInfo& info) {

    executeSceneCommands(_scenePostStepQueue, scene);
    sceneDidStep(*_runner, scene, info);
}

void Application::dispatchDidBeginFrame(VisualWorld& visualWorld, const VisualWorld::RenderInfo& info) {
    if (info.frameIndex == 0) {
        log::app::i()("Time to first frame: {:.0f}ms", util::chrono::Milliseconds(_startupTimer.stop()));
    }
    frameDidBegin(*_runner, *_scene, visualWorld, info);
}

void Application::dispatchContactDidBegin(PhysicsWorld& physicsWorld, const PhysicsContact& contact) {
    contactDidBegin(*_runner, *_scene, physicsWorld, contact);
}

void Application::dispatchContactDidContinue(PhysicsWorld& physicsWorld, const PhysicsContact& contact) {
    contactDidContinue(*_runner, *_scene, physicsWorld, contact);
}

void Application::dispatchContactDidEnd(PhysicsWorld& physicsWorld, const PhysicsContact& contact) {
    contactDidEnd(*_runner, *_scene, physicsWorld, contact);
}

namespace {
    // [Private Non-Member Functions]

#ifdef A3D_WEB

    void RegisterEmscriptenVisibilityCallbacks(Runner& runner) {

        const auto result =
            emscripten_set_visibilitychange_callback(&runner, false, EmscriptenVisibilityChangeCallback);

        if (result != EMSCRIPTEN_RESULT_SUCCESS) {
            throw runtime_error("Failed to register Emscripten visibility callback.");
        }

        // handle the unlikely case that we started while already hidden.
        EmscriptenVisibilityChangeEvent visibility {};

        if (emscripten_get_visibility_status(&visibility) == EMSCRIPTEN_RESULT_SUCCESS && visibility.hidden) {
            runner.simulationClockSuspended(true);
        }
    }

    void UnregisterEmscriptenVisibilityCallbacks() {

        emscripten_set_visibilitychange_callback(nullptr, false, nullptr);
    }

    EM_BOOL EmscriptenVisibilityChangeCallback(int,
                                               const EmscriptenVisibilityChangeEvent* event,
                                               void*                                  userData) {

        auto& runner = *static_cast<Runner*>(userData);

        if (event->hidden) {
            runner.simulationClockSuspended(true);
        }
        else {
            runner.simulationClockSuspended(false);
        }

        return EM_FALSE;
    }

#endif
} // namespace
} // namespace a3d
