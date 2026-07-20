//
//  App.cc
//  004-geoprimitives
//
//  Created by Morgan Davis on 7/11/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#include "App.h"

#include "a3d/a3d.h"

using namespace a3d;
using namespace a3d::math;
using namespace test::geoprimitives;
using namespace std;

/// Private Static Non-Member Prototypes ///

// nada

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
		//auto backgroundColor = make_shared<Color>(109.0f/255.0f, 136.0f/255.0f, 164.0f/255.0f, 1.0f);
		visualWorld->background(Color::Black());

		auto scene = make_unique<Scene>(std::move(visualWorld),
		                                nullptr,
		                                Window::InputManager());
		scene->debugOptions(Scene::DebugOptions::ShowStatsOverlay);

		auto ambientLight = make_shared<AmbientLight>(make_shared<Color>(0.1f));
		auto ambientLightNode = Node::LightNode(ambientLight);
		scene->rootNode()->addChild(ambientLightNode);

		auto pointLight = make_shared<PointLight>(Color::LightGray());
		auto pointLightNode = Node::LightNode(pointLight);
		pointLightNode->position({0, 0, 5});
		auto material = make_shared<Material>(monostate{},
											  monostate{},
											  monostate{},
											  Color::White());
		auto sphere = Sphere::Mesh(0.25f, 12, material);
		pointLightNode->mesh(sphere);
		auto pointLightPivotNode = Node::NamedNode("point light pivot");
		_pointLightPivotNode = pointLightPivotNode.get();
		pointLightPivotNode->addChild(pointLightNode);
		scene->rootNode()->addChild(pointLightPivotNode);

		{
			auto mesh = Box::Mesh(1.5f, 1.0f, 1.5f);
			auto node = make_shared<Node>();
			node->name("box");
			node->mesh(mesh);
			scene->rootNode()->addChild(node);
			node->rotation({0.0f, 1.0f, 0.0f}, radians(-45.0f));
			node->position(vec3(1.67f, -2.5f, 0.0f));
		}

		{
			auto mesh = Capsule::Mesh(0.5f, 1.0f);
			auto node = make_shared<Node>();
			node->name("capsule");
			node->mesh(mesh);
			scene->rootNode()->addChild(node);
			node->position(vec3(-5.0f, -2.5f, 0.0f));
		}

		{
			auto mesh = Cone::Mesh(1.0f, 2.0f);
			auto node = make_shared<Node>();
			mesh->name("cone");
			node->mesh(mesh);
			scene->rootNode()->addChild(node);
			//node->position(vec3(-5.0f, 0.0f, 0.0f));
			node->position(vec3(-1.67f, -2.5f, 0.0f));
		}

		{
			auto mesh = Cylinder::Mesh(0.5f, 2.0f);
			auto node = make_shared<Node>();
			mesh->name("cylinder");
			node->mesh(mesh);
			scene->rootNode()->addChild(node);
			node->position(vec3(5.0f, -2.5f, 0.0f));
		}

		{
			auto mesh = Plane::Mesh(10.0f, 10.0f);
			auto node = make_shared<Node>();
			mesh->name("plane");
			node->mesh(mesh);
			scene->rootNode()->addChild(node);
			node->rotation({-1.0f, 0.0f, 0.0f}, radians(90.0f));
			node->position(vec3(0.0f, -5.0f, 0.0f));
		}

		{
			auto mesh = RoundedBox::Mesh(0.25f, 1, 1, 1);
			auto node = make_shared<Node>();
			mesh->name("rounded box");
			node->mesh(mesh);
			scene->rootNode()->addChild(node);
			node->rotation({0.0f, 1.0f, 0.0f}, radians(70.0f));
			node->position(vec3(-3.0f, 2.5f, 0.0f));
		}

		{
			auto mesh = Sphere::Mesh(1.0f);
			auto node = make_shared<Node>();
			mesh->name("sphere");
			node->mesh(mesh);
			scene->rootNode()->addChild(node);
			node->position(vec3(3.0f, 2.5f, 0.0f));
		}

		{
			auto mesh = Spring::Mesh(0.2f, 0.5f, 2.5f);
			auto node = make_shared<Node>();
			mesh->name("spring");
			node->mesh(mesh);
			scene->rootNode()->addChild(node);
			node->position(vec3(0.0f, 3.5f, 0.0f));
			node->rotation({0.0f, 1.0f, 0.0f}, radians(-90.0f));
		}

		{
			auto mesh = Torus::Mesh(0.75f, 1.0f);
			auto node = make_shared<Node>();
			mesh->name("torus");
			node->mesh(mesh);
			scene->rootNode()->addChild(node);
			node->position(vec3(4.25f, 0.0f, 0.0f));
		}

		{
			auto mesh = TorusKnot::Mesh(2, 3);
			auto node = make_shared<Node>();
			mesh->name("torus knot");
			node->mesh(mesh);
			scene->rootNode()->addChild(node);
			node->rotation({0.0f, 1.5f, 0.0f}, radians(45.0f));
			node->position(vec3(0.0f, 0.0f, 0.0f));
		}

		{
			auto mesh = Tube::Mesh(0.5f, 0.75f, 2.0f);
			auto node = make_shared<Node>();
			mesh->name("tube");
			node->mesh(mesh);
			scene->rootNode()->addChild(node);
			node->position(vec3(-4.25f, 0.0f, 0.0f));
		}

		{
			auto mesh = Wedge::Mesh(2.0f, 2.0f, 1.0f);
			auto node = make_shared<Node>();
			mesh->name("wedge");
			node->mesh(mesh);
			scene->rootNode()->addChild(node);
			node->position(vec3(2.5f, 5.0f, 0.0f));
			node->rotation({0.0f, 1.0f, 0.0f}, radians(30.0f));
		}

		{
			auto mesh = Dome::Mesh(1.0f,
								   math::radians(60.0), math::radians(90.0),
								   0, math::radians(180.0));
			auto node = make_shared<Node>();
			mesh->name("dome");
			node->mesh(mesh);
			scene->rootNode()->addChild(node);
			node->position(vec3(-2.5f, 5.0f, 0.0f));
			node->rotation({0.0f, 1.0f, 0.0f}, radians(-30.0f));
		}

	//	int texIndex = 0;
	//	vector<shared_ptr<Image>> textures = { utils::ImageNamed("test_textures/blue", "png"),
	//										   utils::ImageNamed("test_textures/cyan", "png"),
	//										   utils::ImageNamed("test_textures/green", "png"),
	//										   utils::ImageNamed("test_textures/magenta", "png"),
	//										   utils::ImageNamed("test_textures/orange", "png"),
	//										   utils::ImageNamed("test_textures/purple", "png"),
	//										   utils::ImageNamed("test_textures/red", "png"),
	//										   utils::ImageNamed("test_textures/yellow", "png"),
	//										   utils::ImageNamed("test_textures/blue", "png"),
	//										   utils::ImageNamed("test_textures/cyan", "png") };
	//
	//	for (auto& node : scene->rootNode()->children(true)) {
	//		if (auto mesh = node->mesh(); mesh) {
	//
	//			auto elements = mesh->elements();
	//			for (int e=0; e<elements.size(); ++e) {
	//
	//				auto material = make_shared<Material>();
	//				MaterialProperty property = make_shared<Texture>(textures[texIndex++]);
	//				material->diffuse(property);
	//				material->doubleSided(true);
	//				//mesh->addMaterial(material);
	//				mesh->replaceMaterial(0, material);
	//				if (texIndex >= textures.size()) {
	//					texIndex = 0;
	//				}
	//			}
	//		}
	//	}

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

	Window* window = nullptr;
	if (scene.visualWorld()) {
		window = dynamic_cast<Window*>(scene.visualWorld()->renderContext());
	}

	// get input

	auto im = static_cast<DesktopInputManager*>(scene.inputManager());

	auto keysPressed = im->keysPressed();
	auto keysDown = im->keysDown();

	auto cursorCaptured = true;
	if (window) {
		cursorCaptured = window->cursorCaptured();
	}

	using Key = DesktopInputManager::Key;
	using MouseButton = DesktopInputManager::MouseButton;

	if (keysPressed.count(Key::Slash)) {
		window->cursorCaptured(!(window->cursorCaptured()));
	}

	if (keysPressed.count(Key::Escape)) {
		window->close();
	}

	using DebugOptions = Scene::DebugOptions;

	if (keysPressed.count(Key::F)) {
		if (util::bitmask::contains(scene.debugOptions(), DebugOptions::ShowWireframes)) {
			scene.debugOptions(util::bitmask::remove(scene.debugOptions(), DebugOptions::ShowWireframes));
		}
		else {
			scene.debugOptions(util::bitmask::add(scene.debugOptions(), DebugOptions::ShowWireframes));
		}
	}
	if (keysPressed.count(Key::B)) {
		if (util::bitmask::contains(scene.debugOptions(), DebugOptions::ShowBoundingBoxes)) {
			scene.debugOptions(util::bitmask::remove(scene.debugOptions(), DebugOptions::ShowBoundingBoxes));
		}
		else {
			scene.debugOptions(util::bitmask::add(scene.debugOptions(), DebugOptions::ShowBoundingBoxes));
		}
	}


	if (cursorCaptured) {

		// move camera

		if (auto pov = scene.visualWorld()->pointOfView().lock()) {

			// look

			vec3 camForward = pov->worldForward();
			vec3 camRight = pov->worldRight();
			vec3 camUp = pov->worldUp();

			// tanA = mouseDelta / distance
			// A = atan(mouseDelta / distance)

			static const float MOUSE_SPEED_SCALAR = .002;
			static const float MOUSE_SPEED = MOUSE_SENSITIVITY * MOUSE_SPEED_SCALAR;

			vec2 mousePositionDelta = im->mousePositionDelta();
			float deltaRotX = math::atan(MOUSE_SPEED * mousePositionDelta.x);
			float deltaRotY = math::atan(MOUSE_SPEED * mousePositionDelta.y);

			vec3 angles = pov->eulerAngles();
			// weird angles
			//_cameraNode->eulerAngles(vec3(angles.x + -deltaRotX, 0, angles.z + deltaRotY));
			// pitch, yaw, roll
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

	if (_pointLightPivotNode) {

		// rotate the duck
		auto rotationDeg = deltaTime * radians(-30.0); // 10deg/sec

		auto duckSpinnerEuler = _pointLightPivotNode->eulerAngles();
		_pointLightPivotNode->eulerAngles(vec3(0, duckSpinnerEuler.y - rotationDeg, 0));
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

/// Private Static Non-Member Functions ///

// nada