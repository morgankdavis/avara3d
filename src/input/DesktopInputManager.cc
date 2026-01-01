//
//  DesktopInputManager.cc
//  avara3d
//
//  Created by Morgan Davis on 12/2/2025.
//  Copyright © 2025 Morgan K Davis. All rights reserved.
//

#include "a3d/input/DesktopInputManager.h"

#include "a3d/log/Log.h"

using namespace a3d;
using namespace std;
using namespace a3d::math;

/// Public Lifecycle Functions ///

DesktopInputManager::DesktopInputManager():
		_keysDown{},
		_mouseButtonsDown{},
		_keysPressed{},
		_keysPressedCleared{},
		_mouseButtonsPressed{},
		_mouseButtonsPressedCleared{},
		_mousePositionDelta{0.0f, 0.0f},
		_mouseScrollWheelDelta{0.0f, 0.0f} {}

DesktopInputManager::~DesktopInputManager() {
	log::d()("Destroying InputManager {:p}", static_cast<void*>(this));
}

/// Public Member Functions ///

bool DesktopInputManager::keyDown(Key key) {
	return _keysDown.count(key);
}

bool DesktopInputManager::mouseButtonDown(MouseButton button) {
	return _mouseButtonsDown.count(button);
}

bool DesktopInputManager::keyPressed(Key key) {
	bool pressed = _keysPressed.count(key);
	if (pressed) {
		_keysPressed.erase(key);
		_keysPressedCleared.insert(key);
	}
	return pressed;
}

bool DesktopInputManager::mouseButtonPressed(MouseButton button) {
	bool pressed = _mouseButtonsPressed.count(button);
	if (pressed) {
		_mouseButtonsPressed.erase(button);
		_mouseButtonsPressedCleared.insert(button);
	}
	return pressed;
}

unordered_set<Key> DesktopInputManager::keysDown() {
	auto keysDownCopy = _keysDown;
	return keysDownCopy;
}

unordered_set<MouseButton> DesktopInputManager::mouseButtonsDown() {
	auto mouseButtonsDownCopy = _mouseButtonsDown;
	return mouseButtonsDownCopy;
}

unordered_set<Key> DesktopInputManager::keysPressed() {
	auto keysPressedCopy = _keysPressed;
	_keysPressed.clear();
	return keysPressedCopy;
}

unordered_set<MouseButton> DesktopInputManager::mouseButtonsPressed() {
	auto mouseButtonsPressedCopy = _mouseButtonsPressed;
	_mouseButtonsPressed.clear();
	return mouseButtonsPressedCopy;
}

vec2 DesktopInputManager::mousePositionDelta() {
	auto mouseMoveDeltaCopy = _mousePositionDelta;
	clearMousePositionDelta();
	return mouseMoveDeltaCopy;
}

vec2 DesktopInputManager::mouseScrollWheelDelta() {
	auto mouseScrollWheelDeltaCopy = _mouseScrollWheelDelta;
	clearMouseScrollWheelDelta();
	return mouseScrollWheelDeltaCopy;
}

void DesktopInputManager::clearMousePositionDelta() {
	_mousePositionDelta.x = 0.0f;
	_mousePositionDelta.y = 0.0f;
}

void DesktopInputManager::clearMouseScrollWheelDelta() {
	_mouseScrollWheelDelta.x = 0.0f;
	_mouseScrollWheelDelta.y = 0.0f;
}
