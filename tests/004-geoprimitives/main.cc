//
//  main.cpp
//  avara3d
//
//  Created by Morgan Davis on 10/15/17.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#include <memory>
#include <string>
#include <utility>

#include "glm/glm.hpp"

#include "a3d/a3d.h"
#include "a3d/Utilities.h"


using namespace a3d;
using namespace glm;
using namespace std;
using namespace std::placeholders;


constexpr LogLevel				A3D_APP_LOG_LEVEL =		LogLevel::Debug;
constexpr uvec2					WINDOW_SIZE =			{1280, 768};
constexpr bool					FULLSCREEN =			false;
constexpr bool					ENABLE_HIGH_DPI =		true;
constexpr AntialiasingMode		ANTIALIAS_MODE =		AntialiasingMode::Msaa4X;
constexpr bool					ENABLE_VSYNC =			false;
constexpr bool					CAPTURE_CURSOR =		false;
constexpr float					MOUSE_SENSITIVITY =		0.5;


void UpdateCallback(Scene& scene, double time, double deltaTime);
void WillRenderCallback(VisualWorld& world, double time, double deltaTime);
void DidRenderCallback(VisualWorld& world, double time, double deltaTime);


void InitLog();
void LogBuildInfo();


std::unique_ptr<a3d::Logger>		g_logger;
std::shared_ptr<a3d::Node>			g_pointLightPivotNode;


int main(int argc, const char* argv[]) {

	try {
		InitLog();
		LogBuildInfo();

		auto window = make_unique<Window>(RenderingApi::OpenGL,
										  *utils::ExecutableName(),
										  WINDOW_SIZE,
										  FULLSCREEN,
										  ENABLE_HIGH_DPI,
										  ANTIALIAS_MODE);
		window->vSyncEnabled(ENABLE_VSYNC);
		window->cursorCaptured(CAPTURE_CURSOR);

		auto inputManager = make_unique<WindowInputManager>(window.get());
		if (inputManager->errorMask() == WindowInputManagerErrorMask::PermissionDenied) {
			A3D_APP_LOG_E(g_logger, "WindowInputManager permission denied.");
			// on macOS 10.15 Catalina+, this is probably a permissions issue,
			// and the OS will alert the user.
			// just keep going and let the user decide what they want to do.
		}

		auto visualWorld = make_unique<VisualWorld>(*window);
		//auto backgroundColor = make_shared<Color>(109.0f/255.0f, 136.0f/255.0f, 164.0f/255.0f, 1.0f);
		visualWorld->background(Color::Black());
		visualWorld->willRender(bind(&WillRenderCallback, _1, _2, _3));
		visualWorld->didRender(bind(&DidRenderCallback, _1, _2, _3));

		auto scene = make_unique<Scene>(std::move(visualWorld), nullptr, std::move(inputManager));
		scene->debugOptions(DebugOptions::ShowStatsOverlay);
		scene->update(bind(&UpdateCallback, _1, _2, _3));

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
		g_pointLightPivotNode = Node::NamedNode("point light pivot");
		g_pointLightPivotNode->addChild(pointLightNode);
		scene->rootNode()->addChild(g_pointLightPivotNode);


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
			auto mesh = Disk::Mesh(2.5f, 5.0f);
			auto node = make_shared<Node>();
			node->name("disk");
			node->mesh(mesh);
			scene->rootNode()->addChild(node);
			node->position(vec3(0.0f, 5.0f, 0.0f));
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
			node->position(vec3(-5.0f, 2.5f, 0.0f));
		}

		{
			auto mesh = Sphere::Mesh(1.0f);
			auto node = make_shared<Node>();
			mesh->name("sphere");
			node->mesh(mesh);
			scene->rootNode()->addChild(node);
			node->position(vec3(5.0f, 2.5f, 0.0f));
		}

		{
			auto mesh = Spring::Mesh(0.2f, 0.5f, 2.5f);
			auto node = make_shared<Node>();
			mesh->name("spring");
			node->mesh(mesh);
			scene->rootNode()->addChild(node);
			node->position(vec3(0.0f, 2.5f, 0.0f));
			node->rotation({0.0f, 1.0f, 0.0f}, radians(-90.0f));
		}

		{
			auto mesh = Torus::Mesh(0.75f, 1.0f);
			auto node = make_shared<Node>();
			mesh->name("torus");
			node->mesh(mesh);
			scene->rootNode()->addChild(node);
			//node->rotation({0.0f, 1.0f, 0.0f}, radians(45.0f));
			node->position(vec3(5.0f, 0.0f, 0.0f));
		}

		{
			auto mesh = TorusKnot::Mesh(2, 3);
			auto node = make_shared<Node>();
			mesh->name("torus knot");
			node->mesh(mesh);
			scene->rootNode()->addChild(node);
			node->rotation({0.0f, 1.0f, 0.0f}, radians(45.0f));
			node->position(vec3(0.0f, 0.0f, 0.0f));
		}

		{
			auto mesh = Tube::Mesh(0.5f, 0.75f, 2.0f);
			auto node = make_shared<Node>();
			mesh->name("tube");
			node->mesh(mesh);
			scene->rootNode()->addChild(node);
//			node->rotation({1.0f, -1.0f, 0.0f}, radians(-45.0f));
//			node->position(vec3(-1.67f, -2.5f, 0.0f));
			node->position(vec3(-5.0f, 0.0f, 0.0f));
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


		window->center();
		window->open();
		scene->run();
	}
	catch (Exception& e)
	{
		A3D_APP_LOG_F(g_logger, "Exception: {}", e.what());
		return -1;
	}

	return 0;
}

