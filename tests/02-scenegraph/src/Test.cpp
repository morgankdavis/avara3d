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
#define DEFAULT_WIDTH			800
#define DEFAULT_HEIGHT			600


/***************************************************************************************
     MARK:   Public
 **************************************************************************************/

int Test::run(const vector<string>& args) {
	cout << "Test::run()\n" << endl;
	
	if (init() != 0) { cout << "Init error!" << endl; return -1; }

	
	// BoxA: load, translate +10x, scale 2xyz, rotate 45deg on z
	
	auto boxAScene = TestSceneNamed("box");
	auto boxANode = boxAScene->rootNode()->allChildNodes()[2];
	boxANode->name("A");
	auto boxATranslate = 	translate(mat4(1.0f), vec3(10.0f, 0.0f, 0.0f));
	auto boxAScale = 		scale(mat4(1.0f), vec3(2.0f, 2.0f, 2.0f));
	auto boxARotate = 		rotate(mat4(1.0f), (float)radians(45.0), vec3(0.0f, 0.0f, 1.0f));
	//boxANode->transform((boxATranslate * boxAScale) * boxARotate);
	boxANode->transform(boxARotate * boxAScale * boxATranslate);
	
	//	cout << "boxATranslate:\n" << boxATranslate << endl;
	//	cout << "boxAScale:\n" << boxAScale << endl;
	//	cout << "boxARotate:\n" << boxARotate << endl;
	//	cout << "boxANode->transform:\n" << boxANode->transform() << endl;
	
	
	// BoxB: load, translate -10x, -5y : attach to BoxA
	
	auto boxBScene = TestSceneNamed("box");
	auto boxBNode = boxBScene->rootNode()->allChildNodes()[2];
	boxBNode->name("B");
	auto boxBTranslate = translate(mat4(1.0f), vec3(-15.0f, -5.0f, 0.0f));
	boxBNode->transform(boxBTranslate);
	boxANode->addChildNode(boxBNode);
	
	
	// BoxC: load, translate -10z : attach to BoxB
	
	auto boxCScene = TestSceneNamed("box");
	auto boxCNode = boxCScene->rootNode()->allChildNodes()[2];
	boxCNode->name("C");
	auto boxCTranslate = translate(mat4(1.0f), vec3(0.0f, 0.0f, -10.0f));
	boxCNode->transform(boxCTranslate);
	boxBNode->addChildNode(boxCNode);
	
	
	
	
	auto scene = make_shared<Scene>();
	scene->rootNode()->addChildNode(boxANode);
	
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
