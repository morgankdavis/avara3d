//
//  InputManager.cc
//	avara-engine
//
//  Created by Morgan Davis on 10/9/17.
//  Copyright © 2017 Morgan K Davis. All rights reserved.
//

#include "InputManager.h"

#include <iostream>

#include "Exception.h"
#include "Logger.h"


using namespace ae;
using namespace std;
using namespace glm;


/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

InputManager::InputManager():
	m_keysDown(set<KEY>()),
	m_mouseButtonsDown(set<MOUSE_BUTTON>()),
	m_keysPressed(set<KEY>()),
	m_keysPressedCleared(set<KEY>()),
	m_mouseButtonsPressed(set<MOUSE_BUTTON>()),
	m_mouseButtonsPressedCleared(set<MOUSE_BUTTON>()),
	m_mousePositionDelta(vec2(0.0f, 0.0f)),
	m_mouseScrollWheelDelta(vec2(0.0f, 0.0f)) {

		//inputManager = this;
}

InputManager::~InputManager() {

}

/*********************************************************************************************
	Public
 *********************************************************************************************/

bool InputManager::keyDown(KEY key) {
	return m_keysDown.count(key);
}

bool InputManager::mouseButtonDown(MOUSE_BUTTON button) {
	return m_mouseButtonsDown.count(button);
}

bool InputManager::keyPressed(KEY key) {
	bool pressed = m_keysPressed.count(key);
	if (pressed) {
		m_keysPressed.erase(key);
		m_keysPressedCleared.insert(key);
	}
	return pressed;
}

bool InputManager::mouseButtonPressed(MOUSE_BUTTON button) {
	bool pressed = m_mouseButtonsPressed.count(button);
	if (pressed) {
		m_mouseButtonsPressed.erase(button);
		m_mouseButtonsPressedCleared.insert(button);
	}
	return pressed;
}

set<KEY> InputManager::keysDown() {
	auto keysDownCopy = m_keysDown;
	return keysDownCopy;
}

set<MOUSE_BUTTON> InputManager::mouseButtonsDown() {
	auto mouseButtonsDownCopy = m_mouseButtonsDown;
	return mouseButtonsDownCopy;
}

set<KEY> InputManager::keysPressed() {
	auto keysPressedCopy = m_keysPressed;
	m_keysPressed.clear();
	return keysPressedCopy;
}

set<MOUSE_BUTTON> InputManager::mouseButtonsPressed() {
	auto mouseButtonsPressedCopy = m_mouseButtonsPressed;
	m_mouseButtonsPressed.clear();
	return mouseButtonsPressedCopy;
}

vec2 InputManager::mousePositionDelta() {
//	if (m_mousePositionDelta.x != 0 && m_mousePositionDelta.y != 0) {
//		AE_LOG_D("MOUSE DELTA: ({}, {})", m_mousePositionDelta.x, m_mousePositionDelta.y);
//	}

	auto mouseMoveDeltaCopy = m_mousePositionDelta;
	clearMousePositionDelta();
	return mouseMoveDeltaCopy;
}

vec2 InputManager::mouseScrollWheelDelta() {
	auto mouseScrollWheelDeltaCopy = m_mouseScrollWheelDelta;
	clearMouseScrollWheelDelta();
	return mouseScrollWheelDeltaCopy;
}

/*********************************************************************************************
	Protexted
 *********************************************************************************************/

void InputManager::clearMousePositionDelta() {
	m_mousePositionDelta.x = 0.0f;
	m_mousePositionDelta.y = 0.0f;
}

void InputManager::clearMouseScrollWheelDelta() {
	m_mouseScrollWheelDelta.x = 0.0f;
	m_mouseScrollWheelDelta.y = 0.0f;
}
