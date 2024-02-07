//
//  main.cpp
//	avara-engine
//
//  Created by Morgan Davis on 10/15/17.
//  Copyright © 2017 Morgan K Davis. All rights reserved.
//

// TODO: destroy the scene after each test and run them sequentially

#include <iostream>
#define _USE_MATH_DEFINES // needed for Windows
#include <math.h>
#include <memory>

#include "ae/ae.h"
#include "ae/Utilities.h"


using namespace ae;
using namespace ae::utils;
using namespace glm;
using namespace std;
using namespace std::placeholders;


enum class TEST {
	TRAVERSAL/*,
	MATRIX,
	CONVENIENCE,
	EULER,
	REVERSE_EULER,
	ROTATION*/
};

void UpdateCallback(Scene& scene, float time);
void WillRenderCallback(VisualWorld& world, float time);
void DidRenderCallback(VisualWorld& world, float time);


constexpr TEST					USING_TEST =			TEST::TRAVERSAL;
constexpr bool					USE_HIGH_DPI =			false;
constexpr unsigned				WINDOW_WIDTH =			1024;
constexpr unsigned				WINDOW_HEIGHT =			768;
constexpr bool					FULLSCREEN =			false;
constexpr AntialiasingMode		ANTIALIAS_MODE =		AntialiasingMode::Msaa4X;
constexpr bool					ENABLE_VSYNC =			false;
constexpr bool					CAPTURE_CURSOR =		false;


