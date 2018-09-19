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


constexpr bool					USE_HIGH_DPI =			true;
constexpr unsigned				WINDOW_WIDTH =			800;
constexpr unsigned				WINDOW_HEIGHT =			600;
constexpr bool					FULLSCREEN =			false;
constexpr ANTIALIASING_MODE		ANTIALIAS_MODE =		ANTIALIASING_MODE::NONE;
constexpr bool					ENABLE_VSYNC =			false;
constexpr bool					CAPTURE_CURSOR =		true;
constexpr float					MOUSE_SENSITIVITY =		0.5;


/***************************************************************************************
     Public
 ***************************************************************************************/

int Test::run(const vector<string>& args) {
	AE_INIT();
	
	m_logger = make_shared<Logger>("test", Logger::MainLogger()->sinks());
	
	LOG_I(m_logger, "Test::run()");

	m_window = make_shared<Window>(FULLSCREEN,
								   WINDOW_WIDTH, WINDOW_HEIGHT,
								   USE_HIGH_DPI,
								   ANTIALIAS_MODE,
								   RENDER_API::OPENGL);
	m_window->updateCallback(bind(&Test::updateCallback, this, _1, _2));
	m_window->willRenderCallback(bind(&Test::willRenderCallback, this, _1, _2));
	m_window->didRenderCallback(bind(&Test::didRenderCallback, this, _1, _2));
	m_window->enableVSync(ENABLE_VSYNC);
	m_window->captureCursor(CAPTURE_CURSOR);
	DEBUG_OPTIONS debugOptions = DEBUG_OPTIONS::NONE;
	debugOptions = DEBUG_OPTIONS_ADD(debugOptions, DEBUG_OPTIONS::SHOW_STATS_OVERLAY);
	debugOptions = DEBUG_OPTIONS_ADD(debugOptions, DEBUG_OPTIONS::SHOW_BOUNDING_BOXES);
	m_window->debugOptions(debugOptions);
	

	auto scene = make_shared<Scene>();
	scene->rootNode(make_shared<Node>("Root node"));
	

	auto teapotScene = SceneNamed("teapot");
	auto teapotNode = teapotScene->rootNode()->children(true)[1];
    teapotNode->rotation({1, 0, 0, radians(30.0)});
	scene->rootNode()->addChild(teapotNode);

	auto dragonScene = SceneNamed("dragon", "obj");
	auto dragonNode = dragonScene->rootNode()->children(true)[0];
	dragonNode->scale({2.5, 2.5, 2.5});
	dragonNode->position({50, 0, 0});
	scene->rootNode()->addChild(dragonNode);
	
	
	// test exception
	try {
		scene->rootNode()->addChild(teapotNode);
	}
	catch (Exception& e) {
		
//		LOG_E(m_logger, e.what());
		//logger->error(e.what());
	}
	

	auto background = make_shared<MaterialProperty>(CubeImageNamed("sky1", "png"));
	scene->background(background);

	auto ambientLight = make_shared<Light>(LIGHT_TYPE::AMBIENT, make_shared<Color>(0.25, 0.25, 0.25, 1.0));
	auto ambientLightNode = make_shared<Node>("Ambient light");
	ambientLightNode->light(ambientLight);
	scene->rootNode()->addChild(ambientLightNode);

	auto pointLight = make_shared<Light>(LIGHT_TYPE::POINT, Color::White());
	pointLight->attenuationFactor(0.0000015);
	auto pointLightNode = make_shared<Node>();
	pointLightNode->light(pointLight);
	scene->rootNode()->addChild(pointLightNode);

	pointLightNode->position({100.0, 20.0, 20.0});

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
	scene->fogColor(Color::LightGray());

	
	m_window->scene(scene);
	m_inputManager = m_window->inputManager();
	m_window->display();
	
	return 0;
}

/***************************************************************************************
     RenderContext Callbacks
 ***************************************************************************************/

