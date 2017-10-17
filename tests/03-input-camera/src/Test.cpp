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
#define WINDOW_WIDTH			640
#define WINDOW_HEIGHT			480


/***************************************************************************************
     MARK:   Public
 **************************************************************************************/

int Test::run(const vector<string>& args) {
	cout << "Test3::run()\n" << endl;
	
	if (init() != 0) { cout << "Init error!" << endl; return -1; }

	
	auto scene = TestSceneNamed("importTest");
	//	auto scene = TestSceneNamed("dragon", "obj", (SceneLoadingOption)0);
	
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
	
	// TODO: Use trig
	const static float mouseSensitivity = 5.0f;
	
	auto someNode = scene.rootNode()->immediateChildNodes()[4];
	if (m_cameraNode) {
		m_cameraNode->transform( rotate(m_cameraNode->transform(),
										deltaSeconds * mouseSensitivity * mousePositionDelta.x,
										vec3(1.0f, 0.0f, 0.0f)) );
		m_cameraNode->transform( rotate(m_cameraNode->transform(),
										deltaSeconds * mouseSensitivity * mousePositionDelta.y,
										vec3(0.0f, 1.0f, 0.0f)) );

		const static float MOVE_SPEED = 1.0f; // units/sec

		if (keysDown.count(Key_W)) {
			m_cameraNode->transform( translate(m_cameraNode->transform(),
											   vec3(0.0f, 0.0f, deltaSeconds * MOVE_SPEED) ));
		}
		if (keysDown.count(Key_S)) {
			m_cameraNode->transform( translate(m_cameraNode->transform(),
											   vec3(0.0f, 0.0f, deltaSeconds * -MOVE_SPEED) ));
		}
		if (keysDown.count(Key_A)) {
			m_cameraNode->transform( translate(m_cameraNode->transform(),
											   vec3(deltaSeconds * MOVE_SPEED, 0.0f, 0.0f) ));
		}
		if (keysDown.count(Key_D)) {
			m_cameraNode->transform( translate(m_cameraNode->transform(),
											   vec3(deltaSeconds * -MOVE_SPEED, 0.0f, 0.0f) ));
		}
	}
}

void Test::windowDidUpdateCallback(Scene& scene, float deltaSeconds) {

}
