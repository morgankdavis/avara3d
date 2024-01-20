//
//  Example.cpp
//	avara-engine
//
//  Created by Morgan Davis on 1/4/18.
//  Copyright © 2018 Morgan K Davis. All rights reserved.
//

#include "Test.h"

#include <iostream>

#include <glm/glm.hpp>

#include "ae/utilities/Utilities.h"


using namespace ae;
using namespace ae::utils;
using namespace glm;
using namespace std;
using namespace std::placeholders;
using namespace test;


constexpr bool					USE_HIGH_DPI =			true;
constexpr unsigned				WINDOW_WIDTH =			800;
constexpr unsigned				WINDOW_HEIGHT =			600;
constexpr bool					FULLSCREEN =			false;
constexpr ANTIALIASING_MODE		ANTIALIAS_MODE =		ANTIALIASING_MODE::NONE;
constexpr bool					ENABLE_VSYNC =			false;
constexpr bool					CAPTURE_CURSOR =		false;
constexpr float					MOUSE_SENSITIVITY =		0.5;


/***************************************************************************************
	Public
 ***************************************************************************************/

int Test::run(const vector<string>& args) {

	//AE_INIT();

	auto logger = make_shared<Logger>("example", Logger::MainLogger()->sinks());
	
	LOG_I(logger, "");


	auto fileSink = make_shared<FileLoggerSink>("log/rotating.log", 20, 1024 * 512);
	auto rotatingLogger = make_shared<Logger>("rotating", static_pointer_cast<LoggerSink>(fileSink));

	for (unsigned l=0; l < 50000; ++l) {
		LOG_I(rotatingLogger, "line {}", l);
	}

	auto window = make_shared<Window>(RENDER_API::OPENGL,
									  FULLSCREEN,
									  WINDOW_WIDTH,
									  WINDOW_HEIGHT,
									  USE_HIGH_DPI,
									  ANTIALIAS_MODE);
	window->vSyncEnabled(ENABLE_VSYNC);
	window->cursorCaptured(CAPTURE_CURSOR);

	auto visualWorld = make_shared<VisualWorld>(window);
	visualWorld->fogStartDistance(500.0);
	visualWorld->fogEndDistance(5000.0);
	visualWorld->fogDensityExponent(1.0);
	visualWorld->fogColor(Color::LightGray());
	visualWorld->background(make_shared<MaterialProperty>(CubeImageNamed("sky1", "png")));
	visualWorld->willRender(bind(&Test::willRenderCallback, this, _1, _2));
	visualWorld->didRender(bind(&Test::didRenderCallback, this, _1, _2));

	auto inputManager = make_shared<WindowInputManager>(window);

	auto scene = make_shared<Scene>(visualWorld, nullptr, inputManager);
	DEBUG_OPTIONS debugOptions = DEBUG_OPTIONS::NONE;
	debugOptions = DEBUG_OPTIONS_ADD(debugOptions, DEBUG_OPTIONS::SHOW_STATS_OVERLAY);
	debugOptions = DEBUG_OPTIONS_ADD(debugOptions, DEBUG_OPTIONS::SHOW_BOUNDING_BOXES);
	scene->debugOptions(debugOptions);
	scene->update(bind(&Test::updateCallback, this, _1, _2));

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

	auto teapotScene = SceneNamed("teapot");
	auto teapotNode = teapotScene->rootNode()->childNamed("teapot", true);
    teapotNode->rotation({1, 0, 0}, radians(30.0));
	scene->rootNode()->addChild(teapotNode);

	auto dragonScene = SceneNamed("dragon", "obj");
	auto dragonNode = dragonScene->rootNode()->childNamed("g default", true);
	dragonNode->scale({2.5, 2.5, 2.5});
	dragonNode->position({50, 0, 0});

	scene->rootNode()->addChild(dragonNode);

	auto boxNode = Node::GeometryNode(make_shared<Box>(1.0, 1.0, 1.0));
	scene->rootNode()->addChild(boxNode);

	// test exception
//	try {
//		scene->rootNode()->addChild(teapotNode);
//	}
//	catch (Exception& e) {
//		AE_LOG_E(e.what());
//	}

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

	auto window = static_pointer_cast<Window>(scene.visualWorld()->renderContext());

	// get input
	
	auto keysPressed = scene.inputManager()->keysPressed();
	
	if (keysPressed.count(KEY::ESCAPE)) {
		window->close();
	}

	if (keysPressed.count(KEY::SLASH)) {
		window->cursorCaptured(!(window->cursorCaptured()));
	}

	if (keysPressed.count(KEY::F)) {
		if (DEBUG_OPTIONS_CONTAINS(scene.debugOptions(), DEBUG_OPTIONS::SHOW_WIREFRAMES)) {
			scene.debugOptions(DEBUG_OPTIONS_REMOVE(scene.debugOptions(),
													DEBUG_OPTIONS::SHOW_WIREFRAMES));
		}
		else {
			scene.debugOptions(DEBUG_OPTIONS_ADD(scene.debugOptions(),
												 DEBUG_OPTIONS::SHOW_WIREFRAMES));
		}
	}
	if (keysPressed.count(KEY::B)) {
		if (DEBUG_OPTIONS_CONTAINS(scene.debugOptions(), DEBUG_OPTIONS::SHOW_BOUNDING_BOXES)) {
			scene.debugOptions(DEBUG_OPTIONS_REMOVE(scene.debugOptions(),
													DEBUG_OPTIONS::SHOW_BOUNDING_BOXES));
		}
		else {
			scene.debugOptions(DEBUG_OPTIONS_ADD(scene.debugOptions(),
												 DEBUG_OPTIONS::SHOW_BOUNDING_BOXES));
		}
	}
	if (keysPressed.count(KEY::I)) {
		if (DEBUG_OPTIONS_CONTAINS(scene.debugOptions(), DEBUG_OPTIONS::SHOW_STATS_OVERLAY)) {
			scene.debugOptions(DEBUG_OPTIONS_REMOVE(scene.debugOptions(),
													DEBUG_OPTIONS::SHOW_STATS_OVERLAY));
		}
		else {
			scene.debugOptions(DEBUG_OPTIONS_ADD(scene.debugOptions(),
												 DEBUG_OPTIONS::SHOW_STATS_OVERLAY));
		}
	}

	if (keysPressed.count(KEY::V)) {
		window->vSyncEnabled(!(window->vSyncEnabled()));
	}
	
	if (keysPressed.count(KEY::BACKSLASH)) {
		SaveSnapshot(*window);
	}
	
	if (keysPressed.count(KEY::R)) {
		if (!window->recordingGIF()) {
			StartGIFRecording(*window, 320, 8);
		}
		else {
			StopGIFRecording(*window);
		}
	}

	if (window->cursorCaptured()) {

		// mouselook

		vec2 mousePositionDelta = scene.inputManager()->mousePositionDelta();

		auto pov = scene.visualWorld()->pointOfView();
		if (pov) {

			// look

			vec3 camForward = pov->worldForward();
			vec3 camRight = pov->worldRight();
			vec3 camUp = pov->worldUp();

			static const float MOUSE_SPEED_SCALAR = .002;
			static const float MOUSE_SPEED = MOUSE_SENSITIVITY * MOUSE_SPEED_SCALAR;

			float deltaRotX = atan(MOUSE_SPEED * mousePositionDelta.x);
			float deltaRotY = atan(MOUSE_SPEED * mousePositionDelta.y);

			vec3 angles = pov->eulerAngles();
			pov->eulerAngles(vec3(angles.x + deltaRotY, angles.y - deltaRotX, 0));

			// move

			static float MOVE_SPEED = Max(scene.rootNode()->extent());

			auto keysDown = scene.inputManager()->keysDown();

			if (keysDown.count(KEY::W)) {
				vec3 positionDelta = deltaSeconds * MOVE_SPEED * camForward;
				pov->position(pov->position() + positionDelta);
			} else if (keysDown.count(KEY::S)) {
				vec3 positionDelta = deltaSeconds * MOVE_SPEED * -camForward;
				pov->position(pov->position() + positionDelta);
			}

			if (keysDown.count(KEY::A)) {
				vec3 positionDelta = deltaSeconds * MOVE_SPEED * -camRight;
				pov->position(pov->position() + positionDelta);
			} else if (keysDown.count(KEY::D)) {
				vec3 positionDelta = deltaSeconds * MOVE_SPEED * camRight;
				pov->position(pov->position() + positionDelta);
			}

			if (keysDown.count(KEY::SPACE)) {
				vec3 positionDelta = deltaSeconds * MOVE_SPEED * camUp;
				pov->position(pov->position() + positionDelta);
			}
		}
	}
}

/***************************************************************************************
	VisualWorld Callbacks
 ***************************************************************************************/

void Test::willRenderCallback(VisualWorld& world, float time) {

}

void Test::didRenderCallback(VisualWorld& world, float time) {

}
