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


constexpr unsigned				WINDOW_WIDTH =			800;
constexpr unsigned				WINDOW_HEIGHT =			600;


/***************************************************************************************
     Public
 ***************************************************************************************/

int Test::run(const vector<string>& args) {
	cout << "Test::run()\n" << endl;

	auto window = make_shared<Window>(false, WINDOW_WIDTH, WINDOW_HEIGHT, true);
	window->updateCallback(bind(&Test::windowUpdateCallback, this, _1, _2));
	window->willRenderCallback(bind(&Test::windowWillRenderCallback, this, _1, _2));
	window->didRenderCallback(bind(&Test::windowDidRenderCallback, this, _1, _2));
	window->captureCursor(true);
	window->enableVSync(false);
	
	
	auto scene = TestSceneNamed("importTest");
	auto rootImmediateChildren = scene->rootNode()->children(false);
	
	for (auto c : rootImmediateChildren) {
		c->removeFromParentNode();
	}
	
	parentNode = make_shared<Node>();
	parentNode->addChilds(rootImmediateChildren);
	scene->rootNode()->addChild(parentNode);
	
//	mat4 parentScale = scale(mat4(1.0f), vec3(1.0f, 1.0f, 1.0f) * 25.0f);
//	parentNode->transform(parentScale);
	

//	cout << "Loading siamese scene..." << endl;
//	auto siameseScene = TestSceneNamed("siamese");
//	siameseNode = siameseScene->rootNode()->children(false)[3];
//	siameseNode->name("Siamese");
//	cout << "siameseNode: " << *siameseNode->name() << endl;
//	//siameseNode->transform(scale(siameseNode->transform(), vec3(1.0f) * 0.001f));
//	siameseNode->scale(siameseNode->scale() * 0.1f);
//	siameseNode->hidden(false);
//	parentNode->addChild(siameseNode);

	
	torusNode = scene->rootNode()->children(true)[6];
	torusNode->name("torus");
	
	
	coneNode = scene->rootNode()->children(true)[8];
	coneNode->name("cone");
	
	
	ballNode = scene->rootNode()->children(true)[4];
	ballNode->name("ball");

	
	auto palmScene = TestSceneNamed("cartoon_palm_tree", "obj");
	palmNode = palmScene->rootNode();
	palmNode->name("palm");
	cout << "palmNode: " << *palmNode->name() << endl;
	//palmNode->transform(scale(mat4(1.0f), vec3(1.0f) * 0.025f));
	palmNode->scale(vec3(1.0f) * 2.0f);
	palmNode->hidden(false);
	parentNode->addChild(palmNode);
	
	
//	auto teapotScene = TestSceneNamed("teapot");
//	teapotNode = teapotScene->rootNode();
//	//teapotNode = teapotScene->rootNode()->children(false)[1];
//	teapotNode->name("teapot");
//	cout << "teapotNode: " << *teapotNode->name() << endl;
//	auto teapotTranslate = translate(mat4(1.0f), vec3(0.5f, -0.5f, 0.0f));
//	auto teapotScale = scale(mat4(1.0f), vec3(1.0f) * 0.005f);
//	teapotNode->transform(teapotTranslate * teapotScale);
//	teapotNode->hidden(false);
//	parentNode->addChild(teapotNode);
	
	
	
	// ******** make everything look like it did before materials worked ********
	
	auto ambientProperty = make_shared<MaterialProperty>(make_shared<Color>(0.75, 0.75, 0.75, 1.0));
	auto diffuseProperty = make_shared<MaterialProperty>(make_shared<Color>(1.0, 1.0, 1.0, 1.0));
	auto material = make_shared<Material>(ambientProperty, diffuseProperty, nullptr);
	
	for (auto n : scene->rootNode()->children(true)) {
		if (n->geometry()) {
			n->geometry()->replaceMaterial(0, material);
		}
	}
	
	auto backgroundColor = make_shared<Color>(109.0f/256.0f, 136.0f/256.0f, 164.0f/256.0f, 1.0f);
	auto background = make_shared<MaterialProperty>(backgroundColor);
	scene->background(background);
	
	// **************************************************************************
	
	
	//StartGIFRecording(window, 240, 8);

	window->scene(scene);
	window->display();
	
	//StopGIFRecording(window);
	
	return 0;
}

/***************************************************************************************
     RenderContext Callbacks
 ***************************************************************************************/

void Test::windowUpdateCallback(Scene& scene, float time) {
	
	static double previousSeconds = time;
	float deltaSeconds = time - previousSeconds;
	previousSeconds = time;
	
	float rotationDeg = deltaSeconds * 30.0; // 30deg/sec
	
	parentNode->transform(rotate(parentNode->transform(),
								 radians(rotationDeg),
								 vec3(0.0f, 1.0f, 0.0f)));
	
	//siameseNode->rotation(vec4(0.0f, 1.0f, 1.0f, siameseNode->rotation().w + radians(rotationDeg * 2.0f)));
	
//	torusNode->transform(rotate(torusNode->transform(),
//								radians(rotationDeg),
//								vec3(1.0f, 0.0f, 1.0f)));
//
//	coneNode->transform(rotate(coneNode->transform(),
//							   radians(rotationDeg),
//							   vec3(0.0f, 0.0f, 1.0f)));
//
//	ballNode->transform(rotate(ballNode->transform(),
//							   radians(rotationDeg),
//							   vec3(sin(time), cos(time), -cos(time))));

	//teapotNode->rotation(vec4(0.0f, 1.0f, 0.0f, teapotNode->rotation().w - radians(rotationDeg * 3.0f)));
}

void Test::windowWillRenderCallback(Scene& scene, float time) {

}

void Test::windowDidRenderCallback(Scene& scene, float time) {

}
