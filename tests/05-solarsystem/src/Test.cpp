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
	



	auto solarSystemScene = make_shared<Scene>();
	
	auto sunScene = TestSceneNamed("teapot");
	auto sunNode = sunScene->rootNode()->allChildNodes()[1];
	sunNode->scale(vec3(1.0) * 1.00f);
	
	m_earthOrbitNode = make_shared<Node>("earth orbit");
	m_earthOrbitNode->position(vec3(20.0f, 0.0f, 0.0f));
	auto earthScene = TestSceneNamed("teapot");
	m_earthNode = earthScene->rootNode()->allChildNodes()[1];
	m_earthNode->scale(vec3(1.0) * 0.1f);

//	m_moonOrbitNode = make_shared<Node>("moon orbit");
//	m_moonOrbitNode->position(vec3(0.5f, 0.0f, 0.0f));
//	auto moonScene = TestSceneNamed("teapot");
//	m_moonNode = moonScene->rootNode()->allChildNodes()[1];
//	m_moonNode->scale(vec3(1.0) * 0.025f);


	solarSystemScene->rootNode()->addChildNode(sunNode);
	solarSystemScene->rootNode()->addChildNode(m_earthOrbitNode);
	m_earthOrbitNode->addChildNode(m_earthNode);
	//m_earthOrbitNode->addChildNode(m_moonOrbitNode);
	//m_moonOrbitNode->addChildNode(m_moonNode);
	
	


	
	auto window = Window(WINDOW_WIDTH, WINDOW_HEIGHT, FRAMEBUFFER_SCALE);
	window.willUpdateCallback(bind(&Test::windowWillUpdateCallback, this, _1, _2));
	window.didUpdateCallback(bind(&Test::windowDidUpdateCallback, this, _1, _2));
	window.scene(solarSystemScene);
	window.enableCursor(false);

	m_inputManager = window.inputManager();
	
	window.display();
	
	return 0;
}







void Test::windowWillUpdateCallback(Scene& scene, float deltaSeconds) {

	static float totalSeconds = 0;
	totalSeconds += deltaSeconds;

	// scene animation



	static const float EARTH_ORBITAL_PERIOD = 1.00001742096; // hrs
	static const float EARTH_ROTATIONAL_VELOCITY = 1674.4; // km/hr

	auto earthOrbitRotation = vec4(0.0f, 1.0f, 0.0f, m_earthOrbitNode->rotation().w + radians(100.0 * deltaSeconds));
	m_earthOrbitNode->rotation(earthOrbitRotation);




	// get input
	
	auto keysDown = m_inputManager->keysDown();
	vec2 mousePositionDelta = m_inputManager->mousePositionDelta();
	
	if (keysDown.count(Key_Escape)) {
		exit(0);
	}

	// camera
	
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
		
		const static float MOVE_SPEED = 50.0f; // units/sec
		
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
