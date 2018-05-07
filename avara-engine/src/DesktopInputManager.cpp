//
//  DesktopInputManager.cpp
//	avara-engine
//
//  Created by Morgan Davis on 5/6/18.
//  Copyright © 2018 Morgan K Davis. All rights reserved.
//

#ifdef DESKTOP


#include "DesktopInputManager.h"

#include <iostream>

#include <GLFW/glfw3.h>
#include <manymouse/manymouse.h>

#include "Exception.h"
#include "Logger.h"
#include "Window.h"


using namespace ae;
using namespace std;
using namespace glm;


#define FLIP_MOUSE_VERTICAL		true
#define FLIP_MOUSE_HORIZONTAL	false


/***************************************************************************************
     Lifecycle
 ***************************************************************************************/

DesktopInputManager::DesktopInputManager(shared_ptr<Window> window):
	InputManager(),
	m_window(window) {

		registerGLFWCallbacks(window->glfwWindow());
		initManyMouse();
}

DesktopInputManager::~DesktopInputManager() {
	quitManyMouse();
	unregisterGLFWCallbacks(m_window.lock()->glfwWindow());
}

/***************************************************************************************
     Internal
 ***************************************************************************************/

void DesktopInputManager::update() {
	
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
}

/***************************************************************************************
     GLFW Callbacks
 ***************************************************************************************/

void DesktopInputManager::glfwMouseButtonCallback(GLFWwindow* glfwWindow, int button, int action, int mods) {
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
	
	Window* window = (Window*)glfwGetWindowUserPointer(glfwWindow);
	auto inputManager = static_pointer_cast<DesktopInputManager>(window->inputManager());
	
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

void DesktopInputManager::glfwCursorPositionCallback(GLFWwindow* glfwWindow, double xPos, double yPos) {
	//cout << "glfwCursorPositionCallback()" << endl;
	// ignoring in favor of ManyMouse
}

void DesktopInputManager::glfwScrollWheelCallback(GLFWwindow* glfwWindow, double xOffset, double yOffset) {
	//cout << "glfwScrollWheelCallback()" << endl;
	// ignoring in favor of ManyMouse
}

void DesktopInputManager::glfwKeyCallback(GLFWwindow* glfwWindow, int key, int scancode, int action, int mods) {
	//cout << "glfwKeyCallback()" << endl;
	
	Window* window = (Window*)glfwGetWindowUserPointer(glfwWindow);
	auto inputManager = static_pointer_cast<DesktopInputManager>(window->inputManager());
	
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

void DesktopInputManager::initManyMouse() {
	int availableMice = ManyMouse_Init();
	
	if (availableMice < 0) {
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

void DesktopInputManager::quitManyMouse() {
	ManyMouse_Quit();
}

void DesktopInputManager::registerGLFWCallbacks(GLFWwindow* glfwWindow) {
	// register GLFW callbacks
	
	glfwSetMouseButtonCallback(glfwWindow, DesktopInputManager::glfwMouseButtonCallback);
	//	glfwSetCursorPosCallback(glfwWindow, glfwCursorPositionCallback);
	//	glfwSetScrollCallback(glfwWindow, glfwScrollWheelCallback);
	glfwSetKeyCallback(glfwWindow, DesktopInputManager::glfwKeyCallback);
}

void DesktopInputManager::unregisterGLFWCallbacks(GLFWwindow* glfwWindow) {

	glfwSetMouseButtonCallback(glfwWindow, NULL);
	glfwSetCursorPosCallback(glfwWindow, NULL);
	glfwSetScrollCallback(glfwWindow, NULL);
	glfwSetKeyCallback(glfwWindow, NULL);
}

#endif // DESKTOP
