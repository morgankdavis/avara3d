//
//  Test.cpp
//	avara-engine
//
//  Created by Morgan Davis on 11/19/17.
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


#define FRAMEBUFFER_SCALE       1.0f
#define WINDOW_WIDTH			800
#define WINDOW_HEIGHT			600


/***************************************************************************************
     MARK:   Public
 **************************************************************************************/

int Test::run(const vector<string>& args) {
	cout << "Test::run()\n" << endl;
	
	if (init() != 0) { cout << "Init error!" << endl; return -1; }

//	auto scene = make_shared<Scene>();
	
//	auto siameseScene = TestSceneNamed("siamese");
//	auto siameseNode = siameseScene->rootNode()->childNode("Siamese", true);
//	siameseNode->scale(siameseNode->scale() * 0.015f);
//	siameseNode->position(vec3(5.0, 0, 0));
////	scene->rootNode()->addChildNode(siameseNode);
//	auto scene = siameseScene;
	
	
//
//	auto testScene = TestSceneNamed("materialTest");
//	for (auto node : testScene->rootNode()->allChildNodes()) {
//		cout << "node: " << node->name() << endl;
//	}
////	scene->rootNode()->addChildNodes(testScene->rootNode()->allChildNodes());
//	auto scene = testScene;
	
	
//	auto duckScene = TestSceneNamed("duck");
//	auto scene = duckScene;
	
//	auto palmScene = TestSceneNamed("cartoon_palm_tree");
//	auto scene = palmScene;
	
//	auto importTestScene = TestSceneNamed("importTest");
//	auto scene = importTestScene;
	
//	auto palletScene = TestSceneNamed("Pallet");
//	auto scene = palletScene;
	
//	auto mushroomScene = TestSceneNamed("mushroom");
//	auto scene = mushroomScene;
	
//	auto crocusScene = TestSceneNamed("FL43_Crocus_sp_Crocus_3ds/FL43_1");
//	auto scene = crocusScene;
	
//	auto tree1Scene = TestSceneNamed("tree1");
//	auto scene = tree1Scene;
	
//	auto tunaScene = TestSceneNamed("tuna");
//	auto scene = tunaScene;

//	auto woodContainerScene = TestSceneNamed("WoodContainer");
//	auto scene = woodContainerScene;
	
//	auto crate1Scene = TestSceneNamed("Crate1");
//	auto scene = crate1Scene;

	auto woddenCrateScene = TestSceneNamed("WoddenCrate");
	auto scene = woddenCrateScene;
	
	

	auto window = Window(WINDOW_WIDTH, WINDOW_HEIGHT, FRAMEBUFFER_SCALE);
	window.willUpdateCallback(bind(&Test::windowWillUpdateCallback, this, _1, _2));
	window.didUpdateCallback(bind(&Test::windowDidUpdateCallback, this, _1, _2));
	window.scene(scene);
	window.enableCursor(false);
	window.maximumFramerate(120.0);

	m_inputManager = window.inputManager();
	
	window.display();
	
	return 0;
}







void Test::windowWillUpdateCallback(Scene& scene, float deltaSeconds) {

	static float totalSeconds = 0;
	totalSeconds += deltaSeconds;


	// get input

	auto keysDown = m_inputManager->keysDown();
	if (keysDown.count(Key_Escape)) {
		exit(0);
	}
	vec2 mousePositionDelta = m_inputManager->mousePositionDelta();


	// move camera

	const static float mouseSensitivity = (1.0f / 0.5f);


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

		//const static float MOVE_SPEED = 5.0f; // units/sec
		const static float MOVE_SPEED = Max(scene.extent());

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
