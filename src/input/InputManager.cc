//
//  InputManager.cc
//	avara-engine
//
//  Created by Morgan Davis on 10/9/17.
//  Copyright © 2017 Morgan K Davis. All rights reserved.
//

#include "ae/input/InputManager.h"

#include "ae/diagnostic/logging/Logger.h"


using namespace ae;
using namespace std;
using namespace glm;


/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

InputManager::InputManager():
		_keysDown(set<KEY>()),
		_mouseButtonsDown(set<MOUSE_BUTTON>()),
		_keysPressed(set<KEY>()),
		_keysPressedCleared(set<KEY>()),
		_mouseButtonsPressed(set<MOUSE_BUTTON>()),
		_mouseButtonsPressedCleared(set<MOUSE_BUTTON>()),
		_mousePositionDelta(vec2(0.0f, 0.0f)),
		_mouseScrollWheelDelta(vec2(0.0f, 0.0f)),
		_scene(nullptr) { }

InputManager::~InputManager() {
	AE_LOG_D("Destroying InputManager {:p}", static_cast<void*>(this));
}

/*********************************************************************************************
	Public
 *********************************************************************************************/

bool InputManager::keyDown(KEY key) {
	return _keysDown.count(key);
}

bool InputManager::mouseButtonDown(MOUSE_BUTTON button) {
	return _mouseButtonsDown.count(button);
}

bool InputManager::keyPressed(KEY key) {
	bool pressed = _keysPressed.count(key);
	if (pressed) {
		_keysPressed.erase(key);
		_keysPressedCleared.insert(key);
	}
	return pressed;
}

bool InputManager::mouseButtonPressed(MOUSE_BUTTON button) {
	bool pressed = _mouseButtonsPressed.count(button);
	if (pressed) {
		_mouseButtonsPressed.erase(button);
		_mouseButtonsPressedCleared.insert(button);
	}
	return pressed;
}

set<KEY> InputManager::keysDown() {
	auto keysDownCopy = _keysDown;
	return keysDownCopy;
}

set<MOUSE_BUTTON> InputManager::mouseButtonsDown() {
	auto mouseButtonsDownCopy = _mouseButtonsDown;
	return mouseButtonsDownCopy;
}

set<KEY> InputManager::keysPressed() {
	auto keysPressedCopy = _keysPressed;
	_keysPressed.clear();
	return keysPressedCopy;
}

set<MOUSE_BUTTON> InputManager::mouseButtonsPressed() {
	auto mouseButtonsPressedCopy = _mouseButtonsPressed;
	_mouseButtonsPressed.clear();
	return mouseButtonsPressedCopy;
}

vec2 InputManager::mousePositionDelta() {
	auto mouseMoveDeltaCopy = _mousePositionDelta;
	clearMousePositionDelta();
	return mouseMoveDeltaCopy;
}

vec2 InputManager::mouseScrollWheelDelta() {
	auto mouseScrollWheelDeltaCopy = _mouseScrollWheelDelta;
	clearMouseScrollWheelDelta();
	return mouseScrollWheelDeltaCopy;
}

Scene* InputManager::scene() const {
	return _scene;
}

/*********************************************************************************************
	Internal
 *********************************************************************************************/

void InputManager::attachedToScene(Scene* scene) {
	AE_LOG_T("scene: {:p}", static_cast<void*>(scene));

	_scene = scene;
}

void InputManager::detachedFromScene(Scene* scene) {
	AE_LOG_T("scene: {:p}", static_cast<void*>(scene));

	_scene = nullptr;
}

/*********************************************************************************************
	Private
 *********************************************************************************************/

void InputManager::clearMousePositionDelta() {
	_mousePositionDelta.x = 0.0f;
	_mousePositionDelta.y = 0.0f;
}

void InputManager::clearMouseScrollWheelDelta() {
	_mouseScrollWheelDelta.x = 0.0f;
	_mouseScrollWheelDelta.y = 0.0f;
}
