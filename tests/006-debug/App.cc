//
//  App.cc
//  006-debug
//
//  Created by Morgan Davis on 7/11/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#include "App.h"

#include "a3d/a3d.h"

using namespace a3d;
using namespace a3d::math;
using namespace test::debug;
using namespace std;

/// Private Constants ///

const Log::Level						APP_LOG_LEVEL		{Log::Level::Debug};
const uvec2								WINDOW_SIZE			{1280, 768};
const bool								FULLSCREEN			{false};
const bool								ENABLE_HIGH_DPI		{true};
const RenderContext::AntialiasingMode	ANTIALIAS_MODE		{RenderContext::AntialiasingMode::None};
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
		visualWorld->fogStartDistance(500.0);
		visualWorld->fogEndDistance(5000.0);
		visualWorld->fogDensityExponent(1.0);
		visualWorld->fogColor(Color::LightGray());
		visualWorld->background(make_shared<Texture>(util::filesystem::CubeImageNamed("sky1", "png")));

		auto scene = make_unique<Scene>(std::move(visualWorld),
		                                nullptr,
		                                Window::InputContext());
		auto debugOptions = Scene::DebugOptions::ShowStatsOverlay
							| Scene::DebugOptions::ShowBoundingBoxes;
		scene->debugOptions(debugOptions);

		auto ambientLight = make_shared<AmbientLight>(Color::Gray());
		auto ambientLightNode = make_shared<Node>("Ambient light");
		ambientLightNode->light(ambientLight);
		scene->rootNode()->addChild(ambientLightNode);

		auto pointLight = make_shared<PointLight>(Color::White());
		pointLight->attenuation(Attenuation{.quadratic = 0.0001f});
		auto pointLightNode = make_shared<Node>();
		pointLightNode->light(pointLight);
		scene->rootNode()->addChild(pointLightNode);
		pointLightNode->position({100.0, 20.0, 20.0});

		auto materialProperty = pointLight->color();
		auto material = make_shared<Material>();
		material->name("LIGHT material");
		material->emission(materialProperty);
		auto mesh = shared_ptr(Sphere::Mesh(3.5, 4, material));
		pointLightNode->mesh(mesh);

		auto teapotNode = Node::MeshNode(util::filesystem::MeshNamed("teapot/teapot"));
		teapotNode->rotation({1, 0, 0}, radians(30.0));
		teapotNode->scale(teapotNode->scale() * 50.0f);
		scene->rootNode()->addChild(teapotNode);

		auto dragonNode = Node::MeshNode(util::filesystem::MeshNamed("dragon/dragon"));
		dragonNode->scale({2.5, 2.5, 2.5});
		dragonNode->position({50, 0, 0});

		scene->rootNode()->addChild(dragonNode);

		auto boxNode = Node::MeshNode(Box::Mesh(1.0, 1.0, 1.0));
		scene->rootNode()->addChild(boxNode);

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

/// Input Context Callbacks ///

