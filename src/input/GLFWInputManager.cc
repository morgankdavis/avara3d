//
//  GLFWInputManager.cc
//  avara3d
//
//  Created by Morgan Davis on 4/17/2024.
//  Copyright © 2024-2024 Morgan K Davis. All rights reserved.
//

#include "a3d/input/GLFWInputManager.h"

#ifdef MACOS
#include <IOKit/hid/IOHIDLib.h> // for kIOReturnNotPermitted
#endif

#include "GLFW/glfw3.h"
#include "manymouse.h"

#include "a3d/diagnostic/log/Log.h"
#include "a3d/rendering/VisualWorld.h"
#include "a3d/rendering/context/GLFWWindow.h"
#include "a3d/scene/Scene.h"

using namespace a3d;
using namespace std;
using namespace a3d::math;

/// Private Static Non-Member Prototypes ///

static GLFWInputManager* InputManagerFromGLFWWindow(GLFWwindow* glfwWindow);

/// Public Lifecycle Functions ///

GLFWInputManager::GLFWInputManager(GLFWWindow* window):
    DesktopInputManager{},
    _window{window},
    _usingManyMouse{false} {

    window->inputManager(this);
    initMouseInput();
}

GLFWInputManager::~GLFWInputManager() {
    A3D_LOG_D("Destroying GLFWInputManager {:p}", static_cast<void*>(this));

    if (_usingManyMouse) {
        quitManyMouse();
    }
}

/// InputManager Internal Member Functions ///

void GLFWInputManager::update() {

    if (_usingManyMouse) {

        static ManyMouseEvent event;

        while (ManyMouse_PollEvent(&event)) {
            switch (event.type) {
                case MANYMOUSE_EVENT_RELMOTION:
                    if (event.item == 0) _mousePositionDelta.x = float(event.value);
                    else _mousePositionDelta.y = float(-event.value);
                    break;
                case MANYMOUSE_EVENT_SCROLL:
                    if (event.item == 0) _mouseScrollWheelDelta.y += event.value;
                    else _mouseScrollWheelDelta.x += event.value;
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

    // would be great to poll for everything GLFW, but GLFW does not have a polling
    // function for mouse wheel scroll position/delta, so might as well keep it
    // consistent and use callback for everything GLFW...

    _window->pollInput();
}

/// Internal Member Functions ///

void GLFWInputManager::glfwMouseDeltaEvent(double xDelta, double yDelta) {

    if (!_usingManyMouse) {
        _mousePositionDelta.x += xDelta;
        _mousePositionDelta.y += yDelta;
    }
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

    // starting with macOS 10.15 Catalina, GLFW 3.3 raw mouse input never works, and ManyMouse
    // requires the user manually allow "Input Monitoring" in System Preferences ->
    // Privacy & Security -> Input Monitoring, or IOHIDDeviceOpen() in ManyMouse will fail with
    // message "TCC deny IOHIDDeviceOpen" / kIOReturnNotPermitted.  ManyMouse was modified to
    // report kIOReturnNotPermitted and set _errorMask for the client to check.

    // if (glfwRawMouseMotionSupported()) {
        A3D_LOG_I("Using GLFW raw mouse input.");
        glfwSetInputMode(_window->glfwWindow(), GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
        _usingManyMouse = false;
    // }
    // else {
    //     A3D_LOG_W("GLFW raw mouse input unavailable.  Using ManyMouse.");
    //     _usingManyMouse = true;
    //     initManyMouse();
    // }
}

void GLFWInputManager::initManyMouse() {
    A3D_LOG_D("");

    // TODO: must modify to support multiple windows
    auto availableMice = ManyMouse_Init();

    if (availableMice < 0) {
        A3D_LOG_E("Error initializing ManyMouse: {}", availableMice);
        ManyMouse_Quit(); // doesn't seem to allow for re-initialization later

#ifdef MACOS
        // special case for macOS Sonoma 10.15+
        // see note at initMouseInput() above.
        if (availableMice == kIOReturnNotPermitted) { // == -536870174
            A3D_LOG_E("Please allow \"Input Monitoring\" in System Preferences -> " \
                "Privacy & Security -> Input Monitoring");
            _errorMask = DesktopInputManagerErrorMask::PermissionDenied;
        }
        else {
            _errorMask = DesktopInputManagerErrorMask::UnknownError;
        }
#else
        _errorMask = DesktopInputManagerErrorMask::UnknownError;
#endif
    }
    else if (availableMice == 0) {
        A3D_LOG_W("No available mice.");
        _errorMask = DesktopInputManagerErrorMask::NoMice;
    }
    else {
        A3D_LOG_I("ManyMouse driver: {}", ManyMouse_DriverName());
        for (unsigned m = 0; m < availableMice; ++m) {
            A3D_LOG_I("Mouse[{}]: {}", m, ManyMouse_DeviceName(m));
        }
    }
}

void GLFWInputManager::quitManyMouse() {
    ManyMouse_Quit();
}

/// Private Static Functions ///

GLFWInputManager* InputManagerFromGLFWWindow(GLFWwindow* glfwWindow) {

    auto window = (GLFWWindow*)glfwGetWindowUserPointer(glfwWindow);
    return dynamic_cast<GLFWInputManager*>(window->visualWorld()->scene()->inputManager());
}
