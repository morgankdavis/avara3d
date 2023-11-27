//
//  Example.cc
//	avara-engine
//
//  Created by Morgan Davis on 10/31/17.
//  Copyright © 2017 Morgan K Davis. All rights reserved.
//

#include "Example.h"

#include <iostream>

#include <glm/glm.hpp>

#include "utilities/Utilities.h"


using namespace ae;
using namespace ae::utils;
using namespace example;
using namespace std;
using namespace std::placeholders;
using namespace glm;


constexpr bool					USE_HIGH_DPI =			true;
constexpr unsigned				WINDOW_WIDTH =			1024;
constexpr unsigned				WINDOW_HEIGHT =			768;
constexpr bool					FULLSCREEN =			false;
constexpr ANTIALIASING_MODE		ANTIALIAS_MODE =		ANTIALIASING_MODE::MSAA_4X;
constexpr bool					ENABLE_VSYNC =			false;
constexpr bool					CAPTURE_CURSOR =		true;


/***************************************************************************************
	Public
 ***************************************************************************************/

int Example::run(const vector<string>& args) {
	AE_INIT();

	_logger = make_shared<Logger>("example", Logger::MainLogger()->sinks());
	
	LOG_I(_logger, "Example::run()");

	_window = make_shared<Window>(FULLSCREEN,
								  WINDOW_WIDTH, WINDOW_HEIGHT,
								  USE_HIGH_DPI,
								  ANTIALIAS_MODE,
								  RENDER_API::OPENGL);
	_window->updateCallback(bind(&Example::updateCallback, this, _1, _2));
	_window->willRenderCallback(bind(&Example::willRenderCallback, this, _1, _2));
	_window->didRenderCallback(bind(&Example::didRenderCallback, this, _1, _2));
	_window->enableVSync(ENABLE_VSYNC);
	_window->cursorCaptured(CAPTURE_CURSOR);
	
	auto scene = make_shared<Scene>();
	scene->rootNode(make_shared<Node>("Root node"));

	auto planeGeo = make_shared<Plane>(5.0f, 2.5f);
	auto planeNode = make_shared<Node>();
	planeGeo->name("plane");
	planeNode->geometry(planeGeo);
	scene->rootNode()->addChild(planeNode);
	planeNode->rotation({-1.0f, 0.0f, 0.0f}, radians(90.0f));
	planeNode->position(vec3(0.0f, -2.0f, 0.0f));


	auto boxGeo = make_shared<Box>(3.0f, 2.0f, 1.0f);
	auto boxNode = make_shared<Node>();
	boxGeo->name("box");
	boxNode->geometry(boxGeo);
	scene->rootNode()->addChild(boxNode);
	boxNode->rotation({0.0f, 1.0f, 0.0f}, radians(-70.0f));
	boxNode->position(vec3(2.0f, 0.0f, -2.0f));


	auto sphereGeo = make_shared<Sphere>(0.5, 24);
	auto sphereNode = make_shared<Node>();
	sphereGeo->name("sphere");
	sphereNode->geometry(sphereGeo);
	scene->rootNode()->addChild(sphereNode);
	sphereNode->position(vec3(0.0f, 2.0f, 0.0f));
	
	
	auto torusGeo = make_shared<Torus>(0.75f, 1.0f, 64, 128);
	auto torusNode = make_shared<Node>();
	torusGeo->name("torus");
	torusNode->geometry(torusGeo);
	scene->rootNode()->addChild(torusNode);
	torusNode->rotation({0.0f, 1.0f, 0.0f}, radians(45.0f));
	torusNode->position(vec3(-2.0f, 0.0f, -2.0f));
	
	
	auto tubeGeo = make_shared<Tube>(0.5f, 0.75f, 2.0f, 128, 64);
	auto tubeNode = make_shared<Node>();
	tubeGeo->name("tube");
	tubeNode->geometry(tubeGeo);
	scene->rootNode()->addChild(tubeNode);
	tubeNode->rotation({1.0f, -1.0f, 0.0f}, radians(-45.0f));
	tubeNode->position(vec3(0.0f, -1.0f, -2.0f));
	
	
	auto capsuleGeo = make_shared<Capsule>(0.5f, 1.0f, 128, 32, 64);
	auto capsuleNode = make_shared<Node>();
	capsuleGeo->name("capsule");
	capsuleNode->geometry(capsuleGeo);
	scene->rootNode()->addChild(capsuleNode);
	//capsuleNode->rotation(vec4(1.0f, 0.0f, 0.0f, radians(-90.0f)));
	capsuleNode->position(vec3(-3.5f, 2.5f, -1.0f));
	
	
	auto cylinderGeo = make_shared<Cylinder>(0.5f, 2.0f, 128, 64);
	auto cylinderNode = make_shared<Node>();
	cylinderGeo->name("cylinder");
	cylinderNode->geometry(cylinderGeo);
	scene->rootNode()->addChild(cylinderNode);
	//cylinderNode->rotation(vec4(1.0f, 0.0f, 0.0f, radians(-90.0f)));
	cylinderNode->position(vec3(3.5f, 2.5f, -1.0f));
	
	
	auto coneGeo = make_shared<Cone>(1.0, 2.0f, 128, 64);
	//auto coneGeo = make_shared<Cone>(1.0, 1.0f, 4, 4);
	auto coneNode = make_shared<Node>();
	coneGeo->name("cone");
	coneNode->geometry(coneGeo);
	scene->rootNode()->addChild(coneNode);
	//coneNode->rotation(vec4(1.0f, 0.0f, 0.0f, radians(-90.0f)));
	coneNode->position(vec3(-1.5f, 2.5f, -1.0f));
	

	// ******** make everything look like it did before materials worked ********
	
	auto ambientProperty = make_shared<MaterialProperty>(make_shared<Color>(0.75f, 0.75, 0.75, 1.0));
	auto diffuseProperty = make_shared<MaterialProperty>(make_shared<Color>(1.0f, 1.0, 1.0, 1.0));
	auto material = make_shared<Material>(ambientProperty, diffuseProperty, nullptr);
	material->doubleSided(true);
	
	for (auto n : scene->rootNode()->children(true)) {
		if (n->geometry()) {
			n->geometry()->replaceMaterial(0, material);
		}
	}
	
	auto backgroundColor = make_shared<Color>(109.0f/256.0f, 136.0f/256.0f, 164.0f/256.0f, 1.0f);
	auto background = make_shared<MaterialProperty>(backgroundColor);
	scene->background(background);
	
	// **************************************************************************

	
	

	_window->scene(scene);
	_inputManager = _window->inputManager();
	_window->display();
	
	return 0;
}

