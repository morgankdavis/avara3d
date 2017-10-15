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
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include "ae.h"
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
	cout << "Test::run()\n" << endl;
	if (init() != 0) { cout << "Init error!" << endl; return -1; }
	
	
	auto testScene = TestSceneNamed("importTest");
	auto rootImmediateChildren = testScene->rootNode()->immediateChildNodes();
	
	for (auto c : rootImmediateChildren) {
		c->removeFromParentNode();
	}
	
	parentNode = make_shared<Node>();
	parentNode->addChildNodes(rootImmediateChildren);
	testScene->rootNode()->addChildNode(parentNode);
	
	mat4 parentScale = scale(mat4(1.0f), vec3(1.0f, 1.0f, 1.0f) * 25.0f);
	parentNode->transform(parentScale);
	
	//	cout << "test scene nodes:" << endl;
	//	auto testNodes = testScene->rootNode()->allChildNodes();
	//	for (auto node : testNodes) {
	//		cout << "node: " << node->name() << endl;
	//	}
	
	
	
	

	auto siameseScene = TestSceneNamed("siamese");
	
	//	cout << "siamese nodes:" << endl;
	//	for (auto n : siameseScene->rootNode()->allChildNodes()) {
	//		cout << n->name() << endl;
	//	}
	siameseNode = siameseScene->rootNode()->allChildNodes()[4];
	cout << "siameseNode: " << siameseNode->name() << endl;
	siameseNode->transform(scale(mat4(1.0f), vec3(1.0f) * 0.025f));
	siameseNode->hidden(false);
	//parentNode->addChildNode(siameseNode);
	
	
	
	torusNode = testScene->rootNode()->allChildNodes()[8];
	
	
	suzanneNode = testScene->rootNode()->allChildNodes()[7];
	suzanneNode->transform(translate(mat4(1.0f), vec3(-0.25f, 0.0f, 0.0f)));
	suzanneNode->hidden(false);
	suzanneNode->addChildNode(siameseNode);
	
	
	Window window = Window(WINDOW_WIDTH, WINDOW_HEIGHT, FRAMEBUFFER_SCALE);
	window.willUpdateCallback(bind(&Test::windowWillUpdateCallback, this, _1, _2));
	window.didUpdateCallback(bind(&Test::windowDidUpdateCallback, this, _1, _2));
	window.scene(testScene);
	window.display();
	
	return 0;
}

/***************************************************************************************
     MARK:   Window Callbacks
 **************************************************************************************/

void Test::windowWillUpdateCallback(Scene& scene, float deltaSeconds) {
//	cout << "windowWillUpdateCallback(" << &scene << ", "
//	<< deltaSeconds << ")" << endl;
	
	static float totalSeconds = 0;
	totalSeconds += deltaSeconds;
	
	float rotationDeg = deltaSeconds * 30.0; // 30deg/sec
	
	parentNode->transform(rotate(parentNode->transform(),
								 radians(rotationDeg),
								 vec3(0.0f, 1.0f, 0.0f)));
	
	auto siameseRotate = rotate(siameseNode->transform(), radians(rotationDeg * 2.0f), vec3(1.0f, 1.0f, 1.0f));
	siameseNode->transform(siameseRotate);
	
	torusNode->transform(rotate(torusNode->transform(),
								radians(rotationDeg),
								vec3(0.0f, 0.0f, 1.0f)));
	
	//	suzanneNode->transform(rotate(suzanneNode->transform(),
	//								radians(rotationDeg),
	//								vec3(0.0f, 1.0f, 0.0f)));
	
	
	
	//	siameseNode->transform(rotate(siameseNode->transform(),
	//								  radians(rotationDeg * 10.0f),
	//								  vec3(1.0f, 0.0f, 0.0f)));
	
	//	palmNode->transform(rotate(palmNode->transform(),
	//								  radians(rotationDeg),
	//								  vec3(1.0f, 0.0f, 0.0f)));
	//
	//	teapotNode->transform(rotate(teapotNode->transform(),
	//								  radians(rotationDeg),
	//								  vec3(0.0f, 0.0f, 1.0f)));
}

void Test::windowDidUpdateCallback(Scene& scene, float deltaSeconds) {
//	cout << "windowDidUpdateCallback(" << &scene << ", "
//	<< deltaSeconds << ")" << endl;
}
