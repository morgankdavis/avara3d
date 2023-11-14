//
//  Example.cpp
//	avara-engine
//
//  Created by Morgan Davis on 1/4/18.
//  Copyright © 2018 Morgan K Davis. All rights reserved.
//

#include "Example.h"

#include <iostream>

#include <glm/glm.hpp>

#include "utilities/Utilities.h"


using namespace ae;
using namespace ae::utils;
using namespace example;
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

int Example::run(const vector<string>& args) {
	AE_INIT();

	_logger = make_shared<Logger>("example", Logger::MainLogger()->sinks());
	
	LOG_I(_logger, "Example::run()");

	
//	auto nodeA = make_shared<Node>();
//	auto nodeB = make_shared<Node>();
//	nodeA->addChild(nodeB);
//	nodeA->addChild(nodeB);
	
	
	auto fileSink = make_shared<FileLoggerSink>("log/rotating.log", 20, 1024 * 512);
	auto rotatingLogger = make_shared<Logger>("rotating", static_pointer_cast<LoggerSink>(fileSink));
//	unsigned l = 0;
//	while (true) {
//		LOG_I(rotatingLogger, "line {}", l);
//		++l;
//	}
	for (unsigned l=0; l < 50000; ++l) {
		LOG_I(rotatingLogger, "line {}", l);
	}



	_window = make_shared<Window>(FULLSCREEN,
								  WINDOW_WIDTH, WINDOW_HEIGHT,
								  USE_HIGH_DPI,
								  ANTIALIAS_MODE,
								  RENDER_API::OPENGL);
	_window->updateCallback(bind(&Example::updateCallback, this, _1, _2));
	_window->willRenderCallback(bind(&Example::willRenderCallback, this, _1, _2));
	_window->didRenderCallback(bind(&Example::didRenderCallback, this, _1, _2));
	_window->enableVSync(ENABLE_VSYNC);
	_window->captureCursor(CAPTURE_CURSOR);
	DEBUG_OPTIONS debugOptions = DEBUG_OPTIONS::NONE;
	debugOptions = DEBUG_OPTIONS_ADD(debugOptions, DEBUG_OPTIONS::SHOW_STATS_OVERLAY);
	debugOptions = DEBUG_OPTIONS_ADD(debugOptions, DEBUG_OPTIONS::SHOW_BOUNDING_BOXES);
	_window->debugOptions(debugOptions);
	

	auto scene = make_shared<Scene>();
	scene->rootNode(make_shared<Node>("Root node"));
	

	auto teapotScene = SceneNamed("teapot");
	auto teapotNode = teapotScene->rootNode()->children(true)[1];
    teapotNode->rotation({1, 0, 0}, radians(30.0));
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
		
//		LOG_E(_logger, e.what());
		//logger->error(e.what());
	}
	

	auto background = make_shared<MaterialProperty>(CubeImageNamed("sky1", "png"));
	scene->background(background);

	auto ambientLight = make_shared<Light>(LIGHT_TYPE::AMBIENT, make_shared<Color>(0.25f, 0.25, 0.25, 1.0));
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

	
	_window->scene(scene);
	_inputManager = _window->inputManager();
	_window->display();
	
	return 0;
}

/***************************************************************************************
	RenderContext Callbacks
 ***************************************************************************************/

void Example::updateCallback(RenderContext& renderContext, float time) {
	
	static float previousSeconds = time;
	float deltaSeconds = time - previousSeconds;
	previousSeconds = time;
	
	auto scene = renderContext.scene();
	
	// get input
	
	auto keysPressed = _inputManager->keysPressed();
	
	if (keysPressed.count(KEY::ESCAPE)) {
		_window->setShouldClose();
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
		_window->enableVSync(!(_window->vSyncEnabled()));
	}
	
	if (keysPressed.count(KEY::BACKSLASH)) {
		SaveSnapshot(*_window);
	}
	
	if (keysPressed.count(KEY::R)) {
		if (!_window->recordingGIF()) {
			StartGIFRecording(*_window, 240, 8);
		}
		else {
			StopGIFRecording(*_window);
		}
	}
	
	// mouselook
	
	vec2 mousePositionDelta = _inputManager->mousePositionDelta();
	
	static const float mouseSensitivity = (1.0f / MOUSE_SENSITIVITY);
	
	if (!_cameraNode) {
		for (auto n : scene->rootNode()->children(false)) {
			if (n->camera()) {
				_cameraNode = n;
				break;
			}
		}
	}
	
	if (_cameraNode) {
		
		// look
		
		vec3 camForward = _cameraNode->worldForward();
		vec3 camRight = _cameraNode->worldRight();
		vec3 camUp = _cameraNode->worldUp();
		
		float deltaRotX = atan(deltaSeconds * mousePositionDelta.x / mouseSensitivity);
		float deltaRotY = atan(deltaSeconds * mousePositionDelta.y / mouseSensitivity);
		
//		float deltaRotX = deltaSeconds * mousePositionDelta.x / mouseSensitivity;
//		float deltaRotY = deltaSeconds * mousePositionDelta.y / mouseSensitivity;
		
		vec3 angles = _cameraNode->eulerAngles();
		_cameraNode->eulerAngles(vec3(angles.x + deltaRotY, angles.y - deltaRotX, 0));
		
		// move
		
		//		const static float MOVE_SPEED = 5.0f; // units/sec
		static float MOVE_SPEED = 0;
		if (!MOVE_SPEED) MOVE_SPEED = Max(scene->extent());
		
		auto keysDown = _inputManager->keysDown();
		
		if(keysDown.count(KEY::W)) {
			vec3 positionDelta = deltaSeconds * MOVE_SPEED * camForward;
			_cameraNode->position(_cameraNode->position() + positionDelta);
		}
		else if(keysDown.count(KEY::S)) {
			vec3 positionDelta = deltaSeconds * MOVE_SPEED * -camForward;
			_cameraNode->position(_cameraNode->position() + positionDelta);
		}
		
		if(keysDown.count(KEY::A)) {
			vec3 positionDelta = deltaSeconds * MOVE_SPEED * -camRight;
			_cameraNode->position(_cameraNode->position() + positionDelta);
		}
		else if(keysDown.count(KEY::D)) {
			vec3 positionDelta = deltaSeconds * MOVE_SPEED * camRight;
			_cameraNode->position(_cameraNode->position() + positionDelta);
		}
		
		if(keysDown.count(KEY::SPACE)) {
			vec3 positionDelta = deltaSeconds * MOVE_SPEED * camUp;
			_cameraNode->position(_cameraNode->position() + positionDelta);
		}
	}
}

void Example::willRenderCallback(RenderContext& renderContext, float time) {
	
}

void Example::didRenderCallback(RenderContext& renderContext, float time) {
	
}


