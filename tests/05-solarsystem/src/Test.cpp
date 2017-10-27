//
//  Test.cpp
//	avara-engine
//
//  Created by Morgan Davis on 10/15/17.
//  Copyright © 2017 Morgan K Davis. All rights reserved.
//

#include "Test.h"

#include <iostream>

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
	
	
//	solarSystem
//	|    |
//	|   sun
//	|
//	earthOrbit
//	|    |
//	|  earth
//	|
//	moonOrbit
//	|
//	moon
	
	
	auto scene = make_shared<Scene>();
	
	auto sunScene = TestSceneNamed("sphere", "obj");
	auto sunNode = sunScene->rootNode()->allChildNodes()[0];
	
	auto earthOrbitNode = make_shared<Node>("earth orbit");
	earthOrbitNode->position(vec3(1.0f, 0.0f, 0.0f));
	auto earthScene = TestSceneNamed("sphere", "obj");
	auto earthNode = earthScene->rootNode()->allChildNodes()[0];
	earthNode->scale(vec3(1.0) * 0.25f);
	
	auto moonOrbitNode = make_shared<Node>("moon orbit");
	moonOrbitNode->position(vec3(0.5f, 0.0f, 0.0f));
	auto moonScene = TestSceneNamed("sphere", "obj");
	auto moonNode = moonScene->rootNode()->allChildNodes()[0];
	moonNode->scale(vec3(1.0) * 0.025f);
	
	
	scene->rootNode()->addChildNode(sunNode);
	scene->rootNode()->addChildNode(earthOrbitNode);
	earthOrbitNode->addChildNode(earthNode);
	earthOrbitNode->addChildNode(moonOrbitNode);
	moonOrbitNode->addChildNode(moonNode);
	
	


	
	auto window = Window(WINDOW_WIDTH, WINDOW_HEIGHT, FRAMEBUFFER_SCALE);
	window.willUpdateCallback(bind(&Test::windowWillUpdateCallback, this, _1, _2));
	window.didUpdateCallback(bind(&Test::windowDidUpdateCallback, this, _1, _2));
	window.scene(scene);
	window.enableCursor(false);

	m_inputManager = window.inputManager();
	
	window.display();
	
	return 0;
}







void Test::windowWillUpdateCallback(Scene& scene, float deltaSeconds) {

	static float totalSeconds = 0;
	totalSeconds += deltaSeconds;

	// get input
	
	auto keysDown = m_inputManager->keysDown();
	vec2 mousePositionDelta = m_inputManager->mousePositionDelta();
	
	if (keysDown.count(Key_Escape)) {
		exit(0);
	}
	
	
	const static float mouseSensitivity = (1.0f / 1.5f);
	
	
	if (!m_cameraNode) {
		for (auto n : scene.rootNode()->immediateChildNodes()) {
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
		
		float deltaRotX = atan(deltaSeconds * mousePositionDelta.x / mouseSensitivity);
		float deltaRotY = atan(deltaSeconds * mousePositionDelta.y / mouseSensitivity);
		
		vec3 angles = m_cameraNode->eulerAngles();
		m_cameraNode->eulerAngles(vec3(angles.x + deltaRotY, angles.y - deltaRotX, 0));
		
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
			vec3 positionDelta = deltaSeconds * MOVE_SPEED * camUp;
			m_cameraNode->position(m_cameraNode->position() + positionDelta);
		}
	}
}

void Test::windowDidUpdateCallback(Scene& scene, float deltaSeconds) {

}
