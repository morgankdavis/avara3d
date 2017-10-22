//
//  InputManager.cpp
//	avara-engine
//
//  Created by Morgan Davis on 10/9/17.
//  Copyright © 2017 Morgan K Davis. All rights reserved.
//

#include "InputManager.h"

#include <iostream>

//#include <gainput/gainput.h>
#include "manymouse/manymouse.h"

#include "Globals.h"
#include "Window.h"


using namespace ae;
using namespace std;
using namespace glm;


/***************************************************************************************
     MARK:   Globals
 **************************************************************************************/

// TODO: this is going to be a problem when we start making multiple instances of InputManager
// this may help/GLFW callbacks:
// http://www.newty.de/fpt/callback.html
InputManager *inputManager;
//gainput::InputManager*	gainputInputManager;
//gainput::DeviceId gainputMouseId;
//gainput::InputMap* gainputInputMap;
//
//enum GainputMouseAxis
//{
//	MouseX,
//	MouseY
//};

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
		
		registerGLFWCallbacks();
		initManyMouse();
//		initGainput();
}

InputManager::~InputManager() {
	quitManyMouse();
	unregisterGLFWCallbacks();
//	if (gainputInputManager) delete gainputInputManager;
//	if (gainputInputMap) delete gainputInputMap;
}

/***************************************************************************************
     MARK:   Public
 **************************************************************************************/

set<Key> InputManager::keysDown() {
	auto keysDownCopy = m_keysDown;
	clearKeysDown();
	return keysDownCopy;
}

set<MouseButton> InputManager::mouseButtonsDown() {
	auto mosueButtonsDownCopy = m_mouseButtonsDown;
	clearMouseDown();
	return mosueButtonsDownCopy;
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
//
//	gainputInputManager->Update(deltaSeconds);
//
////	if (gainputInputMap->GetFloatDelta(MouseX) != 0.0f || gainputInputMap->GetFloatDelta(MouseY) != 0.0f)
////	{
//		std::cout << "Mouse: " << gainputInputMap->GetFloat(MouseX) << ", " << gainputInputMap->GetFloat(MouseY) << std::endl;
////	}
//
//
//
////	m_mousePositionDelta.x += gainputInputMap->GetFloatDelta(MouseX);
////	m_mousePositionDelta.y += gainputInputMap->GetFloatDelta(MouseY);
//
	
	static ManyMouseEvent event;

	while (ManyMouse_PollEvent(&event)) {

		switch(event.type) {

			case MANYMOUSE_EVENT_RELMOTION:
				//cout << "Mouse moved " << event.value << " on " << (event.item == 0 ? "X" : "Y") << " axis." << endl;

				if (event.item == 0) {
					m_mousePositionDelta.x += event.value;
				}
				else {
					m_mousePositionDelta.y -= event.value; // vertical scroll seems to be inverted
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
				cout << "Mouse " << event.device << " disconnected." << endl;
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
		cerr << "ManyMouse failed to initialize!" << endl;
	}
	else if (availableMice == 0) {
		cerr << "No mice detected!" << endl;
	}
	else {
		cout << "ManyMouse driver: " << ManyMouse_DriverName() << endl;
		for (int m = 0; m<availableMice; ++m) {
			cout << "mouse " << m << ": " << ManyMouse_DeviceName(m) << endl;
		}
	}
}

void InputManager::quitManyMouse() {
	ManyMouse_Quit();
}

//void InputManager::initGainput() {
//	//m_gainputInputManager = make_shared<gainput::InputManager>();
//
//	// setup gainput
//	gainputInputManager = new gainput::InputManager(false);
//	//		cout << "gainput time: " << manager->GetTime() << endl;
//			gainputInputManager->SetDisplaySize(800, 600);
//	gainputMouseId = gainputInputManager->CreateDevice<gainput::InputDeviceMouse>();
//	//		keyboardId = manager->CreateDevice<gainput::InputDeviceKeyboard>();
//	//		padId = manager->CreateDevice<gainput::InputDevicePad>();
//	//
//	gainputInputMap = new gainput::InputMap(*gainputInputManager);
//	//		map->MapBool(ButtonMenu, keyboardId, gainput::KeyEscape);
//	//		map->MapBool(ButtonConfirm, mouseId, gainput::MouseButtonLeft);
//	gainputInputMap->MapFloat(MouseX, gainputMouseId, gainput::MouseAxisX);
//	gainputInputMap->MapFloat(MouseY, gainputMouseId, gainput::MouseAxisY);
//	//		map->MapBool(ButtonConfirm, padId, gainput::PadButtonA);
//}

void InputManager::registerGLFWCallbacks() {
	// register GLFW callbacks
	
	// TODO: USE A LOCAL GLFEWINDOW FOR WINDOW!
	GLFWwindow* glfwWindow = g_glfwWindow;
	
	glfwSetMouseButtonCallback(glfwWindow, InputManager::glfwMouseButtonCallback);
	//	glfwSetCursorPosCallback(glfwWindow, glfwCursorPositionCallback);
	//	glfwSetScrollCallback(glfwWindow, glfwScrollWheelCallback);
	glfwSetKeyCallback(glfwWindow, InputManager::glfwKeyCallback);
}

void InputManager::unregisterGLFWCallbacks() {
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

void InputManager::clearKeysDown() {
	//m_keysDown.clear();
}

void InputManager::clearMouseDown() {
	//m_mouseButtonsDown.clear();
}

void InputManager::clearMousePositionDelta() {
	m_mousePositionDelta.x = 0.0f;
	m_mousePositionDelta.y = 0.0f;
}

void InputManager::clearMouseScrollWheelDelta() {
	m_mouseScrollWheelDelta.x = 0.0f;
	m_mouseScrollWheelDelta.y = 0.0f;
}

