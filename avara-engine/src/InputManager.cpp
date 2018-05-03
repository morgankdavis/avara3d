//
//  InputManager.cpp
//	avara-engine
//
//  Created by Morgan Davis on 10/9/17.
//  Copyright © 2017 Morgan K Davis. All rights reserved.
//

#ifdef DESKTOP


#include "InputManager.h"

#include <iostream>

#include <GLFW/glfw3.h>
#include "manymouse/manymouse.h"

#include "Exception.h"
#include "Logger.h"
#include "Window.h"


using namespace ae;
using namespace std;
using namespace glm;


#define FLIP_MOUSE_VERTICAL		true
#define FLIP_MOUSE_HORIZONTAL	false


/***************************************************************************************
     Globals
 ***************************************************************************************/

InputManager* inputManager;

/***************************************************************************************
     Lifecycle
 ***************************************************************************************/

InputManager::InputManager(Window* window):
	m_keysDown(set<KEY>()),
	m_mouseButtonsDown(set<MOUSE_BUTTON>()),
	m_keysPressed(set<KEY>()),
	m_keysPressedCleared(set<KEY>()),
	m_mouseButtonsPressed(set<MOUSE_BUTTON>()),
	m_mouseButtonsPressedCleared(set<MOUSE_BUTTON>()),
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
     Public
 ***************************************************************************************/

bool InputManager::keyDown(KEY key) {
	return m_keysDown.count(key);
}

bool InputManager::mouseButtonDown(MOUSE_BUTTON button) {
	return m_mouseButtonsDown.count(button);
}

bool InputManager::keyPressed(KEY key) {
	bool pressed = inputManager->m_keysPressed.count(key);
	if (pressed) {
		inputManager->m_keysPressed.erase(key);
		inputManager->m_keysPressedCleared.insert(key);
	}
	return pressed;
}

bool InputManager::mouseButtonPressed(MOUSE_BUTTON button) {
	bool pressed = inputManager->m_mouseButtonsPressed.count(button);
	if (pressed) {
		inputManager->m_mouseButtonsPressed.erase(button);
		inputManager->m_mouseButtonsPressedCleared.insert(button);
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
//		cout << "DELTA: " << m_mousePositionDelta.x << ", " << m_mousePositionDelta.y << endl;
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

/***************************************************************************************
     Internal
 ***************************************************************************************/

void InputManager::update() {
	
//	cout << "update()" << endl;

	static ManyMouseEvent event;
	
//	vec2 accumMousePosition = {0, 0};
//	int xEvents = 0;
//	int yEvents = 0;

	while (ManyMouse_PollEvent(&event)) {
		switch(event.type) {
				
			case MANYMOUSE_EVENT_RELMOTION:
				//cout << "pollCount: " << pollCount << endl;
				//cout << "Mouse moved " << event.value << " on " << (event.item == 0 ? "X" : "Y") << " axis." << endl;

				//AE_LOG->debug("min: {}, max: {}", event.minval, event.maxval);
				
				// we used to ACCUMULATE the delta.
				// it turns out manymouse is doing that for us, and the most recent even has the accumulation...
				// update: actually that doesn't look to be the case... wtf??
				if (event.item == 0) {
//					++xEvents;
//					accumMousePosition.x += (FLIP_MOUSE_HORIZONTAL ? -event.value : event.value);
//					cout << "X value: " << event.value << endl;
					m_mousePositionDelta.x = (FLIP_MOUSE_HORIZONTAL ? -event.value : event.value);
				}
				else {
//					++yEvents;
//					accumMousePosition.y += (FLIP_MOUSE_VERTICAL ? -event.value : event.value);
//					cout << "Y value: " << event.value << endl;
					m_mousePositionDelta.y = (FLIP_MOUSE_VERTICAL ? -event.value : event.value);
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
				AE_LOG->warn("Mouse {} disconnected.", event.device);
				break;

			case MANYMOUSE_EVENT_ABSMOTION:
			case MANYMOUSE_EVENT_BUTTON:
			case MANYMOUSE_EVENT_MAX:
				break;
		}
	}
	
//	m_mousePositionDelta.x = accumMousePosition.x / (float)xEvents;
//	m_mousePositionDelta.y = accumMousePosition.y / (float)yEvents;
}

/***************************************************************************************
     GLFW Callbacks
 ***************************************************************************************/

void InputManager::glfwMouseButtonCallback(GLFWwindow* glfwWindow, int button, int action, int mods) {
	//cout << "glfwMouseButtonCallback()" << endl;
	
//	if (action == GLFW_PRESS) {
//		inputManager->m_mouseButtonsDown.insert(MOUSE_BUTTON_FROM_RAW(button));
//		
//		// if button is in "cleared" it means the client already read it, so don't add it again until
//		// we get button up, and then back down again
//		if (inputManager->m_mouseButtonsPressedCleared.count(MOUSE_BUTTON_FROM_RAW(button)) == 0) {
//			inputManager->m_mouseButtonsPressed.insert(MOUSE_BUTTON_FROM_RAW(button));
//		}
//	}
//	else if (action == GLFW_RELEASE) {
//		inputManager->m_mouseButtonsDown.erase(MOUSE_BUTTON_FROM_RAW(button));
//		inputManager->m_mouseButtonsPressedCleared.erase(MOUSE_BUTTON_FROM_RAW(button));
//	}
	
	if (action == GLFW_PRESS) {
		inputManager->m_mouseButtonsDown.insert((MOUSE_BUTTON)button);
		
		// if button is in "cleared" it means the client already read it, so don't add it again until
		// we get button up, and then back down again
		if (inputManager->m_mouseButtonsPressedCleared.count((MOUSE_BUTTON)button) == 0) {
			inputManager->m_mouseButtonsPressed.insert((MOUSE_BUTTON)button);
		}
	}
	else if (action == GLFW_RELEASE) {
		inputManager->m_mouseButtonsDown.erase((MOUSE_BUTTON)button);
		inputManager->m_mouseButtonsPressedCleared.erase((MOUSE_BUTTON)button);
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
		inputManager->m_keysDown.insert((KEY)key);
		
		// if key is in "cleared" it means the client already read it, so don't add it again until
		// we get key up, and then back down again
		if (inputManager->m_keysPressedCleared.count((KEY)key) == 0) {
			inputManager->m_keysPressed.insert((KEY)key);
		}
	}
	else if (action == GLFW_RELEASE) {
		inputManager->m_keysDown.erase((KEY)key);
		inputManager->m_keysPressedCleared.erase((KEY)key);
	}
}

/***************************************************************************************
     Private
 ***************************************************************************************/

void InputManager::initManyMouse() {
	int availableMice = ManyMouse_Init();
	
	if (availableMice < 0) {
		//AE_LOG->error("ManyMouse failed to initialize.");
		throw Exception("ManyMouse failed to initialize.");
	}
	else if (availableMice == 0) {
		AE_LOG->warn("ManyMouse failed to initialize.");
	}
	else {
		AE_LOG->info("ManyMouse driver: {}", ManyMouse_DriverName());
		for (int m = 0; m<availableMice; ++m) {
			AE_LOG->info("Mouse[{}]: {}", m, ManyMouse_DeviceName(m));
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

#endif // DESKTOP