void App::inputContextDidUpdate(InputContext& inputContext,
                                const InputContext::UpdateInfo& info) {

	auto& scene = *_window->visualWorld()->scene();

	auto window = dynamic_cast<Window*>(scene.visualWorld()->renderContext());

	// get input

	auto im = static_cast<DesktopInputContext*>(&inputContext);

	auto keysPressed = im->keysPressed();

	using Key = DesktopInputContext::Key;
	using MouseButton = DesktopInputContext::MouseButton;

	if (keysPressed.count(Key::Escape)) {
		window->close();
	}

	if (keysPressed.count(Key::Slash)) {
		window->cursorCaptured(!(window->cursorCaptured()));
	}

	using DebugOptions = Scene::DebugOptions;

	if (keysPressed.count(Key::F)) {
		if (util::bitmask::contains(scene.debugOptions(), DebugOptions::ShowWireframes)) {
			scene.debugOptions(util::bitmask::remove(scene.debugOptions(),
											  DebugOptions::ShowWireframes));
		}
		else {
			scene.debugOptions(util::bitmask::add(scene.debugOptions(),
										   DebugOptions::ShowWireframes));
		}
	}
	if (keysPressed.count(Key::B)) {
		if (util::bitmask::contains(scene.debugOptions(), DebugOptions::ShowBoundingBoxes)) {
			scene.debugOptions(util::bitmask::remove(scene.debugOptions(),
											  DebugOptions::ShowBoundingBoxes));
		}
		else {
			scene.debugOptions(util::bitmask::add(scene.debugOptions(),
										   DebugOptions::ShowBoundingBoxes));
		}
	}
	if (keysPressed.count(Key::I)) {
		if (util::bitmask::contains(scene.debugOptions(), DebugOptions::ShowStatsOverlay)) {
			scene.debugOptions(util::bitmask::remove(scene.debugOptions(),
											  DebugOptions::ShowStatsOverlay));
		}
		else {
			scene.debugOptions(util::bitmask::add(scene.debugOptions(),
										   DebugOptions::ShowStatsOverlay));
		}
	}

	if (keysPressed.count(Key::V)) {
		window->vSyncEnabled(!(window->vSyncEnabled()));
	}

	if (keysPressed.count(Key::Backslash)) {
		util::snapshot::SaveSnapshot(*window);
	}

	if (keysPressed.count(Key::R)) {
		if (!window->recordingGIF()) {
			util::snapshot::StartGIFRecording(*window, {320, 240}, 8);
		}
		else {
			util::snapshot::StopGIFRecording(*window);
		}
	}

	if (window->cursorCaptured()) {

		// mouselook

		vec2 mousePositionDelta = im->mousePositionDelta();

		auto pov = scene.visualWorld()->pointOfView().lock();
		if (pov) {

			// look

			vec3 camForward = pov->worldForward();
			vec3 camRight = pov->worldRight();
			vec3 camUp = pov->worldUp();

			static const float MOUSE_SPEED_SCALAR = .002;
			static const float MOUSE_SPEED = MOUSE_SENSITIVITY * MOUSE_SPEED_SCALAR;

			float deltaRotX = math::atan(MOUSE_SPEED * mousePositionDelta.x);
			float deltaRotY = math::atan(MOUSE_SPEED * mousePositionDelta.y);

			vec3 angles = pov->eulerAngles();
			pov->eulerAngles(vec3(angles.x + deltaRotY, angles.y - deltaRotX, 0));

			// move

			static float MOVE_SPEED = math::max(scene.rootNode()->extent());

			auto keysDown = im->keysDown();

			if (keysDown.count(Key::W)) {
				vec3 positionDelta = (float)info.deltaTime * MOVE_SPEED * camForward;
				pov->position(pov->position() + positionDelta);
			} else if (keysDown.count(Key::S)) {
				vec3 positionDelta = (float)info.deltaTime * MOVE_SPEED * -camForward;
				pov->position(pov->position() + positionDelta);
			}

			if (keysDown.count(Key::A)) {
				vec3 positionDelta = (float)info.deltaTime * MOVE_SPEED * -camRight;
				pov->position(pov->position() + positionDelta);
			} else if (keysDown.count(Key::D)) {
				vec3 positionDelta = (float)info.deltaTime * MOVE_SPEED * camRight;
				pov->position(pov->position() + positionDelta);
			}

			if (keysDown.count(Key::Space)) {
				vec3 positionDelta = (float)info.deltaTime * MOVE_SPEED * camUp;
				pov->position(pov->position() + positionDelta);
			}
		}
	}
}

/// Visual World Callbacks ///

void App::visualWorldWillRender(VisualWorld& visualWorld,
                                const VisualWorld::RenderInfo& info) {}

void App::visualWorldDidRender(VisualWorld& visualWorld,
		const VisualWorld::RenderInfo& info) {}
