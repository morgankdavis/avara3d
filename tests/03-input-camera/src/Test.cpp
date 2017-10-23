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


#define FRAMEBUFFER_SCALE       1.0f
#define WINDOW_WIDTH			800
#define WINDOW_HEIGHT			600


/***************************************************************************************
     MARK:   Public
 **************************************************************************************/

int Test::run(const vector<string>& args) {
	cout << "Test::run()\n" << endl;
	
	if (init() != 0) { cout << "Init error!" << endl; return -1; }

	
	auto scene = TestSceneNamed("importTest");
	//auto scene = TestSceneNamed("dragon", "obj");
	
//	auto node = scene->rootNode()->immediateChildNodes()[4];
//	node->position();

	
	auto window = Window(WINDOW_WIDTH, WINDOW_HEIGHT, FRAMEBUFFER_SCALE);
	window.willUpdateCallback(bind(&Test::windowWillUpdateCallback, this, _1, _2));
	window.didUpdateCallback(bind(&Test::windowDidUpdateCallback, this, _1, _2));
	window.scene(scene);
	window.enableCursor(false);
	
	
	
	for (auto n : scene->rootNode()->immediateChildNodes()) {
		if (n->camera()) {
			m_cameraNode = n;
			m_cameraNode->camera()->zNear(0.000000001);
//			m_camRotation = vec3(0.0f, 0.0f, 0.0f);
//			m_cameraNode->transform( translate(mat4(1.0), vec3(0.0f, 0.0f, 0.5f)));
			cout << "camera transform: " << n->transform() << endl;
			break;
		}
	}
	
	
	
	m_inputManager = window.inputManager();
	
	window.display();
	
	return 0;
}







void Test::windowWillUpdateCallback(Scene& scene, float deltaSeconds) {

	static float totalSeconds = 0;
	totalSeconds += deltaSeconds;

	
	
	// get input
	
	auto keysDown = m_inputManager->keysDown();
	for (auto k : keysDown) {
		cout << "Key: " << k << endl;
		//printf("key name: %c\n", k);
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
		cout << "Mouse scroll wheel delta: (" << mouseScrollWheelDelta.x << ", " << mouseScrollWheelDelta.y << ")" << endl;
	}
	
	
	
	// move camera
	
	// TODO: Use trig
	const static float mouseSensitivity = 0.25f;
	
	if (m_cameraNode) {
		
		// look

		vec3 camForward = m_cameraNode->worldFormard();
		vec3 camRight = m_cameraNode->worldRight();
		vec3 camUp = m_cameraNode->worldUp();

		
		float deltaRotX = deltaSeconds * mouseSensitivity * mousePositionDelta.x;
		float deltaRotY = deltaSeconds * mouseSensitivity * mousePositionDelta.y;
		
		vec3 angles = m_cameraNode->eulerAngles();
		cout << "angles: " << angles << endl;
		m_cameraNode->eulerAngles(vec3(angles.x + deltaRotX, angles.y + deltaRotY, angles.z));
		//m_cameraNode->eulerAngles(vec3(angles.x, angles.y, angles.z));

		
		// move

		const static float MOVE_SPEED = 1.0f; // units/sec

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
			vec3 positionDelta = deltaSeconds * MOVE_SPEED * camUp;//normalize(cross(camRight, camForward));
			m_cameraNode->position(m_cameraNode->position() + positionDelta);
		}
	}
}

void Test::windowDidUpdateCallback(Scene& scene, float deltaSeconds) {

}