/***************************************************************************************
	Scene Callbacks
 ***************************************************************************************/

void UpdateCallback(Scene& scene, double time, double deltaTime) {
	A3D_APP_LOG_T(g_logger, "scene: {:p}, time: {}, deltaTime: {}", (void*)&scene, time, deltaTime);

	auto inputManager = scene.inputManager();

	Window* window = nullptr;
	if (scene.visualWorld()) {
		window = dynamic_cast<Window*>(scene.visualWorld()->renderContext());
	}

	auto keysPressed = scene.inputManager()->keysPressed();

	// get input

	auto keysDown = scene.inputManager()->keysDown();

	auto cursorCaptured = true;
	if (window) {
		cursorCaptured = window->cursorCaptured();
	}

	if (keysPressed.count(Key::Slash)) {
		window->cursorCaptured(!(window->cursorCaptured()));
	}

	if (keysPressed.count(Key::Escape)) {
		window->close();
	}

	if (keysPressed.count(Key::F)) {
		if (A3D_MASK_CONTAINS(scene.debugOptions(), DebugOptions::ShowWireframes)) {
			scene.debugOptions(A3D_MASK_REMOVE(scene.debugOptions(), DebugOptions::ShowWireframes));
		}
		else {
			scene.debugOptions(A3D_MASK_ADD(scene.debugOptions(), DebugOptions::ShowWireframes));
		}
	}
	if (keysPressed.count(Key::B)) {
		if (A3D_MASK_CONTAINS(scene.debugOptions(), DebugOptions::ShowBoundingBoxes)) {
			scene.debugOptions(A3D_MASK_REMOVE(scene.debugOptions(), DebugOptions::ShowBoundingBoxes));
		}
		else {
			scene.debugOptions(A3D_MASK_ADD(scene.debugOptions(), DebugOptions::ShowBoundingBoxes));
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

			vec2 mousePositionDelta = scene.inputManager()->mousePositionDelta();
			float deltaRotX = atan(MOUSE_SPEED * mousePositionDelta.x);
			float deltaRotY = atan(MOUSE_SPEED * mousePositionDelta.y);

			vec3 angles = pov->eulerAngles();
			// weird angles
			//_cameraNode->eulerAngles(vec3(angles.x + -deltaRotX, 0, angles.z + deltaRotY));
			// pitch, yaw, roll
			pov->eulerAngles(vec3(angles.x + deltaRotY, angles.y - deltaRotX, 0));


			// move

			static float MOVE_SPEED = utils::Max(scene.rootNode()->extent());

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

	if (g_pointLightPivotNode) {

		// rotate the duck
		auto rotationDeg = deltaTime * radians(-30.0); // 10deg/sec

		auto duckSpinnerEuler = g_pointLightPivotNode->eulerAngles();
		g_pointLightPivotNode->eulerAngles({0, duckSpinnerEuler.y - rotationDeg, 0});
	}
}

/***************************************************************************************
	VisualWorld Callbacks
 ***************************************************************************************/

void WillRenderCallback(VisualWorld& world, double time, double deltaTime) {
	A3D_APP_LOG_T(g_logger, "world: {:p}, time: {}, deltaTime: {}", (void*)&world, time, deltaTime);
}

void DidRenderCallback(VisualWorld& world, double time, double deltaTime) {
	A3D_APP_LOG_T(g_logger, "world: {:p}, time: {}, deltaTime: {}", (void*)&world, time, deltaTime);
}

/***************************************************************************************
	Static
 ***************************************************************************************/

void InitLog() {

	string executableName = *utils::ExecutableName();
	auto nativeSink = make_unique<StdOutLoggerSink>();
	auto fileSink = make_unique<FileLoggerSink>(*(utils::ExecutableDirectory())
												/ (executableName + string(".log")));
	auto sinks = unordered_set<unique_ptr<LoggerSink>>();
	sinks.insert(std::move(nativeSink));
	sinks.insert(std::move(fileSink));

	g_logger = make_unique<Logger>(executableName, std::move(sinks));
	g_logger->level(A3D_APP_LOG_LEVEL);

	Logger::MainLogger().level(A3D_APP_LOG_LEVEL);
}

void LogBuildInfo() {

	auto buildInfo = BuildInfo::Info();
	auto version = buildInfo.version();
	A3D_APP_LOG_I(g_logger, "A3D version: {}.{}.{}", version.major, version.minor, version.patch);
	A3D_APP_LOG_I(g_logger, "Build: {}", buildInfo.number());
	A3D_APP_LOG_I(g_logger, "Type: {}", buildInfo.type() == BuildInfo::Type::Debug ? "Debug" : "Release");
	A3D_APP_LOG_I(g_logger, "Origin: {}", buildInfo.origin() == BuildInfo::Origin::CI ? "CI" : "AdHoc");
}
