//
//  Test.cpp
//	avara-engine
//
//  Created by Morgan Davis on 10/31/17.
//  Copyright © 2017 Morgan K Davis. All rights reserved.
//

#include "Test.h"

#include <iostream>

#include <glm/glm.hpp>

#include "ae.h"
#include "InputManager.h"
#include "Utilities.h"


using namespace ae;
using namespace ae::utils;
using namespace std;
using namespace std::placeholders;
using namespace glm;


#define WINDOW_WIDTH			800
#define WINDOW_HEIGHT			600


/***************************************************************************************
     Public
 ***************************************************************************************/

int Test::run(const vector<string>& args) {
	cout << "Test::run()\n" << endl;
	
	//if (init() != 0) { cout << "Init error!" << endl; return -1; }
	auto window = make_shared<Window>(false, WINDOW_WIDTH, WINDOW_HEIGHT, true);
	window->updateCallback(bind(&Test::windowUpdateCallback, this, _1, _2));
	window->willRenderCallback(bind(&Test::windowWillRenderCallback, this, _1, _2));
	window->didRenderCallback(bind(&Test::windowDidRenderCallback, this, _1, _2));
	window->captureCursor(true);
	window->enableVSync(false);

	auto scene = make_shared<Scene>();
	scene->rootNode(make_shared<Node>("Root node"));


	auto planeGeo = make_shared<Plane>(5.0f, 2.5f);
	auto planeNode = make_shared<Node>();
	planeGeo->name("plane");
	planeNode->geometry(planeGeo);
	scene->rootNode()->addChildNode(planeNode);
	planeNode->rotation(vec4(-1.0f, 0.0f, 0.0f, radians(90.0f)));
	planeNode->position(vec3(0.0f, -2.0f, 0.0f));


	auto boxGeo = make_shared<Box>(3.0f, 2.0f, 1.0f);
	auto boxNode = make_shared<Node>();
	boxGeo->name("box");
	boxNode->geometry(boxGeo);
	scene->rootNode()->addChildNode(boxNode);
	boxNode->rotation(vec4(0.0f, 1.0f, 0.0f, radians(-70.0f)));
	boxNode->position(vec3(2.0f, 0.0f, -2.0f));


	auto sphereGeo = make_shared<Sphere>(0.5, 24);
	auto sphereNode = make_shared<Node>();
	sphereGeo->name("sphere");
	sphereNode->geometry(sphereGeo);
	scene->rootNode()->addChildNode(sphereNode);
	sphereNode->position(vec3(0.0f, 2.0f, 0.0f));
	
	
	auto torusGeo = make_shared<Torus>(0.25f, 1.0f, 64, 128);
	auto torusNode = make_shared<Node>();
	torusGeo->name("torus");
	torusNode->geometry(torusGeo);
	scene->rootNode()->addChildNode(torusNode);
	torusNode->rotation(vec4(0.0f, 1.0f, 0.0f, radians(45.0f)));
	torusNode->position(vec3(-2.0f, 0.0f, -2.0f));
	
	
	auto tubeGeo = make_shared<Tube>(0.5f, 0.75f, 2.0f, 128, 64);
	auto tubeNode = make_shared<Node>();
	tubeGeo->name("tube");
	tubeNode->geometry(tubeGeo);
	scene->rootNode()->addChildNode(tubeNode);
	tubeNode->rotation(vec4(1.0f, -1.0f, 0.0f, radians(-45.0f)));
	tubeNode->position(vec3(0.0f, -1.0f, -2.0f));
	
	
	auto capsuleGeo = make_shared<Capsule>(0.5f, 1.0f, 128, 32, 64);
	auto capsuleNode = make_shared<Node>();
	capsuleGeo->name("capsule");
	capsuleNode->geometry(capsuleGeo);
	scene->rootNode()->addChildNode(capsuleNode);
	//capsuleNode->rotation(vec4(1.0f, 0.0f, 0.0f, radians(-90.0f)));
	capsuleNode->position(vec3(-3.5f, 2.5f, -1.0f));
	
	
	auto cylinderGeo = make_shared<Cylinder>(0.5f, 2.0f, 128, 64);
	auto cylinderNode = make_shared<Node>();
	cylinderGeo->name("cylinder");
	cylinderNode->geometry(cylinderGeo);
	scene->rootNode()->addChildNode(cylinderNode);
	//cylinderNode->rotation(vec4(1.0f, 0.0f, 0.0f, radians(-90.0f)));
	cylinderNode->position(vec3(3.5f, 2.5f, -1.0f));
	
	
	auto coneGeo = make_shared<Cone>(1.0, 2.0f, 128, 64);
	//auto coneGeo = make_shared<Cone>(1.0, 1.0f, 4, 4);
	auto coneNode = make_shared<Node>();
	coneGeo->name("cone");
	coneNode->geometry(coneGeo);
	scene->rootNode()->addChildNode(coneNode);
	//coneNode->rotation(vec4(1.0f, 0.0f, 0.0f, radians(-90.0f)));
	coneNode->position(vec3(-1.5f, 2.5f, -1.0f));
	

	// ******** make everything look like it did before materials worked ********
	
	auto ambientProperty = make_shared<MaterialProperty>(make_shared<Color>(0.75, 0.75, 0.75, 1.0));
	auto diffuseProperty = make_shared<MaterialProperty>(make_shared<Color>(1.0, 1.0, 1.0, 1.0));
	auto material = make_shared<Material>(ambientProperty, diffuseProperty, nullptr);
	material->doubleSided(true);
	
	for (auto n : scene->rootNode()->childNodes(true)) {
		if (n->geometry()) {
			n->geometry()->replaceMaterial(0, material);
		}
	}
	
	auto backgroundColor = make_shared<Color>(109.0f/256.0f, 136.0f/256.0f, 164.0f/256.0f, 1.0f);
	auto background = make_shared<MaterialProperty>(backgroundColor);
	scene->background(background);
	
	// **************************************************************************

	
	

	window->scene(scene);
	m_inputManager = window->inputManager();
	window->display();
	
	return 0;
}

