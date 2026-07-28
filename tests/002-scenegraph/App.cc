//
//  App.cc
//  002-scenegraph
//
//  Created by Morgan Davis on 7/11/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#include "App.h"

#include "a3d/a3d.h"

using namespace a3d;
using namespace a3d::math;
using namespace test::scenegraph;
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

		// I used to do something cool.

		_window = make_unique<Window>(RenderContext::RenderingApi::OpenGL,
									  *util::filesystem::ExecutableName(),
									  WINDOW_SIZE,
									  FULLSCREEN,
									  ENABLE_HIGH_DPI,
									  ANTIALIAS_MODE);
		_window->vSyncEnabled(ENABLE_VSYNC);
		_window->cursorCaptured(CAPTURE_CURSOR);

		auto visualWorld = make_unique<VisualWorld>(*_window);

		auto physicsWorld = make_unique<PhysicsWorld>();
		physicsWorld->timestep(PHYSICS_TIMESTEP);

		auto scene = make_unique<Scene>(std::move(visualWorld),
										std::move(physicsWorld),
										nullptr);
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

	log::app::i()("....and we're done.");
	static_cast<Window*>(scene.visualWorld()->renderContext())->close();
}

/// VisualWorld Callback Overrides ///

void App::visualWorldWillRender(VisualWorld& world, double time, double deltaTime) {

}

void App::visualWorldDidRender(VisualWorld& world, double time, double deltaTime) {

}

/// PhysicsWorld Callback Overrides ///

void App::physicsWorldDidSimulate(PhysicsWorld& world, double time, double deltaTime) {

}
