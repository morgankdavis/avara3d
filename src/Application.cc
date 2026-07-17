//
//  Application.cc
//  avara3d
//
//  Created by Morgan Davis on 7/17/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#include "a3d/Application.h"

#include <stdexcept>
#include <utility>

#include "a3d/Runner.h"
#include "a3d/physics/PhysicalWorld.h"
#include "a3d/scene/Scene.h"
#include "a3d/visual/VisualWorld.h"

#ifdef A3D_WEB
	#include <emscripten/emscripten.h>
#endif

using namespace a3d;
using namespace std;

Application::Application():
	_scene{},
	_runner{},
	_didShutdown{false} {
}

Application::Application(int argc, char *argv[]):
	_args(argv + 1, argv + argc),
	_scene{},
	_runner{},
	_didShutdown{false} {
}

Application::~Application() = default;

int Application::Run(
		unique_ptr<Application> application) {

	if (!application) {
		throw invalid_argument(
			"Application::Run requires a non-null Application.");
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

	while (application->update()) {
	}

	application->shutdown();

	return 0;

#endif
}

void Application::prepare() {

	_scene = initialize();

	if (!_scene) {
		throw runtime_error(
			"Application::initialize returned a null Scene.");
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

	// Destroy Scene while the concrete Application's RenderContext
	// and window still exist.
	_scene.reset();

	try {
		didShutdown();
	}
	catch (...) {
		// Teardown paths must not throw.
	}
}

void Application::registerCallbacks() {

	_scene->updateCallback(
		[this](
				Scene& scene,
				double time,
				double deltaTime) {

			sceneUpdate(scene, time, deltaTime);
		}
	);

	if (auto* world = _scene->visualWorld()) {

		world->willRenderCallback(
			[this](
					VisualWorld& world,
					double time,
					double deltaTime) {

				visualWorldWillRender(
					world,
					time,
					deltaTime
				);
			}
		);

		world->didRenderCallback(
			[this](
					VisualWorld& world,
					double time,
					double deltaTime) {

				visualWorldDidRender(
					world,
					time,
					deltaTime
				);
			}
		);
	}

	if (auto* world = _scene->physicalWorld()) {

		world->didSimulateCallback(
			[this](
					PhysicalWorld& world,
					double time,
					double deltaTime) {

				physicalWorldDidSimulate(
					world,
					time,
					deltaTime
				);
			}
		);
	}
}

bool Application::shouldContinue(const Scene&) {
	return true;
}

void Application::didShutdown() {
}

void Application::sceneUpdate(
		Scene&,
		double,
		double) {
}

void Application::visualWorldWillRender(
		VisualWorld&,
		double,
		double) {
}

void Application::visualWorldDidRender(
		VisualWorld&,
		double,
		double) {
}

void Application::physicalWorldDidSimulate(
		PhysicalWorld&,
		double,
		double) {
}
