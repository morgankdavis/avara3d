//
//  InputManager.cc
//	avara3d
//
//  Created by Morgan Davis on 10/9/17.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#include "a3d/input/InputManager.h"

#include "a3d/diagnostic/logging/Logger.h"


using namespace a3d;
using namespace std;
using namespace glm;


/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

InputManager::InputManager():
		_keysDown{},
		_mouseButtonsDown{},
		_keysPressed{},
		_keysPressedCleared{},
		_mouseButtonsPressed{},
		_mouseButtonsPressedCleared{},
		_mousePositionDelta{0.0f, 0.0f},
		_mouseScrollWheelDelta{0.0f, 0.0f},
		_scene{} { }

InputManager::~InputManager() {
	A3D_LOG_D("Destroying InputManager {:p}", static_cast<void*>(this));
}

/*********************************************************************************************
	Public
 *********************************************************************************************/

bool InputManager::keyDown(Key key) {
	return _keysDown.count(key);
}

bool InputManager::mouseButtonDown(MouseButton button) {
	return _mouseButtonsDown.count(button);
}

bool InputManager::keyPressed(Key key) {
	bool pressed = _keysPressed.count(key);
	if (pressed) {
		_keysPressed.erase(key);
		_keysPressedCleared.insert(key);
	}
	return pressed;
}

bool InputManager::mouseButtonPressed(MouseButton button) {
	bool pressed = _mouseButtonsPressed.count(button);
	if (pressed) {
		_mouseButtonsPressed.erase(button);
		_mouseButtonsPressedCleared.insert(button);
	}
	return pressed;
}

set<Key> InputManager::keysDown() {
	auto keysDownCopy = _keysDown;
	return keysDownCopy;
}

set<MouseButton> InputManager::mouseButtonsDown() {
	auto mouseButtonsDownCopy = _mouseButtonsDown;
	return mouseButtonsDownCopy;
}

set<Key> InputManager::keysPressed() {
	auto keysPressedCopy = _keysPressed;
	_keysPressed.clear();
	return keysPressedCopy;
}

set<MouseButton> InputManager::mouseButtonsPressed() {
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

void InputManager::attachedToScene(Scene& scene) {
	A3D_LOG_T("scene: {:p}", static_cast<void*>(&scene));

	_scene = &scene;
}

void InputManager::detachedFromScene(Scene& scene) {
	A3D_LOG_T("scene: {:p}", static_cast<void*>(&scene));

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
