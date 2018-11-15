//
//  WindowInputManager.cc
//	avara-engine
//
//  Created by Morgan Davis on 5/6/18.
//  Copyright © 2018 Morgan K Davis. All rights reserved.
//

#ifdef DESKTOP


#include "WindowInputManager.h"

#include <iostream>

#include <GLFW/glfw3.h>
#include <manymouse.h>

#include "Exception.h"
#include "Logger.h"
#include "Window.h"


using namespace ae;
using namespace std;
using namespace glm;


constexpr bool	FLIP_MOUSE_VERTICAL =		true;
constexpr bool	FLIP_MOUSE_HORIZONTAL =		false;


/*********************************************************************************************
     Lifecycle
 *********************************************************************************************/

WindowInputManager::WindowInputManager(shared_ptr<Window> window):
	InputManager(),
	m_window(window) {

		registerGLFWCallbacks(window->glfwWindow());
		initManyMouse();
}

WindowInputManager::~WindowInputManager() {
	quitManyMouse();
	auto window = m_window.lock();
	if (window) {
		unregisterGLFWCallbacks(window->glfwWindow());
	}
}

/*********************************************************************************************
     Internal
 *********************************************************************************************/

void WindowInputManager::update() {
	
	static ManyMouseEvent event;
	
	while (ManyMouse_PollEvent(&event)) {
		switch(event.type) {
				
			case MANYMOUSE_EVENT_RELMOTION:

				if (event.item == 0) {
					m_mousePositionDelta.x = (FLIP_MOUSE_HORIZONTAL ? -event.value : event.value);
				}
				else {
					m_mousePositionDelta.y = (FLIP_MOUSE_VERTICAL ? -event.value : event.value);
				}
				break;

			case MANYMOUSE_EVENT_SCROLL:
				if (event.item == 0) {
					m_mouseScrollWheelDelta.y += event.value;
				}
				else {
					m_mouseScrollWheelDelta.x += event.value;
				}
				break;

			case MANYMOUSE_EVENT_DISCONNECT:
				AE_LOG_W("Mouse {} disconnected.", event.device);
				break;

			case MANYMOUSE_EVENT_ABSMOTION:
			case MANYMOUSE_EVENT_BUTTON:
			case MANYMOUSE_EVENT_MAX:
				break;
		}
	}
}

/*********************************************************************************************
     GLFW Callbacks
 *********************************************************************************************/

void WindowInputManager::glfwMouseButtonCallback(GLFWwindow* glfwWindow, int button, int action, int mods) {

	Window* window = (Window*)glfwGetWindowUserPointer(glfwWindow);
	auto inputManager = static_pointer_cast<WindowInputManager>(window->inputManager());
	
	auto aeButton = static_cast<MOUSE_BUTTON>(button);
	
	if (action == GLFW_PRESS) {
		inputManager->m_mouseButtonsDown.insert(aeButton);
		
		// if button is in "cleared" it means the client already read it, so don't add it again until
		// we get button up, and then back down again
		if (inputManager->m_mouseButtonsPressedCleared.count(aeButton) == 0) {
			inputManager->m_mouseButtonsPressed.insert(aeButton);
		}
	}
	else if (action == GLFW_RELEASE) {
		inputManager->m_mouseButtonsDown.erase(aeButton);
		inputManager->m_mouseButtonsPressedCleared.erase(aeButton);
	}
}

void WindowInputManager::glfwCursorPositionCallback(GLFWwindow* glfwWindow, double xPos, double yPos) {
	// ignoring in favor of ManyMouse
}

void WindowInputManager::glfwScrollWheelCallback(GLFWwindow* glfwWindow, double xOffset, double yOffset) {
	// ignoring in favor of ManyMouse
}

void WindowInputManager::glfwKeyCallback(GLFWwindow* glfwWindow, int key, int scancode, int action, int mods) {
	//cout << "glfwKeyCallback()" << endl;
	
	Window* window = (Window*)glfwGetWindowUserPointer(glfwWindow);
	auto inputManager = static_pointer_cast<WindowInputManager>(window->inputManager());
	
	if (action == GLFW_PRESS) {
		inputManager->m_keysDown.insert(static_cast<KEY>(key));
		
		// if key is in "cleared" it means the client already read it, so don't add it again until
		// we get key up, and then back down again
		if (inputManager->m_keysPressedCleared.count(static_cast<KEY>(key)) == 0) {
			inputManager->m_keysPressed.insert(static_cast<KEY>(key));
		}
	}
	else if (action == GLFW_RELEASE) {
		inputManager->m_keysDown.erase(static_cast<KEY>(key));
		inputManager->m_keysPressedCleared.erase(static_cast<KEY>(key));
	}
}

/*********************************************************************************************
     Private
 *********************************************************************************************/

void WindowInputManager::initManyMouse() {
	int availableMice = ManyMouse_Init();
	
	if (availableMice < 0) {
		throw Exception("ManyMouse failed to initialize.");
	}
	else if (availableMice == 0) {
		AE_LOG_W("ManyMouse failed to initialize.");
	}
	else {
		AE_LOG_I("ManyMouse driver: {}", ManyMouse_DriverName());
		for (int m = 0; m<availableMice; ++m) {
			AE_LOG_I("Mouse[{}]: {}", m, ManyMouse_DeviceName(m));
		}
	}
}

void WindowInputManager::quitManyMouse() {
	ManyMouse_Quit();
}

void WindowInputManager::registerGLFWCallbacks(GLFWwindow* glfwWindow) {
	glfwSetMouseButtonCallback(glfwWindow, WindowInputManager::glfwMouseButtonCallback);
	glfwSetKeyCallback(glfwWindow, WindowInputManager::glfwKeyCallback);
}

void WindowInputManager::unregisterGLFWCallbacks(GLFWwindow* glfwWindow) {

	glfwSetMouseButtonCallback(glfwWindow, NULL);
	glfwSetCursorPosCallback(glfwWindow, NULL);
	glfwSetScrollCallback(glfwWindow, NULL);
	glfwSetKeyCallback(glfwWindow, NULL);
}

#endif // DESKTOP
