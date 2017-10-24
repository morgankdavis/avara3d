//
//  Test.cpp
//	avara-engine
//
//  Created by Morgan Davis on 10/15/17.
//  Copyright © 2017 Morgan K Davis. All rights reserved.
//

#include "Test.h"

#include <iostream>
#include <memory>

#include <glm/gtx/string_cast.hpp>

#include "ae.h"
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

	
	// BoxA: load, translate +10x, scale 2xyz, rotate 45deg on z
	
	auto boxAScene = TestSceneNamed("dragon", "obj");
	//auto boxANode = boxAScene->rootNode()->childNode("teapot", false);
	auto boxANode = boxAScene->rootNode()->immediateChildNodes()[0];
	boxANode->name("A");
	
	cout << "boxANode: " << boxANode->transform() << endl;
	
//	auto boxATranslate = 	translate(mat4(1.0f), vec3(10.0f, 0.0f, 0.0f));
//	auto boxAScale = 		scale(mat4(1.0f), vec3(2.0f, 2.0f, 2.0f));
//	auto boxARotate = 		rotate(mat4(1.0f), (float)radians(45.0), vec3(0.0f, 0.0f, 1.0f));
	//boxANode->transform(boxARotate * boxAScale * boxATranslate);
	//boxANode->transform(boxATranslate * boxAScale * boxARotate);
	
	//	cout << "boxATranslate:\n" << boxATranslate << endl;
	//	cout << "boxAScale:\n" << boxAScale << endl;
	//	cout << "boxARotate:\n" << boxARotate << endl;
	//	cout << "boxANode->transform:\n" << boxANode->transform() << endl;
	
	
	// BoxB: load, translate -10x, -5y : attach to BoxA
	
//	auto boxBScene = TestSceneNamed("teapot");
//	auto boxBNode = boxBScene->rootNode()->childNode("teapot", false);
//	boxBNode->name("B");
//	auto boxBTranslate = translate(mat4(1.0f), vec3(-15.0f, -5.0f, 0.0f));
//	boxBNode->transform(boxBTranslate);
//	boxANode->addChildNode(boxBNode);
	
	
	// BoxC: load, translate -10z : attach to BoxB
	
//	auto boxCScene = TestSceneNamed("teapot");
//	auto boxCNode = boxCScene->rootNode()->childNode("teapot", false);
//	boxCNode->name("C");
//	auto boxCTranslate = translate(mat4(1.0f), vec3(0.0f, 0.0f, -10.0f));
//	boxCNode->transform(boxCTranslate);
//	boxBNode->addChildNode(boxCNode);
	
	
	
	
	auto scene = make_shared<Scene>();
	scene->rootNode()->addChildNode(boxANode);
	
	
	auto camera = make_shared<Camera>(0.01f, 1000.0f, 30.0f);
	auto camNode = make_shared<Node>();
	camNode->camera(camera);
	camNode->name("Camera node");
	camNode->position(vec3(0.0f, 5.0f, 50.0f));
	scene->rootNode()->addChildNode(camNode);
	
	
	
	
	Window window = Window(WINDOW_WIDTH, WINDOW_HEIGHT, FRAMEBUFFER_SCALE);
	window.willUpdateCallback(bind(&Test::windowWillUpdateCallback, this, _1, _2));
	window.didUpdateCallback(bind(&Test::windowDidUpdateCallback, this, _1, _2));
	window.scene(scene);
	window.display();
	
	return 0;
}

void Test::windowWillUpdateCallback(Scene& scene, float deltaSeconds) {

}

void Test::windowDidUpdateCallback(Scene& scene, float deltaSeconds) {

}
