//
//  DesktopInputManager.cc
//  avara3d
//
//  Created by Morgan Davis on 12/2/25.
//  Copyright © 2025 Morgan K Davis. All rights reserved.
//

#include "a3d/input/DesktopInputManager.h"

//#ifdef MACOS
//#include <IOKit/hid/IOHIDLib.h> // for kIOReturnNotPermitted
//#endif
//
//#include "manymouse.h"

#include "a3d/diagnostic/log/Log.h"


using namespace a3d;
using namespace std;
using namespace glm;


//constexpr bool INVERT_MOUSE_VERTICAL = true;
//constexpr bool INVERT_MOUSE_HORIZONTAL = false;


/*********************************************************************************************
	Public Lifecycle Functions
 *********************************************************************************************/

DesktopInputManager::DesktopInputManager():
//		_usingManyMouse{false},
		_keysDown{},
		_mouseButtonsDown{},
		_keysPressed{},
		_keysPressedCleared{},
		_mouseButtonsPressed{},
		_mouseButtonsPressedCleared{},
		_mousePositionDelta{0.0f, 0.0f},
		_mouseScrollWheelDelta{0.0f, 0.0f},
		_errorMask{DesktopInputManagerErrorMask::None}
		/*_scene{}*/ {

	//initManyMouse();
}

DesktopInputManager::~DesktopInputManager() {
	A3D_LOG_D("Destroying InputManager {:p}", static_cast<void*>(this));

	//quitManyMouse();
}

/*********************************************************************************************
	Public Member Functions
 *********************************************************************************************/

bool DesktopInputManager::keyDown(Key key) {
	return _keysDown.count(key);
}

bool DesktopInputManager::mouseButtonDown(MouseButton button) {
	return _mouseButtonsDown.count(button);
}

bool DesktopInputManager::keyPressed(Key key) {
	bool pressed = _keysPressed.count(key);
	if (pressed) {
		_keysPressed.erase(key);
		_keysPressedCleared.insert(key);
	}
	return pressed;
}

bool DesktopInputManager::mouseButtonPressed(MouseButton button) {
	bool pressed = _mouseButtonsPressed.count(button);
	if (pressed) {
		_mouseButtonsPressed.erase(button);
		_mouseButtonsPressedCleared.insert(button);
	}
	return pressed;
}

unordered_set<Key> DesktopInputManager::keysDown() {
	auto keysDownCopy = _keysDown;
	return keysDownCopy;
}

unordered_set<MouseButton> DesktopInputManager::mouseButtonsDown() {
	auto mouseButtonsDownCopy = _mouseButtonsDown;
	return mouseButtonsDownCopy;
}

unordered_set<Key> DesktopInputManager::keysPressed() {
	auto keysPressedCopy = _keysPressed;
	_keysPressed.clear();
	return keysPressedCopy;
}

unordered_set<MouseButton> DesktopInputManager::mouseButtonsPressed() {
	auto mouseButtonsPressedCopy = _mouseButtonsPressed;
	_mouseButtonsPressed.clear();
	return mouseButtonsPressedCopy;
}

vec2 DesktopInputManager::mousePositionDelta() {
	auto mouseMoveDeltaCopy = _mousePositionDelta;
	clearMousePositionDelta();
	return mouseMoveDeltaCopy;
}

vec2 DesktopInputManager::mouseScrollWheelDelta() {
	auto mouseScrollWheelDeltaCopy = _mouseScrollWheelDelta;
	clearMouseScrollWheelDelta();
	return mouseScrollWheelDeltaCopy;
}

DesktopInputManagerErrorMask DesktopInputManager::errorMask() const {
	return _errorMask;
}

//Scene* DesktopInputManager::scene() const {
//	return _scene;
//}

/*********************************************************************************************
	InputManager Internal Member Functions
 *********************************************************************************************/

