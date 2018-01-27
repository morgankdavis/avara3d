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


#define WINDOW_WIDTH			800
#define WINDOW_HEIGHT			600


/***************************************************************************************
     MARK:   Public
 **************************************************************************************/

int Test::run(const vector<string>& args) {
	cout << "Test::run()\n" << endl;

	auto window = Window(false, WINDOW_WIDTH, WINDOW_HEIGHT, true);
	window.updateCallback(bind(&Test::windowUpdateCallback, this, _1, _2));
	window.willRenderCallback(bind(&Test::windowWillRenderCallback, this, _1, _2));
	window.didRenderCallback(bind(&Test::windowDidRenderCallback, this, _1, _2));
	window.captureCursor(true);
	window.enableVSync(false);

	
	auto scene = make_shared<Scene>();
	

	
	TEST = MatrixTestCase;

	
	

	if (TEST == MatrixTestCase) {
	
	// test using raw matrix manipulation
		
		auto aScene = TestSceneNamed("dragon", "obj");
		auto aNode = aScene->rootNode()->childNodes(false)[0];
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
		auto bNode = bScene->rootNode()->childNodes(false)[0];
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
		auto fNode = fScene->rootNode()->childNodes(false)[0];
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
		
//		Window window = Window(WINDOW_WIDTH, WINDOW_HEIGHT, FRAMEBUFFER_SCALE);
//		window.willUpdateCallback(bind(&Test::windowWillUpdateCallback, this, _1, _2));
//		window.didUpdateCallback(bind(&Test::windowDidUpdateCallback, this, _1, _2));
//		window.scene(scene);
//		window.display();
		
	}
	else if (TEST == Convenience1TestCase) {
		
		// test using constituent parts manipulation
		
		
		auto xNode = make_shared<Node>();
		
		
		auto aScene = TestSceneNamed("dragon", "obj");
		auto aNode = aScene->rootNode()->childNodes(false)[0];
		aNode->name("A");
		aNode->position(vec3(20.0f, 0.0f, 0.0f));
		aNode->scale(vec3(2.0f, 3.0f, 2.0f));
		aNode->rotation(vec4(0.0f, 1.0f, 3.0f, (float)radians(45.0f)));
		xNode->addChildNode(aNode);
		
		
		
		auto bScene = TestSceneNamed("ConvaliaBouquet", "obj");
		auto bNode = bScene->rootNode();
		bNode->name("B");
		bNode->position(vec3(0.0f, -3.0f, 0.0f));
		bNode->rotation(vec4(3.0f, 1.0f, 2.0f, (float)radians(574.0f)));
		bNode->scale(vec3(15.0f, 1.0f, 1.0f));
		aNode->addChildNode(bNode);
		cout << "bNode->rotation(): " << bNode->rotation() << endl;



		auto cScene = TestSceneNamed("cartoon_palm_tree", "obj");
		auto cNode = cScene->rootNode();
		cNode->name("C");
		cNode->rotation(vec4(3.0f, 13.0f, 3.0f, (float)radians(-110.0f)));
		cNode->scale(vec3(0.5f, 0.5f, -2.0f));
		cNode->position(vec3(-2.0f, 1.0f, -2.0f));
		bNode->addChildNode(cNode);


		auto dScene = TestSceneNamed("dragon", "obj");
		auto dNode = dScene->rootNode();
		dNode->name("D");
		dNode->position(vec3(-15.0f, 10.0f, 20.0f));
		dNode->eulerAngles(vec3((float)radians(45.0f), (float)radians(60.0f), (float)radians(30.0f)));
		xNode->addChildNode(dNode);
		
		
		
		//auto scene = make_shared<Scene>();
		scene->rootNode()->addChildNode(xNode);
		
		cout << "aNode worldTransform:\n" << aNode->worldTransform() << endl;
//		cout << "bNode worldTransform:\n" << bNode->worldTransform() << endl;
//		cout << "cNode worldTransform:\n" << cNode->worldTransform() << endl;
//		cout << "dNode worldTransform:\n" << dNode->worldTransform() << endl;
		
		auto camera = make_shared<Camera>(0.01f, 1000.0f, 30.0f);
		auto camNode = make_shared<Node>();
		camNode->camera(camera);
		camNode->name("Camera node");
		camNode->position(vec3(0.0f, 5.0f, 100.0f));
		scene->rootNode()->addChildNode(camNode);
		
//		Window window = Window(WINDOW_WIDTH, WINDOW_HEIGHT, FRAMEBUFFER_SCALE);
//		window.willUpdateCallback(bind(&Test::windowWillUpdateCallback, this, _1, _2));
//		window.didUpdateCallback(bind(&Test::windowDidUpdateCallback, this, _1, _2));
//		window.scene(scene);
//		window.display();
	}
	else if (TEST == EulerTestCase) {
		
		auto rootNode = make_shared<Node>();
		
		auto aScene = TestSceneNamed("teapot");
		auto aNode = aScene->rootNode()->childNode("ID20564224", true); // teapot
		aNode->name("A");
		aNode->position(vec3(-25.0f, 25.0f, 0.0f));
		aNode->eulerAngles(vec3((float)radians(45.0f), 0, 0));
		
		
		auto bScene = TestSceneNamed("teapot");
		auto bNode = bScene->rootNode()->childNode("ID20564224", true); // teapot
		bNode->name("B");
		bNode->position(vec3(25.0f, 25.0f, 0.0f));
		bNode->eulerAngles(vec3(0, (float)radians(45.0f), 0));
		
		
		auto cScene = TestSceneNamed("teapot");
		auto cNode = cScene->rootNode()->childNode("ID20564224", true); // teapot
		cNode->name("C");
		cNode->position(vec3(-25.0f, -25.0f, 0.0f));
		cNode->eulerAngles(vec3(0, 0, (float)radians(45.0f)));
		
		
		auto dScene = TestSceneNamed("teapot");
		auto dNode = dScene->rootNode()->childNode("ID20564224", true); // teapot
		dNode->name("D");
		dNode->position(vec3(25.0f, -25.0f, 0.0f));
		dNode->eulerAngles(vec3((float)radians(30.0f), (float)radians(45.0f), (float)radians(60.0f)));
		
		
		auto eScene = TestSceneNamed("teapot");
		auto eNode = eScene->rootNode()->childNode("ID20564224", true); // teapot
		eNode->name("E");
		eNode->position(vec3(0.0f, 0.0f, 0.0f));
		eNode->eulerAngles(vec3((float)radians(-130.0f), (float)radians(70.0f), (float)radians(20.0f)));
		
		
		
		rootNode->addChildNode(aNode);
		rootNode->addChildNode(bNode);
		rootNode->addChildNode(cNode);
		rootNode->addChildNode(dNode);
		rootNode->addChildNode(eNode);
		
		
		
		//auto scene = make_shared<Scene>();
		scene->rootNode()->addChildNode(rootNode);
		
		cout << "aNode worldTransform:\n" << aNode->worldTransform() << endl;
		cout << "bNode worldTransform:\n" << bNode->worldTransform() << endl;
		cout << "cNode worldTransform:\n" << cNode->worldTransform() << endl;
		cout << "dNode worldTransform:\n" << dNode->worldTransform() << endl;
		cout << "eNode worldTransform:\n" << eNode->worldTransform() << endl;
		
		auto camera = make_shared<Camera>(0.01f, 1000.0f, 30.0f);
		auto camNode = make_shared<Node>();
		camNode->camera(camera);
		camNode->name("Camera node");
		camNode->position(vec3(0.0f, 10.0f, 150.0f));
		scene->rootNode()->addChildNode(camNode);
		
//		Window window = Window(WINDOW_WIDTH, WINDOW_HEIGHT, FRAMEBUFFER_SCALE);
//		window.willUpdateCallback(bind(&Test::windowWillUpdateCallback, this, _1, _2));
//		window.didUpdateCallback(bind(&Test::windowDidUpdateCallback, this, _1, _2));
//		window.scene(scene);
//		window.display();
	}
	else if (TEST == ReverseEulerTestCase) {
		auto rootNode = make_shared<Node>();
		
		auto aScene = TestSceneNamed("teapot");
		auto aNode = aScene->rootNode()->childNode("ID20564224", true); // teapot
		aNode->name("A");
		aNode->position(vec3(-25.0f, 25.0f, 0.0f));
		aNode->orientation(quat((float)radians(45.0f), 1.0f, 0.0f, 0.0f));
		cout << "aNode eulerAngles: " << aNode->eulerAngles() << endl;
		
		
		auto bScene = TestSceneNamed("teapot");
		auto bNode = bScene->rootNode()->childNode("ID20564224", true); // teapot
		bNode->name("B");
		bNode->position(vec3(25.0f, 25.0f, 0.0f));
		bNode->orientation(quat((float)radians(45.0f), 0.0f, 1.0f, 0.0f));
		cout << "bNode eulerAngles: " << bNode->eulerAngles() << endl;
		
		
		
		auto cScene = TestSceneNamed("teapot");
		auto cNode = cScene->rootNode()->childNode("ID20564224", true); // teapot
		cNode->name("C");
		cNode->position(vec3(-25.0f, -25.0f, 0.0f));
		cNode->orientation(quat((float)radians(45.0f), 0.0f, 0.0f, 1.0f));
		cout << "cNode eulerAngles: " << cNode->eulerAngles() << endl;
		
		
		
		auto dScene = TestSceneNamed("teapot");
		auto dNode = dScene->rootNode()->childNode("ID20564224", true); // teapot
		dNode->name("D");
		dNode->position(vec3(25.0f, -25.0f, 0.0f));
		dNode->orientation(quat((float)radians(45.0f), 0.5f, 0.25f, 0.35f));
		cout << "dNode eulerAngles: " << dNode->eulerAngles() << endl;
		
		
		// what does in comes out
		
		auto uNode = make_shared<Node>();
		uNode->eulerAngles(vec3((float)radians(40.0f), (float)radians(-35.0f), (float)radians(75.0f)));
		cout << "uNode->eulerAngles(): " << uNode->eulerAngles() << endl;
		
		auto vNode = make_shared<Node>();
		vNode->eulerAngles(vec3((float)radians(-10.0f), (float)radians(25.0f), (float)radians(30.0f)));
		cout << "vNode->eulerAngles(): " << vNode->eulerAngles() << endl;
		
		
		
		rootNode->addChildNode(aNode);
		rootNode->addChildNode(bNode);
		rootNode->addChildNode(cNode);
		rootNode->addChildNode(dNode);
		//rootNode->addChildNode(eNode);
		
		
		
		//auto scene = make_shared<Scene>();
		scene->rootNode()->addChildNode(rootNode);
		
		cout << "aNode worldTransform:\n" << aNode->worldTransform() << endl;
		cout << "bNode worldTransform:\n" << bNode->worldTransform() << endl;
		cout << "cNode worldTransform:\n" << cNode->worldTransform() << endl;
		cout << "dNode worldTransform:\n" << dNode->worldTransform() << endl;
//		cout << "eNode worldTransform:\n" << eNode->worldTransform() << endl;
		
		auto camera = make_shared<Camera>(0.01f, 1000.0f, 30.0f);
		auto camNode = make_shared<Node>();
		camNode->camera(camera);
		camNode->name("Camera node");
		camNode->position(vec3(0.0f, 10.0f, 150.0f));
		scene->rootNode()->addChildNode(camNode);
		
//		Window window = Window(WINDOW_WIDTH, WINDOW_HEIGHT, FRAMEBUFFER_SCALE);
//		window.willUpdateCallback(bind(&Test::windowWillUpdateCallback, this, _1, _2));
//		window.didUpdateCallback(bind(&Test::windowDidUpdateCallback, this, _1, _2));
//		window.scene(scene);
//		window.display();
	}
	else if (TEST == RotationAnimationTestCase) {
		auto rootNode = make_shared<Node>();
		
		auto aScene = TestSceneNamed("teapot");
		auto aNode = aScene->rootNode()->childNode("teapot", true); // teapot
		aNode->name("A");

		
		
		
		rootNode->addChildNode(aNode);
		
		
		
		//auto scene = make_shared<Scene>();
		scene->rootNode()->addChildNode(rootNode);
		
		cout << "aNode worldTransform:\n" << aNode->worldTransform() << endl;
		
		auto camera = make_shared<Camera>(0.01f, 1000.0f, 30.0f);
		auto camNode = make_shared<Node>();
		camNode->camera(camera);
		camNode->name("Camera node");
		camNode->position(vec3(0.0f, 10.0f, 150.0f));
		scene->rootNode()->addChildNode(camNode);
		
//		Window window = Window(WINDOW_WIDTH, WINDOW_HEIGHT, FRAMEBUFFER_SCALE);
//		window.willUpdateCallback(bind(&Test::windowWillUpdateCallback, this, _1, _2));
//		window.didUpdateCallback(bind(&Test::windowDidUpdateCallback, this, _1, _2));
//		window.scene(scene);
//		window.display();
	}
	
	
	// ******** make everything look like it did before materials worked ********
	
	auto ambientProperty = make_shared<MaterialProperty>(make_shared<Color>(0.75, 0.75, 0.75, 1.0));
	auto diffuseProperty = make_shared<MaterialProperty>(make_shared<Color>(1.0, 1.0, 1.0, 1.0));
	auto material = make_shared<Material>(ambientProperty, diffuseProperty, nullptr);
	
	for (auto n : scene->rootNode()->childNodes(true)) {
		if (n->geometry()) {
			n->geometry()->replaceMaterial(0, material);
		}
	}
	
	auto backgroundColor = make_shared<Color>(109.0f/256.0f, 136.0f/256.0f, 164.0f/256.0f, 1.0f);
	auto background = make_shared<MaterialProperty>(backgroundColor);
	scene->background(background);
	
	// **************************************************************************
	
	
	window.scene(scene);
	window.display();
	
	return 0;
}

