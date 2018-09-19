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

	AE_INIT();
	
	auto window = make_shared<Window>(FULLSCREEN,
									  WINDOW_WIDTH, WINDOW_HEIGHT,
									  USE_HIGH_DPI,
									  ANTIALIAS_MODE,
									  RENDER_API::OPENGL);
	window->updateCallback(bind(&Test::updateCallback, this, _1, _2));
	window->willRenderCallback(bind(&Test::willRenderCallback, this, _1, _2));
	window->didRenderCallback(bind(&Test::didRenderCallback, this, _1, _2));
	window->enableVSync(ENABLE_VSYNC);
	window->captureCursor(CAPTURE_CURSOR);
	
	auto scene = make_shared<Scene>();
	scene->rootNode(make_shared<Node>());
	
	auto testScene = SceneNamed("importTest");
	m_parentNode = testScene->rootNode();
	scene->rootNode()->addChild(m_parentNode);
	
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

	window->scene(scene);
	window->display();
	
	return 0;
}

/***************************************************************************************
     RenderContext Callbacks
 ***************************************************************************************/

void Test::updateCallback(RenderContext& renderContext, float time) {
	
	static float previousSeconds = time;
	float deltaSeconds = time - previousSeconds;
	previousSeconds = time;
	
	float rotationDeg = deltaSeconds * 30.0; // 30deg/sec

	m_parentNode->transform(rotate(m_parentNode->transform(),
								   radians(rotationDeg),
								   vec3(0.0f, 1.0f, 0.0f)));	
}

void Test::willRenderCallback(RenderContext& renderContext, float time) {
	
}

void Test::didRenderCallback(RenderContext& renderContext, float time) {
	
}