//void DesktopInputManager::update() {
//
////	if (_usingManyMouse) {
//
////	A3D_LOG_I("update()");
//
//		static ManyMouseEvent event;
//
//		while (ManyMouse_PollEvent(&event)) {
//			A3D_LOG_I("POLL");
//			switch (event.type) {
//				case MANYMOUSE_EVENT_RELMOTION:
//					A3D_LOG_I("MANYMOUSE_EVENT_RELMOTION");
//					if (event.item == 0) _mousePositionDelta.x = (INVERT_MOUSE_HORIZONTAL ? -event.value : event.value);
//					else _mousePositionDelta.y = (INVERT_MOUSE_VERTICAL ? -event.value : event.value);
//					A3D_LOG_I("x: {}, y: {}", _mousePositionDelta.x, _mousePositionDelta.y);
//					break;
//				case MANYMOUSE_EVENT_SCROLL:
//					if (event.item == 0) _mouseScrollWheelDelta.y += event.value;
//					else _mouseScrollWheelDelta.x += event.value;
//					break;
//				case MANYMOUSE_EVENT_DISCONNECT:
//					A3D_LOG_W("Mouse {} disconnected.", event.device);
//					break;
//				case MANYMOUSE_EVENT_ABSMOTION:
//				case MANYMOUSE_EVENT_BUTTON:
//				case MANYMOUSE_EVENT_MAX:
//					break;
//			}
//		}
//
//	//A3D_LOG_I("DONE POLLLING");
////	}
//}

/*********************************************************************************************
	Protected Member Functions
 *********************************************************************************************/

//void DesktopInputManager::initManyMouse() {
//	A3D_LOG_D("");
//
//	// TODO: must modify to support multiple windows
//	auto availableMice = ManyMouse_Init();
//
//	if (availableMice < 0) {
//		A3D_LOG_E("Error initializing ManyMouse: {}", availableMice);
//		ManyMouse_Quit(); // doesn't seem to allow for re-initialization later
//
//#ifdef MACOS
//		// special case for macOS Sonoma 10.15+
//        // see note at initMouseInput() above.
//        if (availableMice == kIOReturnNotPermitted) { // == -536870174
//            A3D_LOG_E("Please allow \"Input Monitoring\" in System Preferences -> " \
//                "Privacy & Security -> Input Monitoring");
//            _errorMask = WindowInputManagerErrorMask::PermissionDenied;
//        }
//        else {
//            _errorMask = WindowInputManagerErrorMask::UnknownError;
//        }
//#else
//		_errorMask = DesktopInputManagerErrorMask::UnknownError;
//#endif
//	}
//	else if (availableMice == 0) {
//		A3D_LOG_W("No available mice.");
//		_errorMask = DesktopInputManagerErrorMask::NoMice;
//	}
//	else {
//		A3D_LOG_I("ManyMouse driver: {}", ManyMouse_DriverName());
//		for (unsigned m = 0; m < availableMice; ++m) {
//			A3D_LOG_I("Mouse[{}]: {}", m, ManyMouse_DeviceName(m));
//		}
//	}
//}

/*********************************************************************************************
	Internal Member Functions
 *********************************************************************************************/

//void DesktopInputManager::attachedToScene(Scene& scene) {
//	A3D_LOG_T("scene: {:p}", static_cast<void*>(&scene));
//
//	_scene = &scene;
//}
//
//void DesktopInputManager::detachedFromScene(Scene& scene) {
//	A3D_LOG_T("scene: {:p}", static_cast<void*>(&scene));
//
//	_scene = nullptr;
//}

/*********************************************************************************************
	Private Member Functions
 *********************************************************************************************/

//void DesktopInputManager::initMouseInput() {
//    // starting with macOS 10.15 Catalina, GLFW 3.3 raw mouse input never works, and ManyMouse
//    // requires the user manually allow "Input Monitoring" in System Preferences ->
//    // Privacy & Security -> Input Monitoring, or IOHIDDeviceOpen() in ManyMouse will fail with
//    // message "TCC deny IOHIDDeviceOpen" / kIOReturnNotPermitted.  ManyMouse was modified to
//    // report kIOReturnNotPermitted and set _errorMask for the client to check.
//
//    if (glfwRawMouseMotionSupported()) {
//        A3D_LOG_I("Using GLFW raw mouse input.");
//        glfwSetInputMode(_window->glfwWindow(), GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
//        glfwSetCursorPosCallback(_window->glfwWindow(),
//                                 GlfwInputManager::GLFWCursorPositionCallback);
//        _usingManyMouse = false;
//    }
//    else {
//        A3D_LOG_W("GLFW raw mouse input unavailable.  Using ManyMouse.");
//        initManyMouse();
//        _usingManyMouse = true;
//    }
//}


//void DesktopInputManager::quitManyMouse() {
//
//	ManyMouse_Quit();
//}

void DesktopInputManager::clearMousePositionDelta() {
	_mousePositionDelta.x = 0.0f;
	_mousePositionDelta.y = 0.0f;
}

void DesktopInputManager::clearMouseScrollWheelDelta() {
	_mouseScrollWheelDelta.x = 0.0f;
	_mouseScrollWheelDelta.y = 0.0f;
}
