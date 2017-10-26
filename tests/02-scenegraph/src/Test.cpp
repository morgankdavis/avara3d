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

//#define GLM_ENABLE_EXPERIMENTAL
//#include <glm/gtx/string_cast.hpp>

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
	
	
#define MATRIX_TEST


#ifdef MATRIX_TEST
	
	// test using raw matrix manipulation
	
	auto aScene = TestSceneNamed("dragon", "obj");
	auto aNode = aScene->rootNode()->immediateChildNodes()[0];
	aNode->name("A");
	auto aTranslate = 	translate(mat4(1.0f), vec3(10.0f, 0.0f, 0.0f));
	auto aScale = 		scale(mat4(1.0f), vec3(2.0f, 2.0f, 2.0f));
	auto aRotate = 		rotate(mat4(1.0f), (float)radians(45.0), vec3(0.0f, 0.0f, 1.0f));
	auto aTIn = 		aRotate * aScale * aTranslate;
	cout << "aTIn:\n" << aTIn << endl;
	aNode->transform(aTIn);
	cout << "aNode transform:\n" << aNode->transform() << endl;
	cout << "aNode orientation: " << aNode->orientation() << endl;
	cout << "aNode rotation: " << aNode->rotation() << endl;
	cout << "aNode eulerAngles: " << aNode->eulerAngles() << endl;


	
	auto bScene = TestSceneNamed("dragon", "obj");
	auto bNode = bScene->rootNode()->immediateChildNodes()[0];
	bNode->name("B");
	auto bTranslate = 	translate(mat4(1.0f), vec3(0.0f, -20.0f, -7.0f));
	auto bScale = 		scale(mat4(1.0f), vec3(1.0f, 1.0f, 1.5f));
	auto bRotate = 		rotate(mat4(1.0f), (float)radians(30.0), vec3(1.0f, 1.0f, 0.0f));
	auto bTIn = 		bRotate * bScale * bTranslate;
	cout << "bTIn:\n" << bTIn << endl;
	bNode->transform(bTIn);
	cout << "bNode transform:\n" << bNode->transform() << endl;
	cout << "bNode orientation: " << bNode->orientation() << endl;
	cout << "bNode rotation: " << bNode->rotation() << endl;
	cout << "bNode eulerAngles: " << bNode->eulerAngles() << endl;
	aNode->addChildNode(bNode);
	
	
	
	
	auto fScene = TestSceneNamed("dragon", "obj");
	auto fNode = fScene->rootNode()->immediateChildNodes()[0];
	fNode->name("F");
	auto fTranslate = 	translate(mat4(1.0f), vec3(-16.0f, 0.0f, -60.0f));
	auto fScale = 		scale(mat4(1.0f), vec3(2.0f, 5.0f, 3.0f));
	auto fRotate = 		rotate(mat4(1.0f), (float)radians(-90.0), vec3(0.0f, 1.0f, 3.0f));
	auto fTIn =			fRotate * fScale * fTranslate;
	cout << "fTIn:\n" << fTIn << endl;
	fNode->transform(fTIn);
	cout << "fNode transform:\n" << fNode->transform() << endl;
	cout << "fNode orientation: " << fNode->orientation() << endl;
	cout << "fNode rotation: " << fNode->rotation() << endl;
	cout << "fNode eulerAngles: " << fNode->eulerAngles() << endl;
	bNode->addChildNode(fNode);
	
	
	
	
	auto pScene = TestSceneNamed("cartoon_palm_tree", "obj");
	auto pNode = pScene->rootNode();
	pNode->name("P");
	auto pTranslate = 	translate(mat4(1.0f), vec3(0.0f, -7.0f, 0.0f));
	auto pScale = 		scale(mat4(1.0f), vec3(1.0f, 1.0f, 10.0f));
	auto pRotate = 		rotate(mat4(1.0f), (float)radians(50.0), vec3(1.0f, 1.0f, 0.0f));
	auto pTIn =			pTranslate * pRotate * pScale;
	cout << "pTIn:\n" << pTIn << endl;
	pNode->transform(pTIn);
	cout << "pNode transform:\n" << pNode->transform() << endl;
	cout << "pNode orientation: " << pNode->orientation() << endl;
	cout << "pNode rotation: " << pNode->rotation() << endl;
	cout << "pNode eulerAngles: " << pNode->eulerAngles() << endl;
	fNode->addChildNode(pNode);
	
	
	
	
	auto gScene = TestSceneNamed("ConvaliaBouquet", "obj");
	auto gNode = gScene->rootNode();
	gNode->name("G");
	auto gTranslate = 	translate(mat4(1.0f), vec3(0.0f, -7.0f, 0.0f));
	auto gScale = 		scale(mat4(1.0f), vec3(1.0f, 1.0f, 1.0f));
	auto gRotate = 		rotate(mat4(1.0f), (float)radians(-50.0), vec3(1.0f, 1.0f, 0.0f));
	auto gTIn =			 gScale * gTranslate * gRotate;
	cout << "gTIn:\n" << gTIn << endl;
	gNode->transform(gTIn);
	cout << "gNode transform:\n" << gNode->transform() << endl;
	cout << "gNode orientation: " << gNode->orientation() << endl;
	cout << "gNode rotation: " << gNode->rotation() << endl;
	cout << "gNode eulerAngles: " << gNode->eulerAngles() << endl;
	aNode->addChildNode(gNode);
	

	
#else
	
	// test using constituent parts manipulation
	
	
#endif
	
	
	
	
	auto scene = make_shared<Scene>();
	scene->rootNode()->addChildNode(aNode);
	
	
	cout << "aNode worldTransform:\n" << aNode->worldTransform() << endl;
	cout << "bNode worldTransform:\n" << bNode->worldTransform() << endl;
	cout << "fNode worldTransform:\n" << fNode->worldTransform() << endl;
	cout << "pNode worldTransform:\n" << pNode->worldTransform() << endl;
	

	auto camera = make_shared<Camera>(0.01f, 1000.0f, 30.0f);
	auto camNode = make_shared<Node>();
	camNode->camera(camera);
	camNode->name("Camera node");
	camNode->position(vec3(0.0f, 5.0f, 100.0f));
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