/***************************************************************************************
     Window Callbacks
 ***************************************************************************************/

void Test::windowUpdateCallback(Scene& scene, float time) {
	
	static double previousSeconds = time;
	float deltaSeconds = time - previousSeconds;
	previousSeconds = time;
	
	// get input
	
	auto keysDown = m_inputManager->keysDown();
	
	if (keysDown.count(Key_Escape)) {
		exit(0);
	}
	
	for (auto mb : m_inputManager->mouseButtonsDown()) {
		cout << "Mouse button: " << mb << endl;
	}
	
	vec2 mousePositionDelta = m_inputManager->mousePositionDelta();
	//	if (mousePositionDelta.x || mousePositionDelta.y) {
	//		cout << "Mouse move delta: (" << mousePositionDelta.x << ", " << mousePositionDelta.y << ")" << endl;
	//	}
	
	vec2 mouseScrollWheelDelta = m_inputManager->mouseScrollWheelDelta();
	if (mouseScrollWheelDelta.x || mouseScrollWheelDelta.y) {
		cout << "Mouse scroll wheel delta: (" << mouseScrollWheelDelta.x << ", " << mouseScrollWheelDelta.y << ")" << endl;
	}
	
	// move camera
	
	//const static float mouseSensitivity = 0.5f;
	const static float mouseSensitivity = (1.0f / 0.5f);
	
	if (!m_cameraNode) {
		for (auto n : scene.rootNode()->childNodes(false)) {
			if (n->camera()) {
				m_cameraNode = n;
				break;
			}
		}
	}
	
	if (m_cameraNode) {
		
		// look
		
		vec3 camForward = m_cameraNode->worldForward();
		vec3 camRight = m_cameraNode->worldRight();
		vec3 camUp = m_cameraNode->worldUp();
		
		
		//		float deltaRotX = deltaSeconds * mouseSensitivity * mousePositionDelta.x;
		//		float deltaRotY = deltaSeconds * mouseSensitivity * mousePositionDelta.y;
		
		// tanA = mouseDelta / distance
		// A = atan(mouseDelta / distance)
		
		float deltaRotX = atan(deltaSeconds * mousePositionDelta.x / mouseSensitivity);
		float deltaRotY = atan(deltaSeconds * mousePositionDelta.y / mouseSensitivity);
		
		vec3 angles = m_cameraNode->eulerAngles();
		// weird angles
		//m_cameraNode->eulerAngles(vec3(angles.x + -deltaRotX, 0, angles.z + deltaRotY));
		// pitch, yaw, roll
		m_cameraNode->eulerAngles(vec3(angles.x + deltaRotY, angles.y - deltaRotX, 0));
		
		
		// move
		
		static float MOVE_SPEED = 0;
		if (!MOVE_SPEED) MOVE_SPEED = Max(scene.extent());
		
		if(keysDown.count(Key_W)) {
			vec3 positionDelta = deltaSeconds * MOVE_SPEED * camForward;
			m_cameraNode->position(m_cameraNode->position() + positionDelta);
		}
		else if(keysDown.count(Key_S)) {
			vec3 positionDelta = deltaSeconds * MOVE_SPEED * -camForward;
			m_cameraNode->position(m_cameraNode->position() + positionDelta);
		}
		
		if(keysDown.count(Key_A)) {
			vec3 positionDelta = deltaSeconds * MOVE_SPEED * -camRight;
			m_cameraNode->position(m_cameraNode->position() + positionDelta);
		}
		else if(keysDown.count(Key_D)) {
			vec3 positionDelta = deltaSeconds * MOVE_SPEED * camRight;
			m_cameraNode->position(m_cameraNode->position() + positionDelta);
		}
		
		if(keysDown.count(Key_Space)) {
			vec3 positionDelta = deltaSeconds * MOVE_SPEED * camUp;
			m_cameraNode->position(m_cameraNode->position() + positionDelta);
		}
	}
}

void Test::windowWillRenderCallback(Scene& scene, float time) {

}

void Test::windowDidRenderCallback(Scene& scene, float time) {

}
