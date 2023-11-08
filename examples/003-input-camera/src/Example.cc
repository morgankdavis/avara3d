//
//  Example.cc
//	avara-engine
//
//  Created by Morgan Davis on 10/15/17.
//  Copyright © 2017 Morgan K Davis. All rights reserved.
//

#include "Example.h"

#include <iostream>

#include <glm/gtc/matrix_transform.hpp>

#include "Utilities.h"


using namespace ae;
using namespace ae::utils;
using namespace example;
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

int Example::run(const vector<string>& args) {
	cout << "Example::run()\n" << endl;
	
	AE_INIT();

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
	
	auto scene = SceneNamed("importTest");

	
	// ******** make everything look like it did before materials worked ********
	
	auto ambientProperty = make_shared<MaterialProperty>(make_shared<Color>(0.75f, 0.75, 0.75, 1.0));
	auto diffuseProperty = make_shared<MaterialProperty>(make_shared<Color>(1.0f, 1.0, 1.0, 1.0));
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
	
	auto keysDown = _inputManager->keysDown();
	for (auto k : keysDown) {
		cout << "Key: " << static_cast<underlying_type<KEY>::type>(k) << endl;
	}

	if (keysDown.count(KEY::SLASH)) {
		_window->captureCursor(!(_window->cursorCaptured()));
	}
	
	if (keysDown.count(KEY::ESCAPE)) {
		exit(0);
	}
	
	for (auto mb : _inputManager->mouseButtonsDown()) {
		cout << "Mouse button: " << static_cast<underlying_type<MOUSE_BUTTON>::type>(mb) << endl;
	}
	
	vec2 mousePositionDelta = _inputManager->mousePositionDelta();
	//	if (mousePositionDelta.x || mousePositionDelta.y) {
	//		cout << "Mouse move delta: (" << mousePositionDelta.x << ", " << mousePositionDelta.y << ")" << endl;
	//	}
	
	vec2 mouseScrollWheelDelta = _inputManager->mouseScrollWheelDelta();
	if (mouseScrollWheelDelta.x || mouseScrollWheelDelta.y) {
		cout << "Mouse scroll wheel delta: (" << mouseScrollWheelDelta.x << ", "
		<< mouseScrollWheelDelta.y << ")" << endl;
	}
	
	// move camera

	//const static float mouseSensitivity = 0.5f;
	const static float mouseSensitivity = (1.0f / 1.5f);
	
	
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
		
		
		//		float deltaRotX = deltaSeconds * mouseSensitivity * mousePositionDelta.x;
		//		float deltaRotY = deltaSeconds * mouseSensitivity * mousePositionDelta.y;
		
		// tanA = y/x
		// tanA = mouseDelta / distance
		// A = atan(mouseDelta / distance)
		
		float deltaRotX = atan(deltaSeconds * mousePositionDelta.x / mouseSensitivity);
		float deltaRotY = atan(deltaSeconds * mousePositionDelta.y / mouseSensitivity);
		
		vec3 angles = _cameraNode->eulerAngles();
		// weird angles
		//_cameraNode->eulerAngles(vec3(angles.x + -deltaRotX, 0, angles.z + deltaRotY));
		// pitch, yaw, roll
		_cameraNode->eulerAngles(vec3(angles.x + deltaRotY, angles.y - deltaRotX, 0));
		
		
		// move
		
		static float MOVE_SPEED = 0;
		if (!MOVE_SPEED) MOVE_SPEED = Max(scene->extent());
		
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
