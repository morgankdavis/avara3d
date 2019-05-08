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


// TODO: get rid of these/make non constant
constexpr bool	FLIP_MOUSE_VERTICAL =		true;
constexpr bool	FLIP_MOUSE_HORIZONTAL =		false;


/*********************************************************************************************
     Lifecycle
 *********************************************************************************************/

WindowInputManager::WindowInputManager(shared_ptr<Window> window):
	InputManager(),
	m_window(window) {

		registerGLFWCallbacks(window->glfwWindow());
		initMouseMotionInput();
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

	if (m_usingManyMouse) {
		static ManyMouseEvent event;

		while (ManyMouse_PollEvent(&event)) {
			switch (event.type) {

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
}

/*********************************************************************************************
     GLFW Callbacks
 *********************************************************************************************/

void WindowInputManager::GLFWMouseButtonCallback(GLFWwindow *glfwWindow, int button, int action, int mods) {

	auto inputManager = InputManagerFromGLFWWindow(glfwWindow);
	
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

void WindowInputManager::GLFWCursorPositionCallback(GLFWwindow *glfwWindow, double xPos, double yPos) {
	// ignoring in favor of ManyMouse

	//AE_LOG_D("GLFWCursorPositionCallback(): ({}, {})", xPos, yPos);

	auto inputManager = InputManagerFromGLFWWindow(glfwWindow);

	static double lastXPos = xPos;
	static double lastYPos = yPos;

	double xDelta = lastXPos - xPos;
	double yDelta = lastYPos - yPos;

	//AE_LOG_D("xDelta: ({}, yDelta {})", xDelta, yDelta);

	inputManager->m_mousePositionDelta.x -= xDelta;
	inputManager->m_mousePositionDelta.y += yDelta;

	lastXPos = xPos;
	lastYPos = yPos;
}

void WindowInputManager::GLFWScrollWheelCallback(GLFWwindow *glfwWindow, double xOffset, double yOffset) {
	// ignoring in favor of ManyMouse

	// TODO: use GLFW if using GLFW for raw mouse
}

void WindowInputManager::GLFWKeyCallback(GLFWwindow *glfwWindow, int key, int scancode, int action, int mods) {
	//cout << "GLFWKeyCallback()" << endl;

	auto inputManager = InputManagerFromGLFWWindow(glfwWindow);
	
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

shared_ptr<WindowInputManager> WindowInputManager::InputManagerFromGLFWWindow(GLFWwindow* glfwWindow) {

	Window* window = (Window*)glfwGetWindowUserPointer(glfwWindow);
	return static_pointer_cast<WindowInputManager>(window->inputManager());
}

/*********************************************************************************************
     Private
 *********************************************************************************************/

void WindowInputManager::initMouseMotionInput() {

	if (glfwRawMouseMotionSupported()) {
		AE_LOG_I("Using GLFW raw mouse input.");
		glfwSetInputMode(m_window.lock()->glfwWindow(), GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
		glfwSetCursorPosCallback(m_window.lock()->glfwWindow(), WindowInputManager::GLFWCursorPositionCallback);
		m_usingManyMouse = false;
	}
	else {
		AE_LOG_E("GLFW raw mouse input unavailable. Using ManyMouse raw mouse input.");
		initManyMouse();
		m_usingManyMouse = true;
	}
}

void WindowInputManager::initManyMouse() {

	// TODO: must be changed to support multiple windows
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
	glfwSetMouseButtonCallback(glfwWindow, WindowInputManager::GLFWMouseButtonCallback);
	glfwSetKeyCallback(glfwWindow, WindowInputManager::GLFWKeyCallback);

	// TODO: probably re-factor key callback creation code
}

void WindowInputManager::unregisterGLFWCallbacks(GLFWwindow* glfwWindow) {
	
	glfwSetMouseButtonCallback(glfwWindow, NULL);
	glfwSetCursorPosCallback(glfwWindow, NULL);
	glfwSetScrollCallback(glfwWindow, NULL);
	glfwSetKeyCallback(glfwWindow, NULL);

	// TODO: refactor
	glfwSetCursorPosCallback(glfwWindow, NULL);
}

#endif // DESKTOP