int main(int argc, const char* argv[]) {

	cout << "test002::main()\n" << endl;

	auto window = make_shared<Window>(RenderApi::OpenGL,
									  FULLSCREEN,
									  WINDOW_WIDTH,
									  WINDOW_HEIGHT,
									  USE_HIGH_DPI,
									  ANTIALIAS_MODE);
	window->vSyncEnabled(ENABLE_VSYNC);
	window->cursorCaptured(CAPTURE_CURSOR);

	auto visualWorld = make_shared<VisualWorld>(window);
	auto backgroundColor = make_shared<Color>(109.0f/255.0f, 136.0f/255.0f, 164.0f/255.0f, 1.0f);
	visualWorld->background(make_shared<MaterialProperty>(backgroundColor));
	visualWorld->willRender(bind(&WillRenderCallback, _1, _2));
	visualWorld->didRender(bind(&DidRenderCallback, _1, _2));

	auto scene = make_shared<Scene>();
	scene->visualWorld(visualWorld);
	scene->update(bind(&UpdateCallback, _1, _2));


	if (USING_TEST == TEST::TRAVERSAL) {

		auto a = Node::NamedNode("a");
		auto b = Node::NamedNode("b");
		auto c = Node::NamedNode("c");
		auto d = Node::NamedNode("d");
		auto e = Node::NamedNode("e");
		auto f = Node::NamedNode("f");
		auto g = Node::NamedNode("g");
		auto h = Node::NamedNode("h");
		auto i = Node::NamedNode("i");
		auto j = Node::NamedNode("j");
		auto k = Node::NamedNode("k");
		auto l = Node::NamedNode("l");
		auto m = Node::NamedNode("m");
		auto n = Node::NamedNode("n");
		auto o = Node::NamedNode("o");
		auto p = Node::NamedNode("p");
		auto q = Node::NamedNode("q");
		auto r = Node::NamedNode("r");
		auto s = Node::NamedNode("s");
		auto t = Node::NamedNode("t");
		auto u = Node::NamedNode("u");
		auto v = Node::NamedNode("v");
		auto w = Node::NamedNode("w");

		u->addChild(v);
		u->addChild(w);

		t->addChild(u);

		n->addChild(t);

		g->addChild(n);

		c->addChild(f);
		c->addChild(g);

		a->addChild(c);

		k->addChild(q);
		k->addChild(r);

		m->addChild(s);

		e->addChild(j);
		e->addChild(k);
		e->addChild(l);
		e->addChild(m);

		b->addChild(e);

		a->addChild(b);

		h->addChild(o);
		h->addChild(p);

		d->addChild(h);
		d->addChild(i);

		a->addChild(d);

		a->_debugPrint();

		for (auto& c : a->children(true)) {
			AE_LOG_I("c: {}", *c->name());
		}

//		2023-11-18 21:18:20.479 [ae] [info] [Node.cc:836] [_debugPrintRec()] [0] c
//		2023-11-18 21:18:20.479 [ae] [info] [Node.cc:836] [_debugPrintRec()] [1] f
//		2023-11-18 21:18:20.479 [ae] [info] [Node.cc:836] [_debugPrintRec()] [1] g
//		2023-11-18 21:18:20.479 [ae] [info] [Node.cc:836] [_debugPrintRec()] [2] n
//		2023-11-18 21:18:20.479 [ae] [info] [Node.cc:836] [_debugPrintRec()] [3] t
//		2023-11-18 21:18:20.479 [ae] [info] [Node.cc:836] [_debugPrintRec()] [4] u
//		2023-11-18 21:18:20.479 [ae] [info] [Node.cc:836] [_debugPrintRec()] [5] v
//		2023-11-18 21:18:20.479 [ae] [info] [Node.cc:836] [_debugPrintRec()] [5] w
//		2023-11-18 21:18:20.479 [ae] [info] [Node.cc:836] [_debugPrintRec()] [0] b
//		2023-11-18 21:18:20.479 [ae] [info] [Node.cc:836] [_debugPrintRec()] [1] e
//		2023-11-18 21:18:20.479 [ae] [info] [Node.cc:836] [_debugPrintRec()] [2] j
//		2023-11-18 21:18:20.479 [ae] [info] [Node.cc:836] [_debugPrintRec()] [2] k
//		2023-11-18 21:18:20.479 [ae] [info] [Node.cc:836] [_debugPrintRec()] [3] q
//		2023-11-18 21:18:20.479 [ae] [info] [Node.cc:836] [_debugPrintRec()] [3] r
//		2023-11-18 21:18:20.479 [ae] [info] [Node.cc:836] [_debugPrintRec()] [2] l
//		2023-11-18 21:18:20.479 [ae] [info] [Node.cc:836] [_debugPrintRec()] [2] m
//		2023-11-18 21:18:20.479 [ae] [info] [Node.cc:836] [_debugPrintRec()] [3] s
//		2023-11-18 21:18:20.479 [ae] [info] [Node.cc:836] [_debugPrintRec()] [0] d
//		2023-11-18 21:18:20.479 [ae] [info] [Node.cc:836] [_debugPrintRec()] [1] h
//		2023-11-18 21:18:20.479 [ae] [info] [Node.cc:836] [_debugPrintRec()] [2] o
//		2023-11-18 21:18:20.479 [ae] [info] [Node.cc:836] [_debugPrintRec()] [2] p
//		2023-11-18 21:18:20.479 [ae] [info] [Node.cc:836] [_debugPrintRec()] [1] i

	}
//	else if (USING_TEST == TEST::MATRIX) {
//
//		// test using raw matrix manipulation
//
//		auto aScene = SceneNamed("dragon", "obj");
//		auto aNode = aScene->rootNode()->children(false)[0];
//		aNode->name("A");
//		auto aTranslate = 	translate(mat4(1.0f), vec3(10.0f, 0.0f, 0.0f));
//		auto aScale = 		scale(mat4(1.0f), vec3(2.0f, 2.0f, 2.0f));
//		auto aRotate = 		rotate(mat4(1.0f), (float)radians(45.0), vec3(0.0f, 0.0f, 1.0f));
//		auto aTIn = 		aRotate * aScale * aTranslate;
//		cout << "aTIn:\n" << aTIn << endl;
//		aNode->transform(aTIn);
//		cout << "aNode transform:\n" << aNode->transform() << endl;
//		cout << "aNode orientation: " << aNode->orientation() << endl;
//		cout << "aNode rotation: " << aNode->rotation() << endl;
//		cout << "aNode eulerAngles: " << aNode->eulerAngles() << endl;
//
//
//		auto bScene = SceneNamed("dragon", "obj");
//		auto bNode = bScene->rootNode()->children(false)[0];
//		bNode->name("B");
//		auto bTranslate = 	translate(mat4(1.0f), vec3(0.0f, -20.0f, -7.0f));
//		auto bScale = 		scale(mat4(1.0f), vec3(1.0f, 1.0f, 1.5f));
//		auto bRotate = 		rotate(mat4(1.0f), (float)radians(30.0), vec3(1.0f, 1.0f, 0.0f));
//		auto bTIn = 		bRotate * bScale * bTranslate;
//		cout << "bTIn:\n" << bTIn << endl;
//		bNode->transform(bTIn);
//		cout << "bNode transform:\n" << bNode->transform() << endl;
//		cout << "bNode orientation: " << bNode->orientation() << endl;
//		cout << "bNode rotation: " << bNode->rotation() << endl;
//		cout << "bNode eulerAngles: " << bNode->eulerAngles() << endl;
//		aNode->addChild(bNode);
//
//
//		auto fScene = SceneNamed("dragon", "obj");
//		auto fNode = fScene->rootNode()->children(false)[0];
//		fNode->name("F");
//		auto fTranslate = 	translate(mat4(1.0f), vec3(-16.0f, 0.0f, -60.0f));
//		auto fScale = 		scale(mat4(1.0f), vec3(2.0f, 5.0f, 3.0f));
//		auto fRotate = 		rotate(mat4(1.0f), (float)radians(-90.0), vec3(0.0f, 1.0f, 3.0f));
//		auto fTIn =			fRotate * fScale * fTranslate;
//		cout << "fTIn:\n" << fTIn << endl;
//		fNode->transform(fTIn);
//		cout << "fNode transform:\n" << fNode->transform() << endl;
//		cout << "fNode orientation: " << fNode->orientation() << endl;
//		cout << "fNode rotation: " << fNode->rotation() << endl;
//		cout << "fNode eulerAngles: " << fNode->eulerAngles() << endl;
//		bNode->addChild(fNode);
//
//
//		auto pScene = SceneNamed("cartoon_palm_tree", "obj");
//		auto pNode = pScene->rootNode();
//		pNode->name("P");
//		auto pTranslate = 	translate(mat4(1.0f), vec3(0.0f, -7.0f, 0.0f));
//		auto pScale = 		scale(mat4(1.0f), vec3(1.0f, 1.0f, 10.0f));
//		auto pRotate = 		rotate(mat4(1.0f), (float)radians(50.0), vec3(1.0f, 1.0f, 0.0f));
//		auto pTIn =			pTranslate * pRotate * pScale;
//		cout << "pTIn:\n" << pTIn << endl;
//		pNode->transform(pTIn);
//		cout << "pNode transform:\n" << pNode->transform() << endl;
//		cout << "pNode orientation: " << pNode->orientation() << endl;
//		cout << "pNode rotation: " << pNode->rotation() << endl;
//		cout << "pNode eulerAngles: " << pNode->eulerAngles() << endl;
//		fNode->addChild(pNode);
//
//
//		auto gScene = SceneNamed("ConvaliaBouquet", "obj");
//		auto gNode = gScene->rootNode();
//		gNode->name("G");
//		auto gTranslate = 	translate(mat4(1.0f), vec3(0.0f, -7.0f, 0.0f));
//		auto gScale = 		scale(mat4(1.0f), vec3(1.0f, 1.0f, 1.0f));
//		auto gRotate = 		rotate(mat4(1.0f), (float)radians(-50.0), vec3(1.0f, 1.0f, 0.0f));
//		auto gTIn =			 gScale * gTranslate * gRotate;
//		cout << "gTIn:\n" << gTIn << endl;
//		gNode->transform(gTIn);
//		cout << "gNode transform:\n" << gNode->transform() << endl;
//		cout << "gNode orientation: " << gNode->orientation() << endl;
//		cout << "gNode rotation: " << gNode->rotation() << endl;
//		cout << "gNode eulerAngles: " << gNode->eulerAngles() << endl;
//		aNode->addChild(gNode);
//
//
//		scene->rootNode()->addChild(aNode);
//
//		cout << "aNode worldTransform:\n" << aNode->worldTransform() << endl;
//		cout << "bNode worldTransform:\n" << bNode->worldTransform() << endl;
//		cout << "fNode worldTransform:\n" << fNode->worldTransform() << endl;
//		cout << "pNode worldTransform:\n" << pNode->worldTransform() << endl;
//
//		auto camera = make_shared<PerspectiveCamera>(0.01f, 1000.0f, radians(30.0f));
//		auto camNode = make_shared<Node>();
//		camNode->camera(camera);
//		camNode->name("Camera node");
//		camNode->position(vec3(0.0f, 5.0f, 100.0f));
//		scene->rootNode()->addChild(camNode);
//
////		Window window = Window(WINDOW_WIDTH, WINDOW_HEIGHT, FRAMEBUFFER_SCALE);
////		window.willUpdateCallback(bind(&Test::windowWillUpdateCallback, this, _1, _2));
////		window.didUpdateCallback(bind(&Test::windowDidUpdateCallback, this, _1, _2));
////		window.scene(scene);
////		window.display();
//
//	}
//	else if (USING_TEST == TEST::CONVENIENCE) {
//
//		// test using constituent parts manipulation
//
//
//		auto xNode = make_shared<Node>();
//
//
//		auto aScene = SceneNamed("dragon", "obj");
//		auto aNode = aScene->rootNode()->children(false)[0];
//		aNode->name("A");
//		aNode->position(vec3(20.0f, 0.0f, 0.0f));
//		aNode->scale(vec3(2.0f, 3.0f, 2.0f));
//		aNode->rotation({0.0f, 1.0f, 3.0f}, (float)radians(45.0f));
//		xNode->addChild(aNode);
//
//
//		auto bScene = SceneNamed("ConvaliaBouquet", "obj");
//		auto bNode = bScene->rootNode();
//		bNode->name("B");
//		bNode->position(vec3(0.0f, -3.0f, 0.0f));
//		bNode->rotation({3.0f, 1.0f, 2.0f}, (float)radians(574.0f));
//		bNode->scale(vec3(15.0f, 1.0f, 1.0f));
//		aNode->addChild(bNode);
//		cout << "bNode->rotation(): " << bNode->rotation() << endl;
//
//
//		auto cScene = SceneNamed("cartoon_palm_tree", "obj");
//		auto cNode = cScene->rootNode();
//		cNode->name("C");
//		cNode->rotation({3.0f, 13.0f, 3.0f}, (float)radians(-110.0f));
//		cNode->scale(vec3(0.5f, 0.5f, -2.0f));
//		cNode->position(vec3(-2.0f, 1.0f, -2.0f));
//		bNode->addChild(cNode);
//
//
//		auto dScene = SceneNamed("dragon", "obj");
//		auto dNode = dScene->rootNode();
//		dNode->name("D");
//		dNode->position(vec3(-15.0f, 10.0f, 20.0f));
//		dNode->eulerAngles(vec3((float)radians(45.0f), (float)radians(60.0f), (float)radians(30.0f)));
//		xNode->addChild(dNode);
//
//
//		//auto scene = make_shared<Scene>();
//		scene->rootNode()->addChild(xNode);
//
//		cout << "aNode worldTransform:\n" << aNode->worldTransform() << endl;
////		cout << "bNode worldTransform:\n" << bNode->worldTransform() << endl;
////		cout << "cNode worldTransform:\n" << cNode->worldTransform() << endl;
////		cout << "dNode worldTransform:\n" << dNode->worldTransform() << endl;
//
//		auto camera = make_shared<PerspectiveCamera>(0.01f, 1000.0f, 30.0f);
//		auto camNode = make_shared<Node>();
//		camNode->camera(camera);
//		camNode->name("Camera node");
//		camNode->position(vec3(0.0f, 5.0f, 100.0f));
//		scene->rootNode()->addChild(camNode);
//
////		Window window = Window(WINDOW_WIDTH, WINDOW_HEIGHT, FRAMEBUFFER_SCALE);
////		window.willUpdateCallback(bind(&Test::windowWillUpdateCallback, this, _1, _2));
////		window.didUpdateCallback(bind(&Test::windowDidUpdateCallback, this, _1, _2));
////		window.scene(scene);
////		window.display();
//	}
//	else if (USING_TEST == TEST::EULER) {
//
//		auto rootNode = make_shared<Node>();
//
//		auto aScene = SceneNamed("teapot");
//		auto aNode = aScene->rootNode()->childNamed("ID20564224", true); // teapot
//		aNode->name("A");
//		aNode->position(vec3(-25.0f, 25.0f, 0.0f));
//		aNode->eulerAngles(vec3((float)radians(45.0f), 0, 0));
//
//
//		auto bScene = SceneNamed("teapot");
//		auto bNode = bScene->rootNode()->childNamed("ID20564224", true); // teapot
//		bNode->name("B");
//		bNode->position(vec3(25.0f, 25.0f, 0.0f));
//		bNode->eulerAngles(vec3(0, (float)radians(45.0f), 0));
//
//
//		auto cScene = SceneNamed("teapot");
//		auto cNode = cScene->rootNode()->childNamed("ID20564224", true); // teapot
//		cNode->name("C");
//		cNode->position(vec3(-25.0f, -25.0f, 0.0f));
//		cNode->eulerAngles(vec3(0, 0, (float)radians(45.0f)));
//
//
//		auto dScene = SceneNamed("teapot");
//		auto dNode = dScene->rootNode()->childNamed("ID20564224", true); // teapot
//		dNode->name("D");
//		dNode->position(vec3(25.0f, -25.0f, 0.0f));
//		dNode->eulerAngles(vec3((float)radians(30.0f), (float)radians(45.0f), (float)radians(60.0f)));
//
//
//		auto eScene = SceneNamed("teapot");
//		auto eNode = eScene->rootNode()->childNamed("ID20564224", true); // teapot
//		eNode->name("E");
//		eNode->position(vec3(0.0f, 0.0f, 0.0f));
//		eNode->eulerAngles(vec3((float)radians(-130.0f), (float)radians(70.0f), (float)radians(20.0f)));
//
//
//		rootNode->addChild(aNode);
//		rootNode->addChild(bNode);
//		rootNode->addChild(cNode);
//		rootNode->addChild(dNode);
//		rootNode->addChild(eNode);
//
//
//		//auto scene = make_shared<Scene>();
//		scene->rootNode()->addChild(rootNode);
//
//		cout << "aNode worldTransform:\n" << aNode->worldTransform() << endl;
//		cout << "bNode worldTransform:\n" << bNode->worldTransform() << endl;
//		cout << "cNode worldTransform:\n" << cNode->worldTransform() << endl;
//		cout << "dNode worldTransform:\n" << dNode->worldTransform() << endl;
//		cout << "eNode worldTransform:\n" << eNode->worldTransform() << endl;
//
//		auto camera = make_shared<PerspectiveCamera>(0.01f, 1000.0f, 30.0f);
//		auto camNode = make_shared<Node>();
//		camNode->camera(camera);
//		camNode->name("Camera node");
//		camNode->position(vec3(0.0f, 10.0f, 150.0f));
//		scene->rootNode()->addChild(camNode);
//
////		Window window = Window(WINDOW_WIDTH, WINDOW_HEIGHT, FRAMEBUFFER_SCALE);
////		window.willUpdateCallback(bind(&Test::windowWillUpdateCallback, this, _1, _2));
////		window.didUpdateCallback(bind(&Test::windowDidUpdateCallback, this, _1, _2));
////		window.scene(scene);
////		window.display();
//	}
//	else if (USING_TEST == TEST::REVERSE_EULER) {
//		auto rootNode = make_shared<Node>();
//
//		auto aScene = SceneNamed("teapot");
//		auto aNode = aScene->rootNode()->childNamed("ID20564224", true); // teapot
//		aNode->name("A");
//		aNode->position(vec3(-25.0f, 25.0f, 0.0f));
//		aNode->orientation(quat((float)radians(45.0f), 1.0f, 0.0f, 0.0f));
//		cout << "aNode eulerAngles: " << aNode->eulerAngles() << endl;
//
//
//		auto bScene = SceneNamed("teapot");
//		auto bNode = bScene->rootNode()->childNamed("ID20564224", true); // teapot
//		bNode->name("B");
//		bNode->position(vec3(25.0f, 25.0f, 0.0f));
//		bNode->orientation(quat((float)radians(45.0f), 0.0f, 1.0f, 0.0f));
//		cout << "bNode eulerAngles: " << bNode->eulerAngles() << endl;
//
//
//		auto cScene = SceneNamed("teapot");
//		auto cNode = cScene->rootNode()->childNamed("ID20564224", true); // teapot
//		cNode->name("C");
//		cNode->position(vec3(-25.0f, -25.0f, 0.0f));
//		cNode->orientation(quat((float)radians(45.0f), 0.0f, 0.0f, 1.0f));
//		cout << "cNode eulerAngles: " << cNode->eulerAngles() << endl;
//
//
//		auto dScene = SceneNamed("teapot");
//		auto dNode = dScene->rootNode()->childNamed("ID20564224", true); // teapot
//		dNode->name("D");
//		dNode->position(vec3(25.0f, -25.0f, 0.0f));
//		dNode->orientation(quat((float)radians(45.0f), 0.5f, 0.25f, 0.35f));
//		cout << "dNode eulerAngles: " << dNode->eulerAngles() << endl;
//
//
//		// what goes in comes out
//
//		auto uNode = make_shared<Node>();
//		uNode->eulerAngles(vec3((float)radians(40.0f), (float)radians(-35.0f), (float)radians(75.0f)));
//		cout << "uNode->eulerAngles(): " << uNode->eulerAngles() << endl;
//
//		auto vNode = make_shared<Node>();
//		vNode->eulerAngles(vec3((float)radians(-10.0f), (float)radians(25.0f), (float)radians(30.0f)));
//		cout << "vNode->eulerAngles(): " << vNode->eulerAngles() << endl;
//
//
//		rootNode->addChild(aNode);
//		rootNode->addChild(bNode);
//		rootNode->addChild(cNode);
//		rootNode->addChild(dNode);
//		//rootNode->addChild(eNode);
//
//
//		//auto scene = make_shared<Scene>();
//		scene->rootNode()->addChild(rootNode);
//
//		cout << "aNode worldTransform:\n" << aNode->worldTransform() << endl;
//		cout << "bNode worldTransform:\n" << bNode->worldTransform() << endl;
//		cout << "cNode worldTransform:\n" << cNode->worldTransform() << endl;
//		cout << "dNode worldTransform:\n" << dNode->worldTransform() << endl;
////		cout << "eNode worldTransform:\n" << eNode->worldTransform() << endl;
//
//		auto camera = make_shared<PerspectiveCamera>(0.01f, 1000.0f, 30.0f);
//		auto camNode = make_shared<Node>();
//		camNode->camera(camera);
//		camNode->name("Camera node");
//		camNode->position(vec3(0.0f, 10.0f, 150.0f));
//		scene->rootNode()->addChild(camNode);
//
////		Window window = Window(WINDOW_WIDTH, WINDOW_HEIGHT, FRAMEBUFFER_SCALE);
////		window.willUpdateCallback(bind(&Test::windowWillUpdateCallback, this, _1, _2));
////		window.didUpdateCallback(bind(&Test::windowDidUpdateCallback, this, _1, _2));
////		window.scene(scene);
////		window.display();
//	}
//	else if (USING_TEST == TEST::ROTATION) {
//		auto rootNode = make_shared<Node>();
//
//		auto aScene = SceneNamed("teapot");
//		auto aNode = aScene->rootNode()->childNamed("teapot", true); // teapot
//		aNode->name("A");
//
//
//		rootNode->addChild(aNode);
//
//
//		//auto scene = make_shared<Scene>();
//		scene->rootNode()->addChild(rootNode);
//
//		cout << "aNode worldTransform:\n" << aNode->worldTransform() << endl;
//
//		auto camera = make_shared<PerspectiveCamera>(0.01f, 1000.0f, 30.0f);
//		auto camNode = make_shared<Node>();
//		camNode->camera(camera);
//		camNode->name("Camera node");
//		camNode->position(vec3(0.0f, 10.0f, 150.0f));
//		scene->rootNode()->addChild(camNode);
//
////		Window window = Window(WINDOW_WIDTH, WINDOW_HEIGHT, FRAMEBUFFER_SCALE);
////		window.willUpdateCallback(bind(&Test::windowWillUpdateCallback, this, _1, _2));
////		window.didUpdateCallback(bind(&Test::windowDidUpdateCallback, this, _1, _2));
////		window.scene(scene);
////		window.display();
//	}

	window->open();
	scene->run();

	return 0;
}

