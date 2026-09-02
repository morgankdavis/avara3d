//
//  GLFWInputContext.cc
//  avara3d
//
//  Created by Morgan Davis on 4/17/2024.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
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

using namespace std;
using namespace a3d::math;

namespace a3d {
namespace {

    // [Private Non-Member Prototypes]

    GLFWInputContext* InputContextFromGLFWWindow(GLFWwindow* glfwWindow);

} // namespace

// [Public Lifecycle Functions]

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

// [InputContext Internal Member Functions]

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

// [DesktopInputContext Internal Member Functions]

void GLFWInputContext::rebaseMouseMotion() {
    DesktopInputContext::rebaseMouseMotion();
    _hasMousePosition = false;
}

// [Internal Member Functions]

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

    const auto a3dButton = static_cast<MouseButton>(button);

    if (action == GLFW_PRESS) {

        _mouseButtonsDown.insert(a3dButton);

        // if button is in "cleared" it means the client already read it, so don't add it again until
        // we get button up, and then back down again
        if (_mouseButtonsPressedCleared.count(a3dButton) == 0) {
            _mouseButtonsPressed.insert(a3dButton);
        }
    }
    else if (action == GLFW_RELEASE) {

        const bool wasDown = _mouseButtonsDown.erase(a3dButton) != 0;

        _mouseButtonsPressedCleared.erase(a3dButton);

        if (wasDown) {
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

        // if key is in "cleared" it means the client already read it, so don't add it again until
        // we get key up, and then back down again
        if (_keysPressedCleared.count(a3dKey) == 0) {
            _keysPressed.insert(a3dKey);
        }
    }
    else if (action == GLFW_RELEASE) {

        const bool wasDown = _keysDown.erase(a3dKey) != 0;

        _keysPressedCleared.erase(a3dKey);

        if (wasDown) {
            _keysReleased.insert(a3dKey);
        }
    }
}

void GLFWInputContext::detachedFromWindow(Window& window) {

    if (_window == &window) {
        _window = nullptr;
    }
}

// [Private Member Functions]

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

namespace {

    // [Private Non-Member Functions]

    GLFWInputContext* InputContextFromGLFWWindow(GLFWwindow* glfwWindow) {

        auto window = (Window*) glfwGetWindowUserPointer(glfwWindow);
        return dynamic_cast<GLFWInputContext*>(window->visualWorld()->scene()->inputContext());
    }

} // namespace
} // namespace a3d
