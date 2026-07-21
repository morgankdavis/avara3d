//
//  App.cc
//  000-quadrotor
//
//  Created by Morgan Davis on 7/19/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#include "App.h"

#include "a3d/a3d.h"

using namespace a3d;
using namespace a3d::math;
using namespace test::quadrotor;
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
const float								PHYSICS_TIMESTEP	{1.0/120.0};
const bool								DARK				{false};

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
		auto backgroundColor = make_shared<Color>(u8vec3{109, 136, 164});
		visualWorld->background(backgroundColor);

		auto physicalWorld = make_unique<PhysicsWorld>();
		physicalWorld->timestep(PHYSICS_TIMESTEP);

		auto scene = make_unique<Scene>(std::move(visualWorld),
										std::move(physicalWorld),
										Window::InputManager());
		scene->debugOptions(Scene::DebugOptions::ShowStatsOverlay);

		_bananaNode = Node::MeshNode(util::filesystem::MeshNamed("banana_lod/banana_lod"));
		auto rot90X = math::quaternion({1.0f, 0.0f, 0.0f}, radians(90.0f));
		auto rot90Y = math::quaternion({0.0f, 1.0f, 0.0f}, radians(90.0f));
		_bananaNode->orientation(rot90X * rot90Y);
		scene->rootNode()->addChild(_bananaNode);

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

	if (static_cast<DesktopInputManager*>(
	scene.inputManager())->keysPressed().count(DesktopInputManager::Key::Escape)) {
		_window->close();
	}

	if (_bananaNode) {
		// rotate the banana
		auto rotationDeg = deltaTime * radians(-30.0); // 10deg/sec

		auto rotY = math::quaternion({0.0f, 1.0f, 0.0f}, rotationDeg);
		_bananaNode->orientation(rotY * _bananaNode->orientation());
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
