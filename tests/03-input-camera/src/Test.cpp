//
//  Test.cpp
//	avara-engine
//
//  Created by Morgan Davis on 10/15/17.
//  Copyright © 2017 Morgan K Davis. All rights reserved.
//

#include "Test.h"

#include <iostream>

#include <glm/gtc/matrix_transform.hpp>

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
     MARK:   Public
 **************************************************************************************/

int Test::run(const vector<string>& args) {
	cout << "Test::run()\n" << endl;

	auto window = Window(false, WINDOW_WIDTH, WINDOW_HEIGHT, true);
	window.updateCallback(bind(&Test::windowUpdateCallback, this, _1, _2));
	window.willRenderCallback(bind(&Test::windowWillRenderCallback, this, _1, _2));
	window.didRenderCallback(bind(&Test::windowDidRenderCallback, this, _1, _2));
	window.captureCursor(true);
	window.enableVSync(false);
	
	auto scene = TestSceneNamed("importTest");
	//auto scene = TestSceneNamed("dragon", "obj");
	
//	auto node = scene->rootNode()->childNodes(false)[4];
//	node->position();

	
//	auto window = Window(WINDOW_WIDTH, WINDOW_HEIGHT, FRAMEBUFFER_SCALE);
//	window.willUpdateCallback(bind(&Test::windowWillUpdateCallback, this, _1, _2));
//	window.didUpdateCallback(bind(&Test::windowDidUpdateCallback, this, _1, _2));
//	window.scene(scene);
//	window.enableCursor(false);


//	for (auto n : scene->rootNode()->childNodes(false)) {
//		if (n->camera()) {
//			m_cameraNode = n;
//			break;
//		}
//	}

	m_suzanneNode = scene->rootNode()->childNode("Suzanne", true);
	//m_suzanneNode = scene->rootNode()->childNodes(true)[0];


	
	// ******** make everything look like it did before materials worked ********
	
	auto ambientProperty = make_shared<MaterialProperty>(make_shared<Color>(0.75, 0.75, 0.75, 1.0));
	auto diffuseProperty = make_shared<MaterialProperty>(make_shared<Color>(1.0, 1.0, 1.0, 1.0));
	auto material = make_shared<Material>(ambientProperty, diffuseProperty, nullptr);
	
	for (auto n : scene->rootNode()->childNodes(true)) {
		if (n->geometry()) {
			n->geometry()->replaceMaterial(0, material);
		}
	}
	
	auto backgroundColor = make_shared<Color>(109.0f/256.0f, 136.0f/256.0f, 164.0f/256.0f, 1.0f);
	auto background = make_shared<MaterialProperty>(backgroundColor);
	scene->background(background);
	
	// **************************************************************************
	

	window.scene(scene);
	m_inputManager = window.inputManager();
	window.display();
	
	return 0;
}

/***************************************************************************************
     MARK:   Window Callbacks
 **************************************************************************************/

void Test::windowUpdateCallback(Scene& scene, float time) {
	
	static double previousSeconds = time;
	float deltaSeconds = time - previousSeconds;
	previousSeconds = time;
	
	// get input
	
	auto keysDown = m_inputManager->keysDown();
	for (auto k : keysDown) {
		//cout << "Key: " << to_string(k) << endl;
		//printf("Key: %c\n", k);
	}
	
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
		cout << "Mouse scroll wheel delta: (" << mouseScrollWheelDelta.x << ", "
		<< mouseScrollWheelDelta.y << ")" << endl;
	}
	
	// move camera

	//const static float mouseSensitivity = 0.5f;
	const static float mouseSensitivity = (1.0f / 1.5f);
	
	
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
		
		// tanA = y/x
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
	
	
	if (m_suzanneNode) {
		if(keysDown.count(Key_Right)) {
			m_suzanneNode->eulerAngles(vec3(m_suzanneNode->eulerAngles().x + deltaSeconds,
											m_suzanneNode->eulerAngles().y,
											m_suzanneNode->eulerAngles().z));
		}
		else if(keysDown.count(Key_Left)) {
			m_suzanneNode->eulerAngles(vec3(m_suzanneNode->eulerAngles().x - deltaSeconds,
											m_suzanneNode->eulerAngles().y, m_suzanneNode->eulerAngles().z));
		}
		if(keysDown.count(Key_Up)) {
			m_suzanneNode->eulerAngles(vec3(m_suzanneNode->eulerAngles().x,
											m_suzanneNode->eulerAngles().y,
											m_suzanneNode->eulerAngles().z + deltaSeconds));
		}
		else if(keysDown.count(Key_Down)) {
			m_suzanneNode->eulerAngles(vec3(m_suzanneNode->eulerAngles().x,
											m_suzanneNode->eulerAngles().y,
											m_suzanneNode->eulerAngles().z - deltaSeconds));
		}
	}
}

void Test::windowWillRenderCallback(Scene& scene, float time) {

}

void Test::windowDidRenderCallback(Scene& scene, float time) {

}