/***************************************************************************************
	RenderContext Callbacks
 ***************************************************************************************/

void Example::updateCallback(RenderContext& renderContext, float time) {
	
	static float previousSeconds = time;
	float deltaSeconds = time - previousSeconds;
	previousSeconds = time;
	
	auto scene = renderContext.scene();
	
	// get input
	
	auto keysDown = _inputManager->keysDown();
	
	if (keysDown.count(KEY::ESCAPE)) {
		exit(0);
	}
	
//	for (auto mb : _inputManager->mouseButtonsDown()) {
//		cout << "Mouse button: " << mb << endl;
//	}
	
	vec2 mousePositionDelta = _inputManager->mousePositionDelta();
	//	if (mousePositionDelta.x || mousePositionDelta.y) {
	//		cout << "Mouse move delta: (" << mousePositionDelta.x << ", " << mousePositionDelta.y << ")" << endl;
	//	}
	
//	vec2 mouseScrollWheelDelta = _inputManager->mouseScrollWheelDelta();
//	if (mouseScrollWheelDelta.x || mouseScrollWheelDelta.y) {
//		cout << "Mouse scroll wheel delta: (" << mouseScrollWheelDelta.x << ", " << mouseScrollWheelDelta.y << ")" << endl;
//	}
	
	
	auto keysPressed = _inputManager->keysPressed();
	if (keysPressed.count(KEY::F)) {
		if (DEBUG_OPTIONS_CONTAINS(renderContext.debugOptions(), DEBUG_OPTIONS::SHOW_WIREFRAMES)) {
			renderContext.debugOptions(DEBUG_OPTIONS_REMOVE(renderContext.debugOptions(), DEBUG_OPTIONS::SHOW_WIREFRAMES));
		}
		else {
			renderContext.debugOptions(DEBUG_OPTIONS_ADD(renderContext.debugOptions(), DEBUG_OPTIONS::SHOW_WIREFRAMES));
		}
	}
	if (keysPressed.count(KEY::B)) {
		if (DEBUG_OPTIONS_CONTAINS(renderContext.debugOptions(), DEBUG_OPTIONS::SHOW_BOUNDING_BOXES)) {
			renderContext.debugOptions(DEBUG_OPTIONS_REMOVE(renderContext.debugOptions(), DEBUG_OPTIONS::SHOW_BOUNDING_BOXES));
		}
		else {
			renderContext.debugOptions(DEBUG_OPTIONS_ADD(renderContext.debugOptions(), DEBUG_OPTIONS::SHOW_BOUNDING_BOXES));
		}
	}
	
	
	
	// move camera
	
	//const static float mouseSensitivity = 0.5f;
	const static float mouseSensitivity = (1.0f / 0.5f);
	
	if (!_cameraNode) {
		for (auto n : scene->rootNode()->children(false)) {
			if (n->camera()) {
				_cameraNode = n;
				break;
			}
		}
	}
	
	if (_cameraNode) {
		
		// look
		
		vec3 camForward = _cameraNode->worldForward();
		vec3 camRight = _cameraNode->worldRight();
		vec3 camUp = _cameraNode->worldUp();
		
		
		//		float deltaRotX = deltaSeconds * mouseSensitivity * mousePositionDelta.x;
		//		float deltaRotY = deltaSeconds * mouseSensitivity * mousePositionDelta.y;
		
		// tanA = mouseDelta / distance
		// A = atan(mouseDelta / distance)
		
		float deltaRotX = atan(deltaSeconds * mousePositionDelta.x / mouseSensitivity);
		float deltaRotY = atan(deltaSeconds * mousePositionDelta.y / mouseSensitivity);
		
		vec3 angles = _cameraNode->eulerAngles();
		// weird angles
		//_cameraNode->eulerAngles(vec3(angles.x + -deltaRotX, 0, angles.z + deltaRotY));
		// pitch, yaw, roll
		_cameraNode->eulerAngles(vec3(angles.x + deltaRotY, angles.y - deltaRotX, 0));
		
		
		// move
		
		static float MOVE_SPEED = Max(scene->rootNode()->extent());
		
		if(keysDown.count(KEY::W)) {
			vec3 positionDelta = deltaSeconds * MOVE_SPEED * camForward;
			_cameraNode->position(_cameraNode->position() + positionDelta);
		}
		else if(keysDown.count(KEY::S)) {
			vec3 positionDelta = deltaSeconds * MOVE_SPEED * -camForward;
			_cameraNode->position(_cameraNode->position() + positionDelta);
		}
		
		if(keysDown.count(KEY::A)) {
			vec3 positionDelta = deltaSeconds * MOVE_SPEED * -camRight;
			_cameraNode->position(_cameraNode->position() + positionDelta);
		}
		else if(keysDown.count(KEY::D)) {
			vec3 positionDelta = deltaSeconds * MOVE_SPEED * camRight;
			_cameraNode->position(_cameraNode->position() + positionDelta);
		}
		
		if(keysDown.count(KEY::SPACE)) {
			vec3 positionDelta = deltaSeconds * MOVE_SPEED * camUp;
			_cameraNode->position(_cameraNode->position() + positionDelta);
		}
	}
}

void Example::willRenderCallback(RenderContext& renderContext, float time) {

}

void Example::didRenderCallback(RenderContext& renderContext, float time) {

}