void Test::updateCallback(RenderContext& renderContext, float time) {
	
	static float previousSeconds = time;
	float deltaSeconds = time - previousSeconds;
	previousSeconds = time;
	
	auto scene = renderContext.scene();
	
	// get input
	
	auto keysPressed = m_inputManager->keysPressed();
	
	if (keysPressed.count(KEY::ESCAPE)) {
		m_window->setShouldClose();
	}
	
	if (keysPressed.count(KEY::F)) {
		if (DEBUG_OPTIONS_CONTAINS(renderContext.debugOptions(), DEBUG_OPTIONS::SHOW_WIREFRAMES)) {
			renderContext.debugOptions(DEBUG_OPTIONS_REMOVE(renderContext.debugOptions(),
															DEBUG_OPTIONS::SHOW_WIREFRAMES));
		}
		else {
			renderContext.debugOptions(DEBUG_OPTIONS_ADD(renderContext.debugOptions(),
														 DEBUG_OPTIONS::SHOW_WIREFRAMES));
		}
	}
	if (keysPressed.count(KEY::B)) {
		if (DEBUG_OPTIONS_CONTAINS(renderContext.debugOptions(), DEBUG_OPTIONS::SHOW_BOUNDING_BOXES)) {
			renderContext.debugOptions(DEBUG_OPTIONS_REMOVE(renderContext.debugOptions(),
															DEBUG_OPTIONS::SHOW_BOUNDING_BOXES));
		}
		else {
			renderContext.debugOptions(DEBUG_OPTIONS_ADD(renderContext.debugOptions(),
														 DEBUG_OPTIONS::SHOW_BOUNDING_BOXES));
		}
	}
	if (keysPressed.count(KEY::I)) {
		if (DEBUG_OPTIONS_CONTAINS(renderContext.debugOptions(), DEBUG_OPTIONS::SHOW_STATS_OVERLAY)) {
			renderContext.debugOptions(DEBUG_OPTIONS_REMOVE(renderContext.debugOptions(),
															DEBUG_OPTIONS::SHOW_STATS_OVERLAY));
		}
		else {
			renderContext.debugOptions(DEBUG_OPTIONS_ADD(renderContext.debugOptions(),
														 DEBUG_OPTIONS::SHOW_STATS_OVERLAY));
		}
	}

	if (keysPressed.count(KEY::V)) {
		m_window->enableVSync(!(m_window->vSyncEnabled()));
	}
	
	if (keysPressed.count(KEY::BACKSLASH)) {
		SaveSnapshot(*m_window);
	}
	
	if (keysPressed.count(KEY::R)) {
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
		for (auto n : scene->rootNode()->children(false)) {
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
		if (!MOVE_SPEED) MOVE_SPEED = Max(scene->extent());
		
		auto keysDown = m_inputManager->keysDown();
		
		if(keysDown.count(KEY::W)) {
			vec3 positionDelta = deltaSeconds * MOVE_SPEED * camForward;
			m_cameraNode->position(m_cameraNode->position() + positionDelta);
		}
		else if(keysDown.count(KEY::S)) {
			vec3 positionDelta = deltaSeconds * MOVE_SPEED * -camForward;
			m_cameraNode->position(m_cameraNode->position() + positionDelta);
		}
		
		if(keysDown.count(KEY::A)) {
			vec3 positionDelta = deltaSeconds * MOVE_SPEED * -camRight;
			m_cameraNode->position(m_cameraNode->position() + positionDelta);
		}
		else if(keysDown.count(KEY::D)) {
			vec3 positionDelta = deltaSeconds * MOVE_SPEED * camRight;
			m_cameraNode->position(m_cameraNode->position() + positionDelta);
		}
		
		if(keysDown.count(KEY::SPACE)) {
			vec3 positionDelta = deltaSeconds * MOVE_SPEED * camUp;
			m_cameraNode->position(m_cameraNode->position() + positionDelta);
		}
	}
}

void Test::willRenderCallback(RenderContext& renderContext, float time) {
	
}

void Test::didRenderCallback(RenderContext& renderContext, float time) {
	
}


