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
     MARK:   GLFW Callbacks
 **************************************************************************************/

void glfwMouseButtonCallback(GLFWwindow* glfwWindow, int button, int action, int mods) {
	//cout << "glfwMouseButtonCallback()" << endl;
	
	if (action == GLFW_PRESS) {
		inputManager->m_mouseButtonsDown.insert((MouseButton)button);
	}
	else if (action == GLFW_RELEASE) {
		inputManager->m_mouseButtonsDown.erase((MouseButton)button);
	}
}

void glfwCursorPositionCallback(GLFWwindow* glfwWindow, double xPos, double yPos) {
	//cout << "glfwCursorPositionCallback()" << endl;
	// ignoring in favor of ManyMouse
}

void glfwScrollWheelCallback(GLFWwindow* glfwWindow, double xOffset, double yOffset) {
	//cout << "glfwScrollWheelCallback()" << endl;
	// ignoring in favor of ManyMouse
}

void glfwKeyCallback(GLFWwindow* glfwWindow, int key, int scancode, int action, int mods) {
	//cout << "glfwKeyCallback()" << endl;

	if (action == GLFW_PRESS) {
		inputManager->m_keysDown.insert((Key)key);
	}
	else if (action == GLFW_RELEASE) {
		inputManager->m_keysDown.erase((Key)key);
	}
}

/***************************************************************************************
     MARK:   Lifecycle
 **************************************************************************************/

InputManager::InputManager(Window* window):
	m_keysDown(set<Key>()),
	m_mouseButtonsDown(set<MouseButton>()),
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

/* IMPLEMENT */ bool InputManager::keyDown(Key key) {
	
}

/* IMPLEMENT */ bool InputManager::mouseButtonDown(MouseButton button) {
	
}

/* IMPLEMENT */ bool InputManager::stickyKeyDown(Key key) {
	
}

/* IMPLEMENT */ bool InputManager::stickyMouseButtonDown(MouseButton button) {
	
}

set<Key> InputManager::keysDown() {
	auto keysDownCopy = m_keysDown;
	return keysDownCopy;
}

set<MouseButton> InputManager::mouseButtonsDown() {
	auto mosueButtonsDownCopy = m_mouseButtonsDown;
	return mosueButtonsDownCopy;
}

/* IMPLEMENT */ set<Key> stickyKeysDown() {
	
}

/* IMPLEMENT */ set<MouseButton> stickyMouseButtonsDown() {
	
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
	}
	else if (action == GLFW_RELEASE) {
		inputManager->m_mouseButtonsDown.erase((MouseButton)button);
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
	}
	else if (action == GLFW_RELEASE) {
		inputManager->m_keysDown.erase((Key)key);
	}
}

/***************************************************************************************
     MARK:   Private
 **************************************************************************************/

void InputManager::initManyMouse() {
	const int availableMice = ManyMouse_Init();
	
	if (availableMice < 0) {
		//cerr << "ManyMouse failed to initialize!" << endl;
		AE_LOG.error("ManyMouse failed to initialize.");
	}
	else if (availableMice == 0) {
		//cerr << "No mice detected!" << endl;
		AE_LOG.warn("ManyMouse failed to initialize.");
	}
	else {
		//cout << "ManyMouse driver: " << ManyMouse_DriverName() << endl;
		AE_LOG.info("ManyMouse driver: {}", ManyMouse_DriverName());
		for (int m = 0; m<availableMice; ++m) {
			//cout << "Mouse " << m << ": " << ManyMouse_DeviceName(m) << endl;
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