/***************************************************************************************
     MARK:   Window Callbacks
 **************************************************************************************/

void Test::windowUpdateCallback(Scene& scene, float time) {
	
	static double previousSeconds = time;
	float deltaSeconds = time - previousSeconds;
	previousSeconds = time;
	
	float rotationDeg = deltaSeconds * 30.0; // 30deg/sec

	if (TEST == RotationAnimationTestCase) {
		auto node = scene.rootNode()->childNode("A", true);
		
		// we WANT this to work (this is how scene kit works)
		// but it locks after 2PI rotation
		// (we think it's becasue rotation() is clipping to 2PI when the underlying quaternion indicates anything larger
		//node->rotation(vec4(1.0f, 0.0f, 0.0f, node->rotation().w + radians(rotationDeg * 2.0f)));
		
		float newAngle = node->rotation().w + radians(rotationDeg * 2.0f);
		newAngle = (newAngle > 0 ?
					fmod(newAngle, 2.0f*M_PI) :
					fmod(newAngle, 2.0f*M_PI));
		node->rotation(vec4(1.0f, 0.0f, 0.0f, newAngle));
		
		cout << "node transform:\n" << node->transform() << endl;
	}
}

void Test::windowWillRenderCallback(Scene& scene, float time) {

}

void Test::windowDidRenderCallback(Scene& scene, float time) {

}
