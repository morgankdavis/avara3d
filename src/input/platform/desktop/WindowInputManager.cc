//
//  WindowInputManager.cc
//	avara3d
//
//  Created by Morgan Davis on 5/6/18.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#ifdef DESKTOP


#include "a3d/input/platform/desktop/WindowInputManager.h"

#include "GLFW/glfw3.h"
#include "manymouse.h"

#include "a3d/diagnostic/exception/Exception.h"
#include "a3d/diagnostic/logging/Logger.h"
#include "a3d/rendering/VisualWorld.h"
#include "a3d/rendering/context/platform/desktop/Window.h"
#include "a3d/scene/Scene.h"


using namespace a3d;
using namespace std;
using namespace glm;


// TODO: get rid of these/make non constant
constexpr bool	FLIP_MOUSE_VERTICAL =		true;
constexpr bool	FLIP_MOUSE_HORIZONTAL =		false;


/*********************************************************************************************
	Static Prototypes
 *********************************************************************************************/

static WindowInputManager* InputManagerFromGLFWWindow(GLFWwindow* glfwWindow);

/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

WindowInputManager::WindowInputManager(Window* window):
	InputManager{},
	_usingManyMouse{false},
	_window{window} {

		registerGLFWCallbacks(window->glfwWindow());
	initMouseInput();
}

WindowInputManager::~WindowInputManager() {
	A3D_LOG_D("Destroying WindowInputManager {:p}", static_cast<void*>(this));

	quitManyMouse();
	if (_window) {
		unregisterGLFWCallbacks(_window->glfwWindow());
	}
}

/*********************************************************************************************
	Internal
 *********************************************************************************************/

void WindowInputManager::update() {

	if (_usingManyMouse) {
		static ManyMouseEvent event;

		while (ManyMouse_PollEvent(&event)) {
			switch (event.type) {

				case MANYMOUSE_EVENT_RELMOTION:

					if (event.item == 0) {
						_mousePositionDelta.x = (FLIP_MOUSE_HORIZONTAL ? -event.value : event.value);
					}
					else {
						_mousePositionDelta.y = (FLIP_MOUSE_VERTICAL ? -event.value : event.value);
					}
					break;

				case MANYMOUSE_EVENT_SCROLL:
					if (event.item == 0) {
						_mouseScrollWheelDelta.y += event.value;
					}
					else {
						_mouseScrollWheelDelta.x += event.value;
					}
					break;

				case MANYMOUSE_EVENT_DISCONNECT:
					A3D_LOG_W("Mouse {} disconnected.", event.device);
					break;

				case MANYMOUSE_EVENT_ABSMOTION:
				case MANYMOUSE_EVENT_BUTTON:
				case MANYMOUSE_EVENT_MAX:
					break;
			}
		}
	}

	// needed for non-mouse events (keyboard, not joystrick, OTHER NON-INPUT??)
	// https://www.glfw.org/docs/latest/group__window.html#ga37bd57223967b4211d60ca1a0bf3c832
	_window->pollInput();
}

/*********************************************************************************************
	GLFW Callbacks
 *********************************************************************************************/

void WindowInputManager::GLFWMouseButtonCallback(GLFWwindow* glfwWindow,
												 int button,
												 int action,
												 int mods) {

	auto inputManager = InputManagerFromGLFWWindow(glfwWindow);
	
	auto a3dButton = static_cast<MouseButton>(button);
	
	if (action == GLFW_PRESS) {
		inputManager->_mouseButtonsDown.insert(a3dButton);
		
		// if button is in "cleared" it means the client already read it, so don't add it again until
		// we get button up, and then back down again
		if (inputManager->_mouseButtonsPressedCleared.count(a3dButton) == 0) {
			inputManager->_mouseButtonsPressed.insert(a3dButton);
		}
	}
	else if (action == GLFW_RELEASE) {
		inputManager->_mouseButtonsDown.erase(a3dButton);
		inputManager->_mouseButtonsPressedCleared.erase(a3dButton);
	}
}

