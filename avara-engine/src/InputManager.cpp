//
//  InputManager.cpp
//	avara-engine
//
//  Created by Morgan Davis on 10/9/17.
//  Copyright © 2017 Morgan K Davis. All rights reserved.
//

#include "InputManager.h"

#include <iostream>

#include "manymouse/manymouse.h"

#include "Logger.h"
#include "Window.h"


using namespace ae;
using namespace std;
using namespace glm;


/***************************************************************************************
     MARK:   Globals
 **************************************************************************************/

InputManager* inputManager;

/***************************************************************************************
     MARK:   Lifecycle
 **************************************************************************************/

InputManager::InputManager(Window* window):
	m_keysDown(set<Key>()),
	m_mouseButtonsDown(set<MouseButton>()),
	m_keysPressed(set<Key>()),
	m_keysPressedCleared(set<Key>()),
	m_mouseButtonsPressed(set<MouseButton>()),
	m_mouseButtonsPressedCleared(set<MouseButton>()),
	m_mousePositionDelta(vec2(0.0f, 0.0f)),
	m_mouseScrollWheelDelta(vec2(0.0f, 0.0f)),
	m_window(window) {

		inputManager = this;
		
		registerGLFWCallbacks(window->glfwWindow());
		initManyMouse();
}

InputManager::~InputManager() {
	quitManyMouse();
	unregisterGLFWCallbacks(m_window->glfwWindow());
}

/***************************************************************************************
     MARK:   Public
 **************************************************************************************/

bool InputManager::keyDown(Key key) {
	return m_keysDown.count(key);
}

bool InputManager::mouseButtonDown(MouseButton button) {
	return m_mouseButtonsDown.count(button);
}

bool InputManager::keyPressed(Key key) {
	bool pressed = inputManager->m_keysPressed.count(key);
	if (pressed) {
		inputManager->m_keysPressed.erase(key);
		inputManager->m_keysPressedCleared.insert(key);
	}
	return pressed;
}

bool InputManager::mouseButtonPressed(MouseButton button) {
	bool pressed = inputManager->m_mouseButtonsPressed.count(button);
	if (pressed) {
		inputManager->m_mouseButtonsPressed.erase(button);
		inputManager->m_mouseButtonsPressedCleared.insert(button);
	}
	return pressed;
}

set<Key> InputManager::keysDown() {
	auto keysDownCopy = m_keysDown;
	return keysDownCopy;
}

set<MouseButton> InputManager::mouseButtonsDown() {
	auto mosueButtonsDownCopy = m_mouseButtonsDown;
	return mosueButtonsDownCopy;
}

set<Key> InputManager::keysPressed() {
	auto keysPressedCopy = m_keysPressed;
	m_keysPressed.clear();
	return keysPressedCopy;
}

set<MouseButton> InputManager::mouseButtonsPressed() {
	auto mouseButtonsPressedCopy = m_mouseButtonsPressed;
	m_mouseButtonsPressed.clear();
	return mouseButtonsPressedCopy;
}

