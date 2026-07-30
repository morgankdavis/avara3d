//
//  GLFWInputContext.cc
//  avara3d
//
//  Created by Morgan Davis on 4/17/2024.
//  Copyright © 2024-2024 Morgan K Davis. All rights reserved.
//

#include "a3d/input/GLFWInputContext.h"

#ifdef A3D_MACOS
#include <IOKit/hid/IOHIDLib.h> // for kIOReturnNotPermitted
#endif

#include <GLFW/glfw3.h>

#include "a3d/log/Log.h"
#include "a3d/render/context/Window.h"
#include "a3d/scene/Scene.h"
#include "a3d/visual/VisualWorld.h"

using namespace a3d;
using namespace std;
using namespace a3d::math;

/// Private Static Non-Member Prototypes ///

static GLFWInputContext* InputContextFromGLFWWindow(GLFWwindow* glfwWindow);

/// Public Lifecycle Functions ///

GLFWInputContext::GLFWInputContext():
	DesktopInputContext{},
	_window{nullptr}/*,
	_usingManyMouse{false}*/ {}

GLFWInputContext::~GLFWInputContext() {
	log::d()("Destroying GLFWInputContext {:p}", static_cast<void *>(this));

	window(nullptr);
}

/// InputContext Internal Member Functions ///

void GLFWInputContext::update() {

	// Window events are dispatched through RenderContext::pollEvents() before
	// this input stage. GLFW input state is maintained by those callbacks.
}

/// DesktopInputContext Internal Member Functions ///

void GLFWInputContext::attachedToScene(Scene& scene) {

	if (auto visualWorld = scene.visualWorld()) {
		window(static_cast<Window*>(visualWorld->renderContext()));
	}
}

void GLFWInputContext::visualWorldAttachedToScene(Scene& scene) {

	if (auto visualWorld = scene.visualWorld()) {
		window(static_cast<Window*>(visualWorld->renderContext()));
	}
}

/// Internal Member Functions ///

void GLFWInputContext::glfwMouseDeltaEvent(double xDelta, double yDelta) {

	_mousePositionDelta.x += xDelta;
	_mousePositionDelta.y += yDelta;
}

void GLFWInputContext::glfwMouseButtonEvent(int button, int action, int mods) {

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

void GLFWInputContext::glfwScrollEvent(double xOffset, double yOffset) {

    _mouseScrollWheelDelta.x += (float)xOffset;
    _mouseScrollWheelDelta.y += (float)yOffset;
}

void GLFWInputContext::glfwKeyEvent(int key, int scanCode, int action, int mods) {

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

void GLFWInputContext::detachedFromWindow(Window& window) {

	if (_window == &window) {
		_window = nullptr;
	}
}

/// Private Member Functions ///

void GLFWInputContext::window(Window* window) {

	if (_window == window) {
		return;
	}

	if (_window) {
		_window->inputContext(nullptr);
	}

	_window = window;

	if (_window) {
		_window->inputContext(this);
		initMouseInput();
	}
}

Window*	GLFWInputContext::window() const {
	return _window;
}

void GLFWInputContext::initMouseInput() {
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

GLFWInputContext* InputContextFromGLFWWindow(GLFWwindow* glfwWindow) {

    auto window = (Window*)glfwGetWindowUserPointer(glfwWindow);
    return dynamic_cast<GLFWInputContext*>(window->visualWorld()->scene()->inputContext());
}
