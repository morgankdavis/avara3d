//
//  InputManager.cpp
//	avara-engine
//
//  Created by Morgan Davis on 10/9/17.
//  Copyright © 2017 Morgan K Davis. All rights reserved.
//

#include "InputManager.h"

#include <iostream>

#define GLFW_DLL
#include <GLFW/glfw3.h>

#include "Globals.h"
#include "Window.h"


using namespace ae;
using namespace std;
using namespace glm;


/***************************************************************************************
     MARK:   Globals
 **************************************************************************************/

InputManager *inputManager;

/***************************************************************************************
     MARK:   GLFW Callbacks
 **************************************************************************************/

void glfwMouseButtonCallback(GLFWwindow* glfwWindow, int button, int action, int mods) {
	//cout << "glfwMouseButtonCallback()" << endl;
	
	if (action == GLFW_PRESS) {
		inputManager->m_mouseButtonsDown.insert((MouseButtonCode)button);
	}
}

void glfwCursorPositionCallback(GLFWwindow* glfwWindow, double xPos, double yPos) {
	cout << "glfwCursorPositionCallback()" << endl;
	
	// ignoring in favor of ManyMouse
}

void glfwScrollWheelCallback(GLFWwindow* glfwWindow, double xOffset, double yOffset) {
	cout << "glfwScrollWheelCallback()" << endl;
	
	// ignoring in favor of ManyMouse
}

void glfwKeyCallback(GLFWwindow* glfwWindow, int key, int scancode, int action, int mods) {
	//cout << "glfwKeyCallback()" << endl;

	if (action == GLFW_PRESS) {
		inputManager->m_keysDown.insert((KeyCode)key);
	}

//	const char* keyName = glfwGetKeyName(key, scancode);
//	cout << "keyName: " << keyName << endl;
}

/***************************************************************************************
     MARK:   Lifecycle
 **************************************************************************************/

InputManager::InputManager(Window* window):
		m_keysDown(set<KeyCode>()),
		m_mouseButtonsDown(set<MouseButtonCode>()),
		m_mouseMoveDelta(vec2(0.0f, 0.0f)),
		m_mouseScrollWheelDelta(vec2(0.0f, 0.0f)),
		m_window(window) {

			//m_window->inputManager(shared_from_this());
			
	inputManager = this;
			
			
	clearKeysDown();
	clearMouseDown();
	clearMouseMoveDelta();
	clearMouseScrollWheelDelta();


	// register GLFW callbacks

	// TODO: USE A LOCAL GLFEWINDOW FOR WINDOW!
	GLFWwindow* glfwWindow = g_glfwWindow;

	glfwSetMouseButtonCallback(glfwWindow, glfwMouseButtonCallback);
//	glfwSetCursorPosCallback(glfwWindow, glfwCursorPositionCallback);
//	glfwSetScrollCallback(glfwWindow, glfwScrollWheelCallback);
	glfwSetKeyCallback(glfwWindow, glfwKeyCallback);
}

/***************************************************************************************
     MARK:   Public
 **************************************************************************************/

set<KeyCode> InputManager::keysDown() {
	auto keysDownCopy = m_keysDown;
	clearKeysDown();
	return keysDownCopy;
}

set<MouseButtonCode> InputManager::mouseButtonsDown() {
	auto mosueButtonsDownCopy = m_mouseButtonsDown;
	clearMouseDown();
	return mosueButtonsDownCopy;
}

vec2 InputManager::mouseMoveDelta() {
	auto mouseMoveDeltaCopy = m_mouseMoveDelta;
	clearMouseMoveDelta();
	return mouseMoveDeltaCopy;
}

vec2 InputManager::mouseScrollWheelDelta() {
	auto mouseScrollWheelDeltaCopy = m_mouseScrollWheelDelta;
	clearMouseScrollWheelDelta();
	return mouseScrollWheelDeltaCopy;
}

/***************************************************************************************
     MARK:   Internal
 **************************************************************************************/

void InputManager::unregisterCallbacks() {
	// TODO: USE A LOCAL GLFEWINDOW FOR WINDOW!
	GLFWwindow* glfwWindow = g_glfwWindow;
	
	glfwSetMouseButtonCallback(glfwWindow, NULL);
	glfwSetCursorPosCallback(glfwWindow, NULL);
	glfwSetScrollCallback(glfwWindow, NULL);
	glfwSetKeyCallback(glfwWindow, NULL);
}

Window* InputManager::window() const {
	return m_window;
}

/***************************************************************************************
     MARK:   Private
 **************************************************************************************/

void InputManager::clearKeysDown() {
	m_keysDown = set<KeyCode>();
}

void InputManager::clearMouseDown() {
	m_mouseButtonsDown = set<MouseButtonCode>();
}

void InputManager::clearMouseMoveDelta() {
	m_mouseMoveDelta = vec2(0.0f, 0.0f);
}

void InputManager::clearMouseScrollWheelDelta() {
	m_mouseScrollWheelDelta = vec2(0.0f, 0.0f);
}

