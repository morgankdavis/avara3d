//
//  GLFWInputManager.cc
//  avara3d
//
//  Created by Morgan Davis on 4/17/2024.
//  Copyright © 2024-2024 Morgan K Davis. All rights reserved.
//

#include "a3d/input/GLFWInputManager.h"

#ifdef A3D_MACOS
#include <IOKit/hid/IOHIDLib.h> // for kIOReturnNotPermitted
#endif

#include <GLFW/glfw3.h>

#include "a3d/log/Log.h"
#include "a3d/render/context/GLFWWindow.h"
#include "a3d/scene/Scene.h"
#include "a3d/visual/VisualWorld.h"

using namespace a3d;
using namespace std;
using namespace a3d::math;

/// Private Static Non-Member Prototypes ///

static GLFWInputManager* InputManagerFromGLFWWindow(GLFWwindow* glfwWindow);

/// Public Lifecycle Functions ///

GLFWInputManager::GLFWInputManager(GLFWWindow* window):
    DesktopInputManager{},
    _window{window}/*,
    _usingManyMouse{false}*/ {

    window->inputManager(this);
    initMouseInput();
}

GLFWInputManager::~GLFWInputManager() {
	log::d()("Destroying GLFWInputManager {:p}", static_cast<void *>(this));

}

/// InputManager Internal Member Functions ///

void GLFWInputManager::update() {

	// would be great to poll for everything GLFW, but GLFW does not have a polling
	// function for mouse wheel scroll position/delta, so might as well keep it
	// consistent and use callback for everything GLFW...

	_window->pollInput();
}

/// Internal Member Functions ///

void GLFWInputManager::glfwMouseDeltaEvent(double xDelta, double yDelta) {

	_mousePositionDelta.x += xDelta;
	_mousePositionDelta.y += yDelta;
}

void GLFWInputManager::glfwMouseButtonEvent(int button, int action, int mods) {

	auto a3dButton = static_cast<MouseButton>(button);

	if (action == GLFW_PRESS) {
		_mouseButtonsDown.insert(a3dButton);

		// if button is in "cleared" it means the client already read it, so don't add it again until
		// we get button up, and then back down again
		if (_mouseButtonsPressedCleared.count(a3dButton) == 0) {
			_mouseButtonsPressed.insert(a3dButton);
		}
	}
	else if (action == GLFW_RELEASE) {
		_mouseButtonsDown.erase(a3dButton);
        _mouseButtonsPressedCleared.erase(a3dButton);
    }
}

void GLFWInputManager::glfwScrollEvent(double xOffset, double yOffset) {

    _mouseScrollWheelDelta.x += (float)xOffset;
    _mouseScrollWheelDelta.y += (float)yOffset;
}

void GLFWInputManager::glfwKeyEvent(int key, int scanCode, int action, int mods) {

    if (action == GLFW_PRESS) {
        _keysDown.insert(static_cast<Key>(key));

        // if key is in "cleared" it means the client already read it, so don't add it again until
        // we get key up, and then back down again
        if (_keysPressedCleared.count(static_cast<Key>(key)) == 0) {
            _keysPressed.insert(static_cast<Key>(key));
        }
    } else if (action == GLFW_RELEASE) {
        _keysDown.erase(static_cast<Key>(key));
        _keysPressedCleared.erase(static_cast<Key>(key));
    }
}

/// Private Member Functions ///

void GLFWInputManager::initMouseInput() {
#if defined(A3D_DESKTOP) && !defined(A3D_WEB)
	if (glfwRawMouseMotionSupported()) {
		log::i()("Using GLFW raw mouse input.");
		glfwSetInputMode(_window->glfwWindow(), GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
	}
	else {
		log::w()("GLFW raw mouse input is not supported on this platform.");
	}
#endif
}

/// Private Static Functions ///

GLFWInputManager* InputManagerFromGLFWWindow(GLFWwindow* glfwWindow) {

    auto window = (GLFWWindow*)glfwGetWindowUserPointer(glfwWindow);
    return dynamic_cast<GLFWInputManager*>(window->visualWorld()->scene()->inputManager());
}
