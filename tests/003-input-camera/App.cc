//
//  App.cc
//  003-inputcamera
//
//  Created by Morgan Davis on 7/11/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#include "App.h"

#include <iostream>
#include <memory>
#include <string>
#include <utility>

#include "a3d/a3d.h"
#include "a3d/util/filesystem.h"

using namespace a3d;
using namespace a3d::math;
using namespace test::inputcamera;
using namespace std;

/// Private Constants ///

const Log::Level						APP_LOG_LEVEL		{Log::Level::Debug};
const uvec2								WINDOW_SIZE			{1280, 768};
const bool								FULLSCREEN			{false};
const bool								ENABLE_HIGH_DPI		{true};
const RenderContext::AntialiasingMode	ANTIALIAS_MODE		{RenderContext::AntialiasingMode::Msaa4X};
const bool								ENABLE_VSYNC		{false};
const bool								CAPTURE_CURSOR		{false};
const float								MOUSE_SENSITIVITY	{0.5};

/// Public Lifecycle Functions ///

App::App(int argc, char* argv[]): Application(argc, argv, APP_LOG_LEVEL) {}

App::~App() = default;

/// Public Member Functions ///

std::unique_ptr<Scene> App::init() {
	try {
		_window = make_unique<Window>(RenderContext::RenderingApi::OpenGL,
									  *util::filesystem::ExecutableName(),
									  WINDOW_SIZE,
									  FULLSCREEN,
									  ENABLE_HIGH_DPI,
									  ANTIALIAS_MODE);
		_window->vSyncEnabled(ENABLE_VSYNC);
		_window->cursorCaptured(CAPTURE_CURSOR);

		auto visualWorld = make_unique<VisualWorld>(*_window);
		//visualWorld->usesDefaultLighting(true);
		auto backgroundColor = make_shared<Color>(u8vec3{109, 136, 164});
		visualWorld->background(backgroundColor);

		auto scene = util::filesystem::SceneNamed("import_test/import_test");
		scene->visualWorld(std::move(visualWorld));
		scene->inputManager(Window::InputManager());
		scene->debugOptions(Scene::DebugOptions::ShowStatsOverlay);

		_window->center();
		_window->open();

		return scene;
	}
	catch (std::exception& e) {
		log::app::f()("Exception: {}", e.what());
		return nullptr;
	}
}

bool App::shouldContinue(const Scene& scene) {
	return _window->isOpen();
}

void App::didShutdown() {

}

/// Scene Callback Overrides ///

void App::sceneUpdate(Scene& scene, double time, double deltaTime) {

	auto window = dynamic_cast<Window*>(scene.visualWorld()->renderContext());

	// get input

	auto im = static_cast<DesktopInputManager*>(scene.inputManager());

	using Key = DesktopInputManager::Key;
	using MouseButton = DesktopInputManager::MouseButton;

	auto keysDown = im->keysDown();
	for (auto k : keysDown) {
		cout << "Key: " << static_cast<underlying_type<Key>::type>(k) << endl;
	}

	auto keysPressed = im->keysPressed();
	if (keysPressed.count(Key::Slash)) {
		window->cursorCaptured(!(window->cursorCaptured()));
	}

	if (keysPressed.count(Key::Escape)) {
		exit(0);
	}

	for (auto mb : im->mouseButtonsDown()) {
		cout << "Mouse button: " << static_cast<underlying_type<MouseButton>::type>(mb) << endl;
	}

	vec2 mouseScrollWheelDelta = im->mouseScrollWheelDelta();
	if (mouseScrollWheelDelta.x > 0 || mouseScrollWheelDelta.y > 0) {
		cout << "Mouse scroll wheel delta: (" << mouseScrollWheelDelta.x << ", "
			 << mouseScrollWheelDelta.y << ")" << endl;
	}

	// move camera

	if (auto pov = scene.visualWorld()->pointOfView().lock()) {

		// look

		vec3 camForward = pov->worldForward();
		vec3 camRight = pov->worldRight();
		vec3 camUp = pov->worldUp();

		// tanA = y/x
		// tanA = mouseDelta / distance
		// A = atan(mouseDelta / distance)

		static const float MOUSE_SPEED_SCALAR = .002;
		static const float MOUSE_SPEED = MOUSE_SENSITIVITY * MOUSE_SPEED_SCALAR;

		vec2 mousePositionDelta = im->mousePositionDelta();
		float deltaRotX = math::atan(MOUSE_SPEED * mousePositionDelta.x);
		float deltaRotY = math::atan(MOUSE_SPEED * mousePositionDelta.y);

		vec3 angles = pov->eulerAngles();
		pov->eulerAngles(vec3(angles.x + deltaRotY, angles.y - deltaRotX, 0));

		// move

		static float MOVE_SPEED = math::max(scene.rootNode()->extent());

		if (keysDown.count(Key::W)) {
			vec3 positionDelta = (float)deltaTime * MOVE_SPEED * camForward;
			pov->position(pov->position() + positionDelta);
		}
		else if (keysDown.count(Key::S)) {
			vec3 positionDelta = (float)deltaTime * MOVE_SPEED * -camForward;
			pov->position(pov->position() + positionDelta);
		}

		if (keysDown.count(Key::A)) {
			vec3 positionDelta = (float)deltaTime * MOVE_SPEED * -camRight;
			pov->position(pov->position() + positionDelta);
		}
		else if (keysDown.count(Key::D)) {
			vec3 positionDelta = (float)deltaTime * MOVE_SPEED * camRight;
			pov->position(pov->position() + positionDelta);
		}

		if (keysDown.count(Key::Space)) {
			vec3 positionDelta = (float)deltaTime * MOVE_SPEED * camUp;
			pov->position(pov->position() + positionDelta);
		}
	}
}

/// VisualWorld Callback Overrides ///

void App::visualWorldWillRender(VisualWorld& world, double time, double deltaTime) {

}

void App::visualWorldDidRender(VisualWorld& world, double time, double deltaTime) {

}

/// PhysicsWorld Callback Overrides ///

void App::physicalWorldDidSimulate(PhysicsWorld& world, double time, double deltaTime) {

}
