//
//  AndroidInputManager.cpp
//	avara-engine
//
//  Created by Morgan Davis on 5/6/18.
//  Copyright © 2018 Morgan K Davis. All rights reserved.
//

#ifdef ANDROID


#include "AndroidInputManager.h"

#include <android_native_app_glue.h>
#include <android/native_window_jni.h>
#include <iostream>

#include "Activity.h"
#include "Exception.h"
#include "Logger.h"


using namespace ae;
using namespace std;
using namespace glm;


#define FLIP_MOUSE_VERTICAL		true
#define FLIP_MOUSE_HORIZONTAL	false


/***************************************************************************************
     Lifecycle
 ***************************************************************************************/

AndroidInputManager::AndroidInputManager(shared_ptr<Activity> activity)):
	InputManager(),
	m_activity(activity) {

}

AndroidInputManager::~AndroidInputManager() {

}

/***************************************************************************************
     Public
 ***************************************************************************************/

//bool InputManager::keyDown(KEY key) {
//	return m_keysDown.count(key);
//}
//
//bool InputManager::mouseButtonDown(MOUSE_BUTTON button) {
//	return m_mouseButtonsDown.count(button);
//}
//
//bool InputManager::keyPressed(KEY key) {
//	bool pressed = inputManager->m_keysPressed.count(key);
//	if (pressed) {
//		inputManager->m_keysPressed.erase(key);
//		inputManager->m_keysPressedCleared.insert(key);
//	}
//	return pressed;
//}
//
//bool InputManager::mouseButtonPressed(MOUSE_BUTTON button) {
//	bool pressed = inputManager->m_mouseButtonsPressed.count(button);
//	if (pressed) {
//		inputManager->m_mouseButtonsPressed.erase(button);
//		inputManager->m_mouseButtonsPressedCleared.insert(button);
//	}
//	return pressed;
//}
//
//set<KEY> InputManager::keysDown() {
//	auto keysDownCopy = m_keysDown;
//	return keysDownCopy;
//}
//
//set<MOUSE_BUTTON> InputManager::mouseButtonsDown() {
//	auto mouseButtonsDownCopy = m_mouseButtonsDown;
//	return mouseButtonsDownCopy;
//}
//
//set<KEY> InputManager::keysPressed() {
//	auto keysPressedCopy = m_keysPressed;
//	m_keysPressed.clear();
//	return keysPressedCopy;
//}
//
//set<MOUSE_BUTTON> InputManager::mouseButtonsPressed() {
//	auto mouseButtonsPressedCopy = m_mouseButtonsPressed;
//	m_mouseButtonsPressed.clear();
//	return mouseButtonsPressedCopy;
//}
//
//vec2 InputManager::mousePositionDelta() {
////	if (m_mousePositionDelta.x != 0 && m_mousePositionDelta.y != 0) {
////		cout << "DELTA: " << m_mousePositionDelta.x << ", " << m_mousePositionDelta.y << endl;
////	}
//	auto mouseMoveDeltaCopy = m_mousePositionDelta;
//	clearMousePositionDelta();
//	return mouseMoveDeltaCopy;
//}
//
//vec2 InputManager::mouseScrollWheelDelta() {
//	auto mouseScrollWheelDeltaCopy = m_mouseScrollWheelDelta;
//	clearMouseScrollWheelDelta();
//	return mouseScrollWheelDeltaCopy;
//}

/***************************************************************************************
     Internal
 ***************************************************************************************/

void AndroidInputManager::update(AInputEvent* event) {
	
}

//weak_ptr<Activity> AndroidInputManager::activity() const {
//	return m_activity;
//}

#endif // ANDROID