void WindowInputManager::GLFWCursorPositionCallback(GLFWwindow* glfwWindow,
													double xPos,
													double yPos) {

	auto inputManager = InputManagerFromGLFWWindow(glfwWindow);

	// keep "lastPos" outside cursorCaptured() check to keep it from
	// jumping when re-capturing the cursor
	static double lastXPos = xPos;
	static double lastYPos = yPos;

	if (auto window = inputManager->_window; window) {
		if (window->cursorCaptured()) {

			double xDelta = lastXPos - xPos;
			double yDelta = lastYPos - yPos;

			//A3D_LOG_D("xDelta: {}, yDelta {}", xDelta, yDelta);

			inputManager->_mousePositionDelta.x -= xDelta;
			inputManager->_mousePositionDelta.y += yDelta;
		}
	}

	lastXPos = xPos;
	lastYPos = yPos;
}

void WindowInputManager::GLFWScrollWheelCallback(GLFWwindow* glfwWindow,
												 double xOffset,
												 double yOffset) {

	auto inputManager = InputManagerFromGLFWWindow(glfwWindow);

	inputManager->_mouseScrollWheelDelta.x += (float)xOffset;
	inputManager->_mouseScrollWheelDelta.y += (float)yOffset;
}

void WindowInputManager::GLFWKeyCallback(GLFWwindow* glfwWindow,
										 int key,
										 int scancode,
										 int action,
										 int mods) {

	auto inputManager = InputManagerFromGLFWWindow(glfwWindow);
	
	if (action == GLFW_PRESS) {
		inputManager->_keysDown.insert(static_cast<Key>(key));
		
		// if key is in "cleared" it means the client already read it, so don't add it again until
		// we get key up, and then back down again
		if (inputManager->_keysPressedCleared.count(static_cast<Key>(key)) == 0) {
			inputManager->_keysPressed.insert(static_cast<Key>(key));
		}
	}
	else if (action == GLFW_RELEASE) {
		inputManager->_keysDown.erase(static_cast<Key>(key));
		inputManager->_keysPressedCleared.erase(static_cast<Key>(key));
	}
}

/*********************************************************************************************
	Private
 *********************************************************************************************/

void WindowInputManager::initMouseInput() {

	if (glfwRawMouseMotionSupported()) {
		A3D_LOG_I("Using GLFW raw mouse input.");
		glfwSetInputMode(_window->glfwWindow(), GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
		glfwSetCursorPosCallback(_window->glfwWindow(),
								 WindowInputManager::GLFWCursorPositionCallback);
		_usingManyMouse = false;
	}
	else {
		A3D_LOG_W("GLFW raw mouse input unavailable.  Using ManyMouse.");
		initManyMouse();
		_usingManyMouse = true;
	}
}

void WindowInputManager::initManyMouse() {
	A3D_LOG_T("");

	// TODO: must be changed to support multiple windows
	auto availableMice = ManyMouse_Init();

	if (availableMice < 0) {
		throw Exception("Failed to initialize ManyMouse.");
	}
	else if (availableMice == 0) {
		A3D_LOG_W("No available mice.");
	}
	else {
		A3D_LOG_I("ManyMouse driver: {}", ManyMouse_DriverName());
		for (int m = 0; m<availableMice; ++m) {
			A3D_LOG_I("Mouse[{}]: {}", m, ManyMouse_DeviceName(m));
		}
	}
}

void WindowInputManager::quitManyMouse() {
	ManyMouse_Quit();
}

void WindowInputManager::registerGLFWCallbacks(GLFWwindow* glfwWindow) {

	glfwSetMouseButtonCallback(glfwWindow, WindowInputManager::GLFWMouseButtonCallback);
	// glfwSetCursorPosCallback -> in initMouseMotionInput()
	glfwSetScrollCallback(glfwWindow, WindowInputManager::GLFWScrollWheelCallback);
	glfwSetKeyCallback(glfwWindow, WindowInputManager::GLFWKeyCallback);

	// TODO: probably re-factor key callback creation code
}

void WindowInputManager::unregisterGLFWCallbacks(GLFWwindow* glfwWindow) {
	
	glfwSetMouseButtonCallback(glfwWindow, nullptr);
	glfwSetCursorPosCallback(glfwWindow, nullptr);
	glfwSetScrollCallback(glfwWindow, nullptr);
	glfwSetKeyCallback(glfwWindow, nullptr);
}

/*********************************************************************************************
	Static
 *********************************************************************************************/

WindowInputManager* InputManagerFromGLFWWindow(GLFWwindow* glfwWindow) {

	auto window = (Window*)glfwGetWindowUserPointer(glfwWindow);
	return dynamic_cast<WindowInputManager*>(window->visualWorld()->scene()->inputManager());
}


#endif // DESKTOP
