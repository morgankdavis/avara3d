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
#include "a3d/util/filesystem.h"
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

	Log appLog{executableName, std::move(sinks)};
	appLog.level(level);
	Log::AppLog(std::move(appLog));

	auto buildInfo = BuildInfo::Info();
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

	registerCallbacks();

	_runner = make_unique<Runner>(*_scene);
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

	_scene->updateCallback(bind(&Application::sceneUpdate, this, _1, _2, _3));

	if (auto* world = _scene->visualWorld()) {
		world->willRenderCallback(bind(&Application::visualWorldWillRender, this, _1, _2, _3));
		world->didRenderCallback(bind(&Application::visualWorldDidRender, this, _1, _2, _3));
	}

	if (auto* world = _scene->physicalWorld()) {
		world->didSimulateCallback(bind(&Application::physicalWorldDidSimulate, this, _1, _2, _3));
	}
}

bool Application::shouldContinue(const Scene&) { return true; }

void Application::didShutdown() {}

void Application::sceneUpdate(Scene& scene, double time, double deltaTime) {}

void Application::visualWorldWillRender(VisualWorld& world, double time, double deltaTime) {}

void Application::visualWorldDidRender(VisualWorld& world, double time, double deltaTime) {}

void Application::physicalWorldDidSimulate(PhysicsWorld& world, double time, double deltaTime) {}
