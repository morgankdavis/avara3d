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
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

#include "ae.h"
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
	//if (init() != 0) { cout << "Init error!" << endl; return -1; }
	
	
	auto window = Window(false, WINDOW_WIDTH, WINDOW_HEIGHT, true);
	window.willUpdateCallback(bind(&Test::windowWillUpdateCallback, this, _1, _2));
	window.didUpdateCallback(bind(&Test::windowDidUpdateCallback, this, _1, _2));
	window.enableCursor(true);
	window.vSyncEnabled(false);
	
	
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
	

	cout << "Loading siamese scene..." << endl;
	auto siameseScene = TestSceneNamed("siamese");
	siameseNode = siameseScene->rootNode()->immediateChildNodes()[2];
	siameseNode->name("Siamese");
	cout << "siameseNode: " << *siameseNode->name() << endl;
	//siameseNode->transform(scale(siameseNode->transform(), vec3(1.0f) * 0.001f));
	siameseNode->scale(siameseNode->scale() * 0.001f);
	siameseNode->hidden(false);

	
	torusNode = testScene->rootNode()->allChildNodes()[6];
	torusNode->name("torus");
	
	
	coneNode = testScene->rootNode()->allChildNodes()[8];
	coneNode->name("cone");
	
	
	ballNode = testScene->rootNode()->allChildNodes()[4];
	ballNode->name("ball");

	
	auto palmScene = TestSceneNamed("cartoon_palm_tree", "obj");
	palmNode = palmScene->rootNode();
	palmNode->name("palm");
	cout << "palmNode: " << *palmNode->name() << endl;
	//palmNode->transform(scale(mat4(1.0f), vec3(1.0f) * 0.025f));
	palmNode->scale(vec3(1.0f) * 0.025f);
	palmNode->hidden(false);
	parentNode->addChildNode(palmNode);
	
	
	auto teapotScene = TestSceneNamed("teapot");
	teapotNode = teapotScene->rootNode();
	//teapotNode = teapotScene->rootNode()->immediateChildNodes()[1];
	teapotNode->name("teapot");
	cout << "teapotNode: " << *teapotNode->name() << endl;
	auto teapotTranslate = translate(mat4(1.0f), vec3(0.5f, -0.5f, 0.0f));
	auto teapotScale = scale(mat4(1.0f), vec3(1.0f) * 0.005f);
	teapotNode->transform(teapotTranslate * teapotScale);
	teapotNode->hidden(false);
	parentNode->addChildNode(teapotNode);
	
	
	auto backgroundColor = make_shared<Color>(Color::White());
	auto background = make_shared<MaterialProperty>(backgroundColor);
	testScene->background(background);
	

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
	
//	auto siameseRotate = rotate(siameseNode->transform(), radians(rotationDeg * 2.0f), vec3(1.0f, 1.0f, 1.0f));
//	siameseNode->transform(siameseRotate);
	siameseNode->rotation(vec4(0.0f, 1.0f, 1.0f, siameseNode->rotation().w + radians(rotationDeg * 2.0f)));
	
	torusNode->transform(rotate(torusNode->transform(),
							   radians(rotationDeg),
							   vec3(1.0f, 0.0f, 1.0f)));
	
	coneNode->transform(rotate(coneNode->transform(),
								radians(rotationDeg),
								vec3(0.0f, 0.0f, 1.0f)));
	
		ballNode->transform(rotate(ballNode->transform(),
									radians(rotationDeg),
									vec3(sin(totalSeconds), cos(totalSeconds), -cos(totalSeconds))));
	
	
	
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
	teapotNode->rotation(vec4(0.0f, 1.0f, 0.0f, teapotNode->rotation().w - radians(rotationDeg * 3.0f)));
}

void Test::windowDidUpdateCallback(Scene& scene, float deltaSeconds) {
//	cout << "windowDidUpdateCallback(" << &scene << ", "
//	<< deltaSeconds << ")" << endl;
}
