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
#endif

#include "a3d/BuildInfo.h"
#include "a3d/Runner.h"
#include "a3d/log/Log.h"
#include "a3d/log/sink/FileLogSink.h"
#include "a3d/log/sink/StdOutLogSink.h"
#include "a3d/scene/Scene.h"
#include "a3d/util/Filesystem.h"

using namespace a3d;
using namespace std;

/// Public Static Member Functions ///

int Application::Run(unique_ptr<Application> application) {

	if (!application) {
		throw invalid_argument("Application::Run() requires a non-null Application.");
	}

	application->prepare();

#ifdef A3D_WEB

	auto* context = application.release();

	emscripten_set_main_loop_arg(
		[](void* opaque) {
			auto* application = static_cast<Application*>(opaque);

			if (!application->update()) {
				emscripten_cancel_main_loop();

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

/// Public Lifecycle Functions ///

Application::Application(int argc, char* argv[], Log::Level logLevel):
	_args(argv + 1, argv + argc),
	_scene{},
	_runner{},
	_didShutdown{false},
	_sceneCommandQueue{},
	_renderCommandQueue{} {
	initLog(logLevel);
}

Application::~Application() = default;

/// Protected Member Functions ///

SimulationConfig Application::simulationConfig() const {
	return {};
}

bool Application::shouldContinue(const Scene&) {
	return true;
}

void Application::didShutdown() {}

void Application::queueSceneCommand(SceneCommand command) {
	_sceneCommandQueue.push(std::move(command));
}

void Application::queueRenderCommand(RenderCommand command) {
	_renderCommandQueue.push(std::move(command));
}

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

const vector<string>& Application::args() const {
	return _args;
}

/// Runner Callbacks ///

void Application::hostUpdate(Runner& runner, const Runner::UpdateInfo& info) {}

/// Scene Callbacks ///

void Application::sceneWillStep(Scene& scene, const Scene::StepInfo& info) {}

void Application::sceneDidStep(Scene& scene, const Scene::StepInfo& info) {}

/// VisualWorld Callbacks ///

void Application::didBeginFrame(VisualWorld& visualWorld, const VisualWorld::RenderInfo& info) {}

/// Private Member Functions ///

void Application::initLog(Log::Level level) {

	Log::MainLog().level(level);

	string executableName = *util::filesystem::ExecutableName();

	auto sinks = vector<unique_ptr<LogSink>>();

	auto nativeSink = make_unique<StdOutLogSink>();
	sinks.push_back(std::move(nativeSink));

#ifndef A3D_WEB
	auto fileSink = make_unique<FileLogSink>(*(util::filesystem::ExecutableDirectory())
											 / (executableName + string(".log")));
	sinks.push_back(std::move(fileSink));
#endif

	Log::AppLog(make_unique<Log>(executableName, std::move(sinks), level));

	const auto& buildInfo = BuildInfo::Info();
	log::app::i()("A3D version: {}", BuildInfo::VersionString(buildInfo.version()));
	log::app::i()("Build: {}", buildInfo.number());
	log::app::i()("Type: {}", BuildInfo::TypeString(buildInfo.type()));
	log::app::i()("Origin: {}", BuildInfo::OriginString(buildInfo.origin()));
}

void Application::prepare() {

	_scene = init();

	if (!_scene) {
		throw runtime_error("Application::initialize() returned a null Scene.");
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
		// Teardown paths must not throw.
	}
}

void Application::registerCallbacks() {

	using namespace std::placeholders;

	_runner->updateCallback(bind(&Application::dispatchHostUpdate, this, _1, _2));
	_scene->willStepCallback(bind(&Application::dispatchSceneWillStep, this, _1, _2));
	_scene->didStepCallback(bind(&Application::dispatchSceneDidStep, this, _1, _2));

	if (auto* world = _scene->visualWorld()) {
		world->didBeginFrameCallback(bind(&Application::dispatchDidBeginFrame, this, _1, _2));
	}
}

void Application::dispatchHostUpdate(Runner& runner, const Runner::UpdateInfo& info) {

	hostUpdate(runner, info);
}

void Application::dispatchSceneWillStep(Scene& scene, const Scene::StepInfo& info) {

	executePendingCommands(_sceneCommandQueue, scene);
	sceneWillStep(scene, info);
}

void Application::dispatchSceneDidStep(Scene& scene, const Scene::StepInfo& info) {

	sceneDidStep(scene, info);
}

void Application::dispatchDidBeginFrame(VisualWorld& visualWorld, const VisualWorld::RenderInfo& info) {

	executePendingCommands(_renderCommandQueue, visualWorld);
	didBeginFrame(visualWorld, info);
}
