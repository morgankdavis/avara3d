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
    DesktopInputContext {},
    _window {nullptr},
    _hasMousePosition {false}/*,
	_usingManyMouse{false}*/
{}

GLFWInputContext::~GLFWInputContext() {
    log::d()("Destroying GLFWInputContext {:p}", static_cast<void*>(this));

    window(nullptr);
}

/// InputContext Internal Member Functions ///

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

void GLFWInputContext::glfwCursorPositionEvent(double xPos, double yPos) {

    const vec2 position {static_cast<float>(xPos), static_cast<float>(yPos)};

    if (_hasMousePosition) {
        _pendingMousePositionDelta.x += position.x - _mousePosition.x;

        // Preserve A3D's existing mouse-delta convention:
        // positive Y means upward mouse motion.
        _pendingMousePositionDelta.y += _mousePosition.y - position.y;
    }

    _mousePosition = position;
    _hasMousePosition = true;
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

        _mouseButtonsReleasedCleared.erase(a3dButton);
    }
    else if (action == GLFW_RELEASE) {

        _mouseButtonsDown.erase(a3dButton);
        _mouseButtonsPressedCleared.erase(a3dButton);

        if (_mouseButtonsReleasedCleared.count(a3dButton) == 0) {
            _mouseButtonsReleased.insert(a3dButton);
        }
    }
}

void GLFWInputContext::glfwScrollEvent(double xOffset, double yOffset) {

    _pendingMouseScrollWheelDelta.x += static_cast<float>(xOffset);
    _pendingMouseScrollWheelDelta.y += static_cast<float>(yOffset);
}

void GLFWInputContext::glfwKeyEvent(int key, int scanCode, int action, int mods) {

    const auto a3dKey = static_cast<Key>(key);

    if (action == GLFW_PRESS) {

        _keysDown.insert(a3dKey);

        // a new press starts a new press/release cycle
        _keysReleasedCleared.erase(a3dKey);

        // if the client already consumed this press, don't report it again
        // until the key has been released and pressed again
        if (_keysPressedCleared.count(a3dKey) == 0) {
            _keysPressed.insert(a3dKey);
        }
    }
    else if (action == GLFW_RELEASE) {

        _keysDown.erase(a3dKey);

        // allow the next press to generate a new pressed edge
        _keysPressedCleared.erase(a3dKey);

        // if the client already consumed this release, don't report it again
        // until the key has been pressed and released again
        if (_keysReleasedCleared.count(a3dKey) == 0) {
            _keysReleased.insert(a3dKey);
        }
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

Window* GLFWInputContext::window() const {
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

    auto window = (Window*) glfwGetWindowUserPointer(glfwWindow);
    return dynamic_cast<GLFWInputContext*>(window->visualWorld()->scene()->inputContext());
}
