//
//  Test.cpp
//	avara-engine
//
//  Created by Morgan Davis on 1/4/18.
//  Copyright © 2018 Morgan K Davis. All rights reserved.
//

#include "Test.h"

#include <iostream>

#include <glm/glm.hpp>

#include "ae.h"
#include "Utilities.h"


using namespace ae;
using namespace ae::utils;
using namespace std;
using namespace std::placeholders;
using namespace glm;


#define USE_HIGH_DPI            true
#define WINDOW_WIDTH			800
#define WINDOW_HEIGHT			600
#define FULLSCREEN 				false


/***************************************************************************************
     MARK:   Public
 **************************************************************************************/

int Test::run(const vector<string>& args) {
	LoggerSink sinks = (LoggerSink)0;
	sinks = (LoggerSink)(sinks | (LoggerSink)LoggerSink_STDOUT);
	//sinks = (LoggerSink)(sinks | (LoggerSink)LoggerSink_MainFile);
	//sinks = (LoggerSink)(sinks | (LoggerSink)LoggerSink_NamedFile);
	auto logger = make_shared<Logger>("test06", sinks);
	
	auto window = Window(FULLSCREEN, WINDOW_WIDTH, WINDOW_HEIGHT, USE_HIGH_DPI);
	logger->info("Test::run()");
	
	window.willUpdateCallback(bind(&Test::windowWillUpdateCallback, this, _1, _2));
	window.didUpdateCallback(bind(&Test::windowDidUpdateCallback, this, _1, _2));
	window.enableCursor(false);
	window.vSyncEnabled(false);
	//window.antialiasingMode(AntialiasingMode_None);
	m_window = &window;
	
	//window.debugOptions(DebugOption_ShowWireframe);

	
	auto scene = make_shared<Scene>();
	

	auto teapotScene = TestSceneNamed("teapot");
	auto teapotNode = teapotScene->rootNode()->allChildNodes()[1];
	teapotNode->geometry()->firstMaterial()->fillMode(FillMode_Lines);
//	teapotNode->geometry()->firstMaterial()->fillMode(FillMode_Points);
	scene->rootNode()->addChildNode(teapotNode);
	
	auto dragonScene = TestSceneNamed("dragon", "obj");
	auto dragonNode = dragonScene->rootNode()->allChildNodes()[0];
	dragonNode->scale({2.5, 2.5, 2.5});
	dragonNode->position({50, 0, 0});
	scene->rootNode()->addChildNode(dragonNode);

	
//	auto background = make_shared<MaterialProperty>(TestCubeNamed("nebula1_blue", "png"));
	auto background = make_shared<MaterialProperty>(TestCubeNamed("sky1", "png"));
	scene->background(background);


	auto ambientLight = make_shared<Light>(LightType_Ambient, make_shared<Color>(0.25, 0.25, 0.25, 1.0));
	auto ambientLightNode = make_shared<Node>(ambientLight);
	scene->rootNode()->addChildNode(ambientLightNode);

	auto pointLight = make_shared<Light>(LightType_Point, make_shared<Color>(Color::White()));
	pointLight->attenuationFactor(0.000015);
	auto pointLightNode = make_shared<Node>(pointLight);
	scene->rootNode()->addChildNode(pointLightNode);

	pointLightNode->position({200.0, 20.0, -20.0});

	auto materialProperty = make_shared<MaterialProperty>(pointLight->color());
	auto material = make_shared<Material>();
	material->name("LIGHT material");
	material->emissive(materialProperty);
	auto geometry = make_shared<Sphere>(3.5, 16);
	geometry->addMaterial(material);
	pointLightNode->geometry(geometry);
	
	
	scene->fogStartDistance(500.0);
	scene->fogEndDistance(5000.0);
	scene->fogDensityExponent(1.0);
	scene->fogColor(make_shared<Color>(Color::LightGray()));

	
	window.scene(scene);
	m_inputManager = window.inputManager();
	window.display();
	
	return 0;
}

void Test::windowWillUpdateCallback(Scene& scene, float deltaSeconds) {

	static float totalSeconds = 0;
	totalSeconds += deltaSeconds;

	// get input

	auto keysDown = m_inputManager->keysDown();
	if (keysDown.count(Key_Escape)) {
		exit(0);
	}
	
	if (keysDown.count(Key_Page_Up)) {
		m_window->debugOptions(DebugOption_ShowWireframe);
	}
	else if (keysDown.count(Key_Page_Down)) {
		m_window->debugOptions((DebugOption)0);
	}
	
//	if (keysDown.count(Key_1)) {
//		scene->debug
//	}
	
	vec2 mousePositionDelta = m_inputManager->mousePositionDelta();

	// move camera

	const static float mouseSensitivity = (1.0f / 0.5f);


	if (!m_cameraNode) {
		for (auto n : scene.rootNode()->immediateChildNodes()) {
			if (n->camera()) {
				m_cameraNode = n;
				break;
			}
		}
	}

	if (m_cameraNode) {

		// look

		vec3 camForward = m_cameraNode->worldForward();
		vec3 camRight = m_cameraNode->worldRight();
		vec3 camUp = m_cameraNode->worldUp();

		float deltaRotX = atan(deltaSeconds * mousePositionDelta.x / mouseSensitivity);
		float deltaRotY = atan(deltaSeconds * mousePositionDelta.y / mouseSensitivity);

		vec3 angles = m_cameraNode->eulerAngles();
		m_cameraNode->eulerAngles(vec3(angles.x + deltaRotY, angles.y - deltaRotX, 0));

		// move

//		const static float MOVE_SPEED = 5.0f; // units/sec
		static float MOVE_SPEED = 0;
		if (!MOVE_SPEED) MOVE_SPEED = Max(scene.extent());

		if(keysDown.count(Key_W)) {
			vec3 positionDelta = deltaSeconds * MOVE_SPEED * camForward;
			m_cameraNode->position(m_cameraNode->position() + positionDelta);
		}
		else if(keysDown.count(Key_S)) {
			vec3 positionDelta = deltaSeconds * MOVE_SPEED * -camForward;
			m_cameraNode->position(m_cameraNode->position() + positionDelta);
		}

		if(keysDown.count(Key_A)) {
			vec3 positionDelta = deltaSeconds * MOVE_SPEED * -camRight;
			m_cameraNode->position(m_cameraNode->position() + positionDelta);
		}
		else if(keysDown.count(Key_D)) {
			vec3 positionDelta = deltaSeconds * MOVE_SPEED * camRight;
			m_cameraNode->position(m_cameraNode->position() + positionDelta);
		}

		if(keysDown.count(Key_Space)) {
			vec3 positionDelta = deltaSeconds * MOVE_SPEED * camUp;
			m_cameraNode->position(m_cameraNode->position() + positionDelta);
		}
	}
}

void Test::windowDidUpdateCallback(Scene& scene, float deltaSeconds) {

}
