//
//  Application.cc
//  avara3d
//
//  Created by Morgan Davis on 7/17/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#include "a3d/Application.h"

#include <stdexcept>

#ifdef A3D_WEB
	#include <emscripten/emscripten.h>
#endif

#include "a3d/BuildInfo.h"
#include "a3d/Runner.h"
#include "a3d/log/Log.h"
#include "a3d/log/sink/FileLogSink.h"
#include "a3d/log/sink/StdOutLogSink.h"
#include "a3d/physics/PhysicsWorld.h"
#include "a3d/scene/Scene.h"
#include "a3d/util/Filesystem.h"
#include "a3d/visual/VisualWorld.h"

using namespace a3d;
using namespace std;
using namespace std::placeholders;

Application::Application(int argc, char *argv[], Log::Level logLevel):
	_args(argv + 1, argv + argc),
	_scene{},
	_runner{},
	_didShutdown{false} {
	initLog(logLevel);
}

Application::~Application() = default;

int Application::Run(
		unique_ptr<Application> application) {

	if (!application) {
		throw invalid_argument("Application::Run() requires a non-null Application.");
	}

	application->prepare();

#ifdef A3D_WEB

	auto* context = application.release();

	emscripten_set_main_loop_arg(
		[](void* opaque) {

			auto* application =
				static_cast<Application*>(opaque);

			if (!application->update()) {
				emscripten_cancel_main_loop();

				application->shutdown();
				delete application;
			}
		},
		context,
		0,
		false
	);

	return 0;

#else

	while (application->update());

	application->shutdown();

	return 0;

#endif
}

/// Protected Static Member Functions ///

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

	Log::AppLog(make_unique<Log>(
		executableName,
		std::move(sinks),
		level));

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

	_runner = make_unique<Runner>(*_scene, simulationConfiguration());

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

	_runner->updateCallback(bind(&Application::runnerUpdate, this, _1, _2));
	_scene->willSimulateCallback(bind(&Application::sceneWillSimulate, this, _1, _2));
	_scene->didSimulateCallback(bind(&Application::sceneDidSimulate, this, _1, _2));

	if (auto* world = _scene->physicsWorld()) {
		world->willStepCallback(bind(&Application::physicsWorldWillStep, this, _1, _2));
		world->didStepCallback(bind(&Application::physicsWorldDidStep, this, _1, _2));
	}

	if (auto* world = _scene->visualWorld()) {
		world->willRenderCallback(bind(&Application::visualWorldWillRender, this, _1, _2));
		world->didRenderCallback(bind(&Application::visualWorldDidRender, this, _1, _2));
	}
}

SimulationConfiguration Application::simulationConfiguration() const {
	return {};
}

bool Application::shouldContinue(const Scene&) { return true; }

void Application::didShutdown() {}

void Application::runnerUpdate(Runner& runner,
                               const HostUpdateInfo& info) {}

void Application::sceneWillSimulate(Scene& scene,
                                    const SimulationStepInfo& info) {}

void Application::sceneDidSimulate(Scene& scene,
                                   const SimulationStepInfo& info) {}

void Application::visualWorldWillRender(VisualWorld& world,
                                        const RenderFrameInfo& info) {}

void Application::visualWorldDidRender(VisualWorld& world,
                                       const RenderFrameInfo& info) {}

void Application::physicsWorldWillStep(PhysicsWorld& world,
                                        const SimulationStepInfo& info) {}

void Application::physicsWorldDidStep(PhysicsWorld& world,
                                       const SimulationStepInfo& info) {}
