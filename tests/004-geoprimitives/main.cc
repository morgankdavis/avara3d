//
//  main.cpp
//	avara3d
//
//  Created by Morgan Davis on 10/15/17.
//  Copyright © 2017 Morgan K Davis. All rights reserved.
//

#include <memory>
#include <string>

#include "glm/glm.hpp"

#include "a3d/a3d.h"
#include "a3d/Utilities.h"


using namespace a3d;
using namespace a3d::utils;
using namespace glm;
using namespace std;
using namespace std::placeholders;


void UpdateCallback(Scene& scene, float time);
void WillRenderCallback(VisualWorld& world, float time);
void DidRenderCallback(VisualWorld& world, float time);


constexpr bool					USE_HIGH_DPI =			false;
constexpr unsigned				WINDOW_WIDTH =			1024;
constexpr unsigned				WINDOW_HEIGHT =			768;
constexpr bool					FULLSCREEN =			false;
constexpr AntialiasingMode		ANTIALIAS_MODE =		AntialiasingMode::Msaa4X;
constexpr bool					ENABLE_VSYNC =			false;
constexpr bool					CAPTURE_CURSOR =		true;
constexpr float					MOUSE_SENSITIVITY =		0.5;


std::shared_ptr<a3d::Logger>		logger;


int main(int argc, const char* argv[]) {

	logger = make_shared<Logger>("test-004", Logger::MainLogger()->sinks());
	LOG_I(logger, "");

	auto window = make_shared<Window>(RenderingApi::OpenGL,
									  *utils::ExecutableName(),
									  WINDOW_WIDTH,
									  WINDOW_HEIGHT,
									  FULLSCREEN,
									  USE_HIGH_DPI,
									  ANTIALIAS_MODE);
	window->vSyncEnabled(ENABLE_VSYNC);
	window->cursorCaptured(CAPTURE_CURSOR);

	auto visualWorld = make_shared<VisualWorld>(window);
	auto backgroundColor = make_shared<Color>(109.0f/255.0f, 136.0f/255.0f, 164.0f/255.0f, 1.0f);
	visualWorld->background(backgroundColor);
	visualWorld->willRender(bind(&WillRenderCallback, _1, _2));
	visualWorld->didRender(bind(&DidRenderCallback, _1, _2));

	auto inputManager = make_shared<WindowInputManager>(window);

	auto scene = make_shared<Scene>(visualWorld, nullptr, inputManager);
	scene->debugOptions(DebugOptions::ShowStatsOverlay);
	scene->update(bind(&UpdateCallback, _1, _2));

	auto planeGeo = make_shared<Plane>(5.0f, 2.5f);
	auto planeNode = make_shared<Node>();
	planeGeo->name("plane");
	planeNode->mesh(planeGeo);
	scene->rootNode()->addChild(planeNode);
	planeNode->rotation({-1.0f, 0.0f, 0.0f}, radians(90.0f));
	planeNode->position(vec3(0.0f, -2.0f, 0.0f));

	auto boxGeo = make_shared<Box>(3.0f, 2.0f, 1.0f);
	auto boxNode = make_shared<Node>();
	boxGeo->name("box");
	boxNode->mesh(boxGeo);
	scene->rootNode()->addChild(boxNode);
	boxNode->rotation({0.0f, 1.0f, 0.0f}, radians(-70.0f));
	boxNode->position(vec3(2.0f, 0.0f, -2.0f));

	auto sphereGeo = make_shared<Sphere>(0.5, 24);
	auto sphereNode = make_shared<Node>();
	sphereGeo->name("sphere");
	sphereNode->mesh(sphereGeo);
	scene->rootNode()->addChild(sphereNode);
	sphereNode->position(vec3(0.0f, 2.0f, 0.0f));

	auto torusGeo = make_shared<Torus>(0.75f, 1.0f, 64, 128);
	auto torusNode = make_shared<Node>();
	torusGeo->name("torus");
	torusNode->mesh(torusGeo);
	scene->rootNode()->addChild(torusNode);
	torusNode->rotation({0.0f, 1.0f, 0.0f}, radians(45.0f));
	torusNode->position(vec3(-2.0f, 0.0f, -2.0f));

	auto tubeGeo = make_shared<Tube>(0.5f, 0.75f, 2.0f, 128, 64);
	auto tubeNode = make_shared<Node>();
	tubeGeo->name("tube");
	tubeNode->mesh(tubeGeo);
	scene->rootNode()->addChild(tubeNode);
	tubeNode->rotation({1.0f, -1.0f, 0.0f}, radians(-45.0f));
	tubeNode->position(vec3(0.0f, -1.0f, -2.0f));

	auto capsuleGeo = make_shared<Capsule>(0.5f, 1.0f, 128, 32, 64);
	auto capsuleNode = make_shared<Node>();
	capsuleGeo->name("capsule");
	capsuleNode->mesh(capsuleGeo);
	scene->rootNode()->addChild(capsuleNode);
	//capsuleNode->rotation(vec4(1.0f, 0.0f, 0.0f, radians(-90.0f)));
	capsuleNode->position(vec3(-3.5f, 2.5f, -1.0f));

	auto cylinderGeo = make_shared<Cylinder>(0.5f, 2.0f, 128, 64);
	auto cylinderNode = make_shared<Node>();
	cylinderGeo->name("cylinder");
	cylinderNode->mesh(cylinderGeo);
	scene->rootNode()->addChild(cylinderNode);
	//cylinderNode->rotation(vec4(1.0f, 0.0f, 0.0f, radians(-90.0f)));
	cylinderNode->position(vec3(3.5f, 2.5f, -1.0f));

	auto coneGeo = make_shared<Cone>(1.0, 2.0f, 128, 64);
	//auto coneGeo = make_shared<Cone>(1.0, 1.0f, 4, 4);
	auto coneNode = make_shared<Node>();
	coneGeo->name("cone");
	coneNode->mesh(coneGeo);
	scene->rootNode()->addChild(coneNode);
	//coneNode->rotation(vec4(1.0f, 0.0f, 0.0f, radians(-90.0f)));
	coneNode->position(vec3(-1.5f, 2.5f, -1.0f));


//	int texIndex = 0;
//	vector<shared_ptr<Image>> textures = { utils::ImageNamed("test_textures/blue", "png"),
//										   utils::ImageNamed("test_textures/cyan", "png"),
//										   utils::ImageNamed("test_textures/green", "png"),
//										   utils::ImageNamed("test_textures/magenta", "png"),
//										   utils::ImageNamed("test_textures/orange", "png"),
//										   utils::ImageNamed("test_textures/purple", "png"),
//										   utils::ImageNamed("test_textures/red", "png"),
//										   utils::ImageNamed("test_textures/yellow", "png") };
//
//	for (auto& node : scene->rootNode()->children(true)) {
//		if (auto geometry = node->geometry(); geometry) {
//
//			auto elements = geometry->elements();
//			for (int e=0; e<elements.size(); ++e) {
//
//				auto material = make_shared<Material>();
//				MaterialProperty property = make_shared<Texture>(textures[texIndex++]);
//				material->diffuse(property);
//				material->doubleSided(true);
//				geometry->addMaterial(material);
//				if (texIndex >= textures.size()) {
//					texIndex = 0;
//				}
//			}
//		}
//	}


	window->open();
	scene->run();

	return 0;
}

