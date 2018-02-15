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


#define ENABLE_HIGH_DPI        	true
#define WINDOW_WIDTH			800
#define WINDOW_HEIGHT			600
#define FULLSCREEN 				false
#define ANTIALIASING_MODE		AntialiasingMode_None
#define ENABLE_VSYNC			false
#define CAPTURE_CURSOR			true
#define MOUSE_SENSITIVITY		0.5f


/***************************************************************************************
     MARK:   Public
 **************************************************************************************/

int Test::run(const vector<string>& args) {
	LoggerSink sinks = (LoggerSink)0;
	sinks = (LoggerSink)(sinks | (LoggerSink)LoggerSink_STDOUT);
	//sinks = (LoggerSink)(sinks | (LoggerSink)LoggerSink_MainFile);
	//sinks = (LoggerSink)(sinks | (LoggerSink)LoggerSink_NamedFile);
	auto logger = make_shared<Logger>("test06", sinks);
	
	auto window = Window(FULLSCREEN, WINDOW_WIDTH, WINDOW_HEIGHT, ENABLE_HIGH_DPI, ANTIALIASING_MODE);
	logger->info("Test::run()");
	
	window.updateCallback(bind(&Test::windowUpdateCallback, this, _1, _2));
	window.willRenderCallback(bind(&Test::windowWillRenderCallback, this, _1, _2));
	window.didRenderCallback(bind(&Test::windowDidRenderCallback, this, _1, _2));
	window.captureCursor(CAPTURE_CURSOR);
	window.enableVSync(ENABLE_VSYNC);
	//window.antialiasingMode(AntialiasingMode_None);
	m_window = &window;
	
	//m_debugOptions = DebugOption_ShowStatsOveray;
	window.debugOptions(DebugOption_ShowStatsOveray);
	
	

//	auto scene = TestSceneNamed("sponza/sponza", "obj");
//	AE_LOG->info("sponza extent: {}", StringFromGLMVec3(scene->extent()));
	
	auto scene = make_shared<Scene>();
	

	auto teapotScene = TestSceneNamed("teapot");
	auto teapotNode = teapotScene->rootNode()->childNodes(true)[1];
    teapotNode->rotation({1, 0, 0, radians(30.0)});
//	teapotNode->geometry()->firstMaterial()->fillMode(FillMode_Lines);
//	teapotNode->geometry()->firstMaterial()->fillMode(FillMode_Points);
	scene->rootNode()->addChildNode(teapotNode);

	auto dragonScene = TestSceneNamed("dragon", "obj");
	auto dragonNode = dragonScene->rootNode()->childNodes(true)[0];
	dragonNode->scale({2.5, 2.5, 2.5});
	dragonNode->position({50, 0, 0});
	scene->rootNode()->addChildNode(dragonNode);
	
	
	// test exception
	try {
		scene->rootNode()->addChildNode(teapotNode);
	}
	catch (Exception& e) {
		logger->error(e.what());
	}
	

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

/***************************************************************************************
     MARK:   Window Callbacks
 **************************************************************************************/

void Test::windowUpdateCallback(Scene& scene, float time) {
	
	static double previousSeconds = time;
	float deltaSeconds = time - previousSeconds;
	previousSeconds = time;
	
	// get input
	
	auto keysPressed = m_inputManager->keysPressed();
	
	if (keysPressed.count(Key_Escape)) {
		exit(0);
	}
	
	DebugOption options = (DebugOption)m_window->debugOptions();
	if (keysPressed.count(Key_F)) {
		if (m_window->debugOptions() & DebugOption_ShowWireframes) {
			m_window->debugOptions((DebugOption)(options & ~DebugOption_ShowWireframes));
		}
		else {
			m_window->debugOptions((DebugOption)(options | DebugOption_ShowWireframes));
		}
	}
	if (keysPressed.count(Key_B)) {
		if (m_window->debugOptions() & DebugOption_ShowBoundingBoxes) {
			m_window->debugOptions((DebugOption)(options & ~DebugOption_ShowBoundingBoxes));
		}
		else {
			m_window->debugOptions((DebugOption)(options | DebugOption_ShowBoundingBoxes));
		}
	}
	if (keysPressed.count(Key_I)) {
		if (m_window->debugOptions() & DebugOption_ShowStatsOveray) {
			m_window->debugOptions((DebugOption)(options & ~DebugOption_ShowStatsOveray));
		}
		else {
			m_window->debugOptions((DebugOption)(options | DebugOption_ShowStatsOveray));
		}
	}

	if (keysPressed.count(Key_V)) {
		m_window->enableVSync(!(m_window->vSyncEnabled()));
	}
	
	if (keysPressed.count(Key_Backslash)) {
		SaveSnapshot(*m_window);
	}
	
	if (keysPressed.count(Key_R)) {
		if (!m_window->recordingGIF()) {
			StartGIFRecording(*m_window, 240, 8);
		}
		else {
			StopGIFRecording(*m_window);
		}
	}
	
	// mouselook
	
	vec2 mousePositionDelta = m_inputManager->mousePositionDelta();
	
	static const float mouseSensitivity = (1.0f / MOUSE_SENSITIVITY);
	
	if (!m_cameraNode) {
		for (auto n : scene.rootNode()->childNodes(false)) {
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
		
//		float deltaRotX = deltaSeconds * mousePositionDelta.x / mouseSensitivity;
//		float deltaRotY = deltaSeconds * mousePositionDelta.y / mouseSensitivity;
		
		vec3 angles = m_cameraNode->eulerAngles();
		m_cameraNode->eulerAngles(vec3(angles.x + deltaRotY, angles.y - deltaRotX, 0));
		
		// move
		
		//		const static float MOVE_SPEED = 5.0f; // units/sec
		static float MOVE_SPEED = 0;
		if (!MOVE_SPEED) MOVE_SPEED = Max(scene.extent());
		
		auto keysDown = m_inputManager->keysDown();
		
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

void Test::windowWillRenderCallback(Scene& scene, float time) {
	
}

void Test::windowDidRenderCallback(Scene& scene, float time) {
	
}


