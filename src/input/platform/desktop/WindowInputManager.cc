//
//  WindowInputManager.cc
//	avara-engine
//
//  Created by Morgan Davis on 5/6/18.
//  Copyright © 2018 Morgan K Davis. All rights reserved.
//

#ifdef DESKTOP


#include "ae/input/platform/desktop/WindowInputManager.h"

#include "GLFW/glfw3.h"

//#include "ae/diagnostic/exceptions/Exception.h"
#include "ae/diagnostic/logging/Logger.h"
#include "ae/rendering/VisualWorld.h"
#include "ae/rendering/context/platform/desktop/Window.h"
#include "ae/scene/Scene.h"


using namespace ae;
using namespace std;
using namespace glm;


// TODO: get rid of these/make non constant
constexpr bool	FLIP_MOUSE_VERTICAL =		true;
constexpr bool	FLIP_MOUSE_HORIZONTAL =		false;


/*********************************************************************************************
	Static Prototypes
 *********************************************************************************************/

static std::shared_ptr<WindowInputManager> InputManagerFromGLFWWindow(GLFWwindow* glfwWindow);

/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

WindowInputManager::WindowInputManager(shared_ptr<Window> window):
	InputManager(),
//	_usingManyMouse(false),
	_window(window) {

		registerGLFWCallbacks(window->glfwWindow());
		initMouseMotionInput();
}

WindowInputManager::~WindowInputManager() {
	AE_LOG_D("Destroying WindowInputManager {:p}", static_cast<void*>(this));

//	quitManyMouse();
	if (auto window = _window.lock()) {
		unregisterGLFWCallbacks(window->glfwWindow());
	}
}

/*********************************************************************************************
	Internal
 *********************************************************************************************/

void WindowInputManager::update() {

//	if (_usingManyMouse) {
//		static ManyMouseEvent event;
//
//		while (ManyMouse_PollEvent(&event)) {
//			switch (event.type) {
//
//				case MANYMOUSE_EVENT_RELMOTION:
//
//					if (event.item == 0) {
//						_mousePositionDelta.x = (FLIP_MOUSE_HORIZONTAL ? -event.value : event.value);
//					}
//					else {
//						_mousePositionDelta.y = (FLIP_MOUSE_VERTICAL ? -event.value : event.value);
//					}
//					break;
//
//				case MANYMOUSE_EVENT_SCROLL:
//					if (event.item == 0) {
//						_mouseScrollWheelDelta.y += event.value;
//					}
//					else {
//						_mouseScrollWheelDelta.x += event.value;
//					}
//					break;
//
//				case MANYMOUSE_EVENT_DISCONNECT:
//					AE_LOG_W("Mouse {} disconnected.", event.device);
//					break;
//
//				case MANYMOUSE_EVENT_ABSMOTION:
//				case MANYMOUSE_EVENT_BUTTON:
//				case MANYMOUSE_EVENT_MAX:
//					break;
//			}
//		}
//	}
//	else {
		_window.lock()->pollInput();
//	}
}

/*********************************************************************************************
	GLFW Callbacks
 *********************************************************************************************/

void WindowInputManager::GLFWMouseButtonCallback(GLFWwindow* glfwWindow,
												 int button,
												 int action,
												 int mods) {

	auto inputManager = InputManagerFromGLFWWindow(glfwWindow);
	
	auto aeButton = static_cast<MOUSE_BUTTON>(button);
	
	if (action == GLFW_PRESS) {
		inputManager->_mouseButtonsDown.insert(aeButton);
		
		// if button is in "cleared" it means the client already read it, so don't add it again until
		// we get button up, and then back down again
		if (inputManager->_mouseButtonsPressedCleared.count(aeButton) == 0) {
			inputManager->_mouseButtonsPressed.insert(aeButton);
		}
	}
	else if (action == GLFW_RELEASE) {
		inputManager->_mouseButtonsDown.erase(aeButton);
		inputManager->_mouseButtonsPressedCleared.erase(aeButton);
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

	if (auto window = inputManager->_window.lock()) {
		if (window->cursorCaptured()) {

			double xDelta = lastXPos - xPos;
			double yDelta = lastYPos - yPos;

			//AE_LOG_D("xDelta: {}, yDelta {}", xDelta, yDelta);

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
		inputManager->_keysDown.insert(static_cast<KEY>(key));
		
		// if key is in "cleared" it means the client already read it, so don't add it again until
		// we get key up, and then back down again
		if (inputManager->_keysPressedCleared.count(static_cast<KEY>(key)) == 0) {
			inputManager->_keysPressed.insert(static_cast<KEY>(key));
		}
	}
	else if (action == GLFW_RELEASE) {
		inputManager->_keysDown.erase(static_cast<KEY>(key));
		inputManager->_keysPressedCleared.erase(static_cast<KEY>(key));
	}
}

/*********************************************************************************************
	Private
 *********************************************************************************************/

void WindowInputManager::initMouseMotionInput() {

	if (glfwRawMouseMotionSupported()) {
		AE_LOG_I("Using GLFW raw mouse input.");
		glfwSetInputMode(_window.lock()->glfwWindow(), GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
		glfwSetCursorPosCallback(_window.lock()->glfwWindow(),
								 WindowInputManager::GLFWCursorPositionCallback);
//		_usingManyMouse = false;
	}
	else {
		AE_LOG_E("GLFW raw mouse input unavailable!");
//		initManyMouse();
//		_usingManyMouse = true;
	}
}

//void WindowInputManager::initManyMouse() {
//
//	// TODO: must be changed to support multiple windows
//	int availableMice = ManyMouse_Init();
//
//	if (availableMice < 0) {
//		throw Exception("ManyMouse failed to initialize.");
//	}
//	else if (availableMice == 0) {
//		AE_LOG_W("ManyMouse failed to initialize.");
//	}
//	else {
//		AE_LOG_I("ManyMouse driver: {}", ManyMouse_DriverName());
//		for (int m = 0; m<availableMice; ++m) {
//			AE_LOG_I("Mouse[{}]: {}", m, ManyMouse_DeviceName(m));
//		}
//	}
//}

//void WindowInputManager::quitManyMouse() {
//	ManyMouse_Quit();
//}

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

shared_ptr<WindowInputManager> WindowInputManager::InputManagerFromGLFWWindow(GLFWwindow* glfwWindow) {

	Window* window = (Window*)glfwGetWindowUserPointer(glfwWindow);
	return static_pointer_cast<WindowInputManager>(window->visualWorld()->scene()->inputManager());
}


#endif // DESKTOP
