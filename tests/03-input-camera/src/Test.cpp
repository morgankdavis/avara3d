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
	cout << "Test3::run()\n" << endl;
	
	if (init() != 0) { cout << "Init error!" << endl; return -1; }

	
	auto scene = TestSceneNamed("importTest");
	//	auto scene = TestSceneNamed("dragon", "obj", (SceneLoadingOption)0);
	
	
	auto window = make_shared<Window>(WINDOW_WIDTH, WINDOW_HEIGHT, FRAMEBUFFER_SCALE);
	window->willUpdateCallback(bind(&Test::windowWillUpdateCallback, this, _1, _2));
	window->didUpdateCallback(bind(&Test::windowDidUpdateCallback, this, _1, _2));
	window->scene(scene);
	window->enableCursor(false);
	
	//InputManager inputManager = InputManager(window);
	
	window->display();
	
	return 0;
}

void Test::windowWillUpdateCallback(Scene& scene, float deltaSeconds) {

	static float totalSeconds = 0;
	totalSeconds += deltaSeconds;

	float rotationDeg = deltaSeconds * 30.0; // 30deg/sec

//	if (m_someNode) {
//		m_someNode->transform(rotate(m_someNode->transform(), radians(rotationDeg), vec3(0.0f, 1.0f, 0.0f)));
//	}
}

void Test::windowDidUpdateCallback(Scene& scene, float deltaSeconds) {

}
