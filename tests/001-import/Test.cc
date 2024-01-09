//
//  Example.cc
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

#include "utilities/Utilities.h"


using namespace ae;
using namespace ae::utils;
using namespace glm;
using namespace std;
using namespace std::placeholders;
using namespace test;


constexpr bool					USE_HIGH_DPI =			true;
constexpr unsigned				WINDOW_WIDTH =			1024;
constexpr unsigned				WINDOW_HEIGHT =			768;
constexpr bool					FULLSCREEN =			false;
constexpr ANTIALIASING_MODE		ANTIALIAS_MODE =		ANTIALIASING_MODE::MSAA_4X;
constexpr bool					ENABLE_VSYNC =			false;
constexpr bool					CAPTURE_CURSOR =		false;


/***************************************************************************************
	Public
 ***************************************************************************************/

int Test::run(const vector<string>& args) {
	cout << "Test::run()\n" << endl;

	//AE_INIT();

	auto window = make_shared<Window>(RENDER_API::OPENGL,
									  FULLSCREEN,
									  WINDOW_WIDTH,
									  WINDOW_HEIGHT,
									  USE_HIGH_DPI,
									  ANTIALIAS_MODE);
	window->vSyncEnabled(ENABLE_VSYNC);
	window->cursorCaptured(CAPTURE_CURSOR);

	auto visualWorld = make_shared<VisualWorld>(window);
	auto backgroundColor = make_shared<Color>(109.0f/255.0f, 136.0f/255.0f, 164.0f/255.0f, 1.0f);
	auto background = make_shared<MaterialProperty>(backgroundColor);
	visualWorld->background(background);
	visualWorld->willRender(bind(&Test::willRenderCallback, this, _1, _2));
	visualWorld->didRender(bind(&Test::didRenderCallback, this, _1, _2));

	auto scene = make_shared<Scene>();
	scene->visualWorld(visualWorld);
	scene->update(bind(&Test::updateCallback, this, _1, _2));

	auto testScene = SceneNamed("importTest");
	_importRoot = testScene->rootNode();
	scene->rootNode()->addChild(_importRoot);

	window->open();
	scene->run();
	
	return 0;
}

/***************************************************************************************
	Scene Callbacks
 ***************************************************************************************/

void Test::updateCallback(Scene& scene, float time) {
	
	static float previousSeconds = time;
	float deltaSeconds = time - previousSeconds;
	previousSeconds = time;
	
	float rotationDeg = deltaSeconds * 30.0; // 30deg/sec

	_importRoot->transform(rotate(_importRoot->transform(),
								  radians(rotationDeg),
								  vec3(0.0f, 1.0f, 0.0f)));
}

/***************************************************************************************
	VisualWorld Callbacks
 ***************************************************************************************/

void Test::willRenderCallback(VisualWorld& world, float time) {

}

void Test::didRenderCallback(VisualWorld& world, float time) {

}