/***************************************************************************************
	Scene Callbacks
 ***************************************************************************************/

void UpdateCallback(Scene& scene, float time) {

	static float previousSeconds = time;
	float deltaSeconds = time - previousSeconds;
	previousSeconds = time;

	float rotationDeg = deltaSeconds * 30.0; // 30deg/sec

//	if (USING_TEST == TEST::ROTATION) {
//		auto node = scene.rootNode()->childNamed("A", true);
//
//		// we WANT this to work (this is how scene kit works)
//		// but it locks after 2PI rotation
//		// (we think it's becasue rotation() is clipping to 2PI when the underlying quaternion indicates anything larger
//		//node->rotation(vec4(1.0f, 0.0f, 0.0f, node->rotation().w + radians(rotationDeg * 2.0f)));
//
//		float newAngle = node->rotation().w + radians(rotationDeg * 2.0f);
//		newAngle = (newAngle > 0 ?
//					fmod(newAngle, 2.0f*M_PI) :
//					fmod(newAngle, 2.0f*M_PI));
//		node->rotation({1.0f, 0.0f, 0.0f}, newAngle);
//
//		cout << "node transform:\n" << node->transform() << endl;
//	}
}

/***************************************************************************************
	VisualWorld Callbacks
 ***************************************************************************************/

void WillRenderCallback(VisualWorld& world, float time) {

}

void DidRenderCallback(VisualWorld& world, float time) {

}