vec2 InputManager::mousePositionDelta() {
	auto mouseMoveDeltaCopy = m_mousePositionDelta;
	clearMousePositionDelta();
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

void InputManager::update(float deltaSeconds) {
	
//	cout << "update()" << endl;

	static ManyMouseEvent event;

	while (ManyMouse_PollEvent(&event)) {

		switch(event.type) {

			case MANYMOUSE_EVENT_RELMOTION:
				//cout << "Mouse moved " << event.value << " on " << (event.item == 0 ? "X" : "Y") << " axis." << endl;

				if (event.item == 0) {
					m_mousePositionDelta.x += event.value;
				}
				else {
					m_mousePositionDelta.y += -event.value; // vertical scroll seems to be inverted
				}
				break;

				/*case MANYMOUSE_EVENT_BUTTON:
				 if (event.value) { // down
				 cout << "Mouse button " << event.item << " down." << endl;
				 }
				 else { // up
				 cout << "Mouse button " << event.item << " up." << endl;
				 }
				 break;*/

			case MANYMOUSE_EVENT_SCROLL:
				if (event.item == 0) {
					//cout << "Mouse scroll: " << (event.value > 0 ? "up" : "down") << endl;
					m_mouseScrollWheelDelta.y += event.value;
				}
				else {
					//cout << "Mouse scroll: " << (event.value > 0 ? "right" : "left") << endl;
					m_mouseScrollWheelDelta.x += event.value;
				}
				break;

			case MANYMOUSE_EVENT_DISCONNECT:
				// TODO: Handle this
				//cout << "Mouse " << event.device << " disconnected." << endl;
				AE_LOG.info("Mouse {} disconnected.", event.device);
				break;

			case MANYMOUSE_EVENT_ABSMOTION:
			case MANYMOUSE_EVENT_BUTTON:
			case MANYMOUSE_EVENT_MAX:
				break;
		}
	}
}

/***************************************************************************************
     MARK:   GLFW Callbacks
 **************************************************************************************/

void InputManager::glfwMouseButtonCallback(GLFWwindow* glfwWindow, int button, int action, int mods) {
	//cout << "glfwMouseButtonCallback()" << endl;
	
	if (action == GLFW_PRESS) {
		inputManager->m_mouseButtonsDown.insert((MouseButton)button);
		
		// if button is in "cleared" it means the client already read it, so don't add it again until
		// we get button up, and then back down again
		if (inputManager->m_mouseButtonsPressedCleared.count((MouseButton)button) == 0) {
			inputManager->m_mouseButtonsPressed.insert((MouseButton)button);
		}
	}
	else if (action == GLFW_RELEASE) {
		inputManager->m_mouseButtonsDown.erase((MouseButton)button);
		inputManager->m_mouseButtonsPressedCleared.erase((MouseButton)button);
	}
}

void InputManager::glfwCursorPositionCallback(GLFWwindow* glfwWindow, double xPos, double yPos) {
	//cout << "glfwCursorPositionCallback()" << endl;
	// ignoring in favor of ManyMouse
}

void InputManager::glfwScrollWheelCallback(GLFWwindow* glfwWindow, double xOffset, double yOffset) {
	//cout << "glfwScrollWheelCallback()" << endl;
	// ignoring in favor of ManyMouse
}

void InputManager::glfwKeyCallback(GLFWwindow* glfwWindow, int key, int scancode, int action, int mods) {
	//cout << "glfwKeyCallback()" << endl;
	
	if (action == GLFW_PRESS) {
		inputManager->m_keysDown.insert((Key)key);
		
		// if key is in "cleared" it means the client already read it, so don't add it again until
		// we get key up, and then back down again
		if (inputManager->m_keysPressedCleared.count((Key)key) == 0) {
			inputManager->m_keysPressed.insert((Key)key);
		}
	}
	else if (action == GLFW_RELEASE) {
		inputManager->m_keysDown.erase((Key)key);
		inputManager->m_keysPressedCleared.erase((Key)key);
	}
}

/***************************************************************************************
     MARK:   Private
 **************************************************************************************/

void InputManager::initManyMouse() {
	const int availableMice = ManyMouse_Init();
	
	if (availableMice < 0) {
		AE_LOG.error("ManyMouse failed to initialize.");
	}
	else if (availableMice == 0) {
		AE_LOG.warn("ManyMouse failed to initialize.");
	}
	else {
		AE_LOG.info("ManyMouse driver: {}", ManyMouse_DriverName());
		for (int m = 0; m<availableMice; ++m) {
			AE_LOG.info("Mouse[{}]: {}", m, ManyMouse_DeviceName(m));
		}
	}
}

void InputManager::quitManyMouse() {
	ManyMouse_Quit();
}

void InputManager::registerGLFWCallbacks(GLFWwindow* glfwWindow) {
	// register GLFW callbacks
	
	glfwSetMouseButtonCallback(glfwWindow, InputManager::glfwMouseButtonCallback);
	//	glfwSetCursorPosCallback(glfwWindow, glfwCursorPositionCallback);
	//	glfwSetScrollCallback(glfwWindow, glfwScrollWheelCallback);
	glfwSetKeyCallback(glfwWindow, InputManager::glfwKeyCallback);
}

void InputManager::unregisterGLFWCallbacks(GLFWwindow* glfwWindow) {

	glfwSetMouseButtonCallback(glfwWindow, NULL);
	glfwSetCursorPosCallback(glfwWindow, NULL);
	glfwSetScrollCallback(glfwWindow, NULL);
	glfwSetKeyCallback(glfwWindow, NULL);
}

Window* InputManager::window() const {
	return m_window;
}

void InputManager::clearMousePositionDelta() {
	m_mousePositionDelta.x = 0.0f;
	m_mousePositionDelta.y = 0.0f;
}

void InputManager::clearMouseScrollWheelDelta() {
	m_mouseScrollWheelDelta.x = 0.0f;
	m_mouseScrollWheelDelta.y = 0.0f;
}