/***************************************************************************************
	Scene Callbacks
 ***************************************************************************************/

void UpdateCallback(Scene& scene, float time) {
	LOG_T(logger, "scene: {:p}, time: {}", (void*)&scene, time);

	static float previousSeconds = time;
	float deltaSeconds = time - previousSeconds;
	previousSeconds = time;

	// get input

	auto keysDown = scene.inputManager()->keysDown();

	if (keysDown.count(Key::Escape)) {
		exit(0);
	}

//	for (auto mb : _inputManager->mouseButtonsDown()) {
//		cout << "Mouse button: " << mb << endl;
//	}

	vec2 mousePositionDelta = scene.inputManager()->mousePositionDelta();
	//	if (mousePositionDelta.x || mousePositionDelta.y) {
	//		cout << "Mouse move delta: (" << mousePositionDelta.x << ", " << mousePositionDelta.y << ")" << endl;
	//	}

//	vec2 mouseScrollWheelDelta = _inputManager->mouseScrollWheelDelta();
//	if (mouseScrollWheelDelta.x || mouseScrollWheelDelta.y) {
//		cout << "Mouse scroll wheel delta: (" << mouseScrollWheelDelta.x << ", " << mouseScrollWheelDelta.y << ")" << endl;
//	}


	auto keysPressed = scene.inputManager()->keysPressed();
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



	// move camera

	auto pov = scene.visualWorld()->pointOfView();
	if (pov) {

		// look

		vec3 camForward = pov->worldForward();
		vec3 camRight = pov->worldRight();
		vec3 camUp = pov->worldUp();

		// tanA = mouseDelta / distance
		// A = atan(mouseDelta / distance)

		static const float MOUSE_SPEED_SCALAR = .002;
		static const float MOUSE_SPEED = MOUSE_SENSITIVITY * MOUSE_SPEED_SCALAR;

		float deltaRotX = atan(MOUSE_SPEED * mousePositionDelta.x);
		float deltaRotY = atan(MOUSE_SPEED * mousePositionDelta.y);

		vec3 angles = pov->eulerAngles();
		// weird angles
		//_cameraNode->eulerAngles(vec3(angles.x + -deltaRotX, 0, angles.z + deltaRotY));
		// pitch, yaw, roll
		pov->eulerAngles(vec3(angles.x + deltaRotY, angles.y - deltaRotX, 0));


		// move

		static float MOVE_SPEED = Max(scene.rootNode()->extent());

		if(keysDown.count(Key::W)) {
			vec3 positionDelta = deltaSeconds * MOVE_SPEED * camForward;
			pov->position(pov->position() + positionDelta);
		}
		else if(keysDown.count(Key::S)) {
			vec3 positionDelta = deltaSeconds * MOVE_SPEED * -camForward;
			pov->position(pov->position() + positionDelta);
		}

		if(keysDown.count(Key::A)) {
			vec3 positionDelta = deltaSeconds * MOVE_SPEED * -camRight;
			pov->position(pov->position() + positionDelta);
		}
		else if(keysDown.count(Key::D)) {
			vec3 positionDelta = deltaSeconds * MOVE_SPEED * camRight;
			pov->position(pov->position() + positionDelta);
		}

		if(keysDown.count(Key::Space)) {
			vec3 positionDelta = deltaSeconds * MOVE_SPEED * camUp;
			pov->position(pov->position() + positionDelta);
		}
	}
}

/***************************************************************************************
	VisualWorld Callbacks
 ***************************************************************************************/

void WillRenderCallback(VisualWorld& world, float time) {
	LOG_T(logger, "world: {:p}, time: {}", (void*)&world, time);
}

void DidRenderCallback(VisualWorld& world, float time) {
	LOG_T(logger, "world: {:p}, time: {}", (void*)&world, time);
}
