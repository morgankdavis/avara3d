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

#include "a3d/diagnostic/exception/Exception.h"
#include "a3d/diagnostic/log/Log.h"
#include "a3d/rendering/VisualWorld.h"
#include "a3d/rendering/context/GLFWWindow.h"
#include "a3d/scene/Scene.h"


#include "imgui_impl_glfw.h" // TEMPORARY



using namespace a3d;
using namespace std;
using namespace glm;

// TODO: get rid of these/make non constant
constexpr bool MANYMOUSE_INVERT_VERTICAL = true;
constexpr bool MANYMOUSE_INVERT_HORIZONTAL = false;

/// Private Static Non-Member Prototypes ///

static GLFWInputManager* InputManagerFromGLFWWindow(GLFWwindow* glfwWindow);

/// Public Lifecycle Functions ///

GLFWInputManager::GLFWInputManager(GLFWWindow* window):
    DesktopInputManager{},
    _window{window},
    _usingManyMouse{false}
   /* _errorMask{DesktopInputManagerErrorMask::None}*/ {

    window->inputManager(this);
    registerGLFWCallbacks(window->glfwWindow());
    initMouseInput();
}

GLFWInputManager::~GLFWInputManager() {
    A3D_LOG_D("Destroying GLFWInputManager {:p}", static_cast<void*>(this));

    if (_usingManyMouse) {
        quitManyMouse();
    }

    if (_window) {
        unregisterGLFWCallbacks(_window->glfwWindow());
    }
}

/// InputManager Internal Member Functions ///

void GLFWInputManager::update() {

    if (_usingManyMouse) {

        static ManyMouseEvent event;

        while (ManyMouse_PollEvent(&event)) {
            switch (event.type) {
                case MANYMOUSE_EVENT_RELMOTION:
                    if (event.item == 0) {
                        _mousePositionDelta.x = (float) (MANYMOUSE_INVERT_HORIZONTAL
                                                         ? -event.value
                                                         : event.value);
                    }
                    else {
                        _mousePositionDelta.y = (float) (MANYMOUSE_INVERT_VERTICAL
                                                         ? -event.value
                                                         : event.value);
                    }
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

    // needed for non-mouse events (keyboard, not joystrick, OTHER NON-INPUT??)
    // https://www.glfw.org/docs/latest/group__window.html#ga37bd57223967b4211d60ca1a0bf3c832
    _window->pollInput();
}

/// Private Member Functions ///

void GLFWInputManager::initMouseInput() {

    // starting with macOS 10.15 Catalina, GLFW 3.3 raw mouse input never works, and ManyMouse
    // requires the user manually allow "Input Monitoring" in System Preferences ->
    // Privacy & Security -> Input Monitoring, or IOHIDDeviceOpen() in ManyMouse will fail with
    // message "TCC deny IOHIDDeviceOpen" / kIOReturnNotPermitted.  ManyMouse was modified to
    // report kIOReturnNotPermitted and set _errorMask for the client to check.

    if (glfwRawMouseMotionSupported()) {
        A3D_LOG_I("Using GLFW raw mouse input.");
        glfwSetInputMode(_window->glfwWindow(), GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
        glfwSetCursorPosCallback(_window->glfwWindow(),
								 GLFWInputManager::GLFWCursorPositionCallback);
        _usingManyMouse = false;
    }
    else {
        A3D_LOG_W("GLFW raw mouse input unavailable.  Using ManyMouse.");
        _usingManyMouse = true;
        initManyMouse();
    }
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

void GLFWInputManager::registerGLFWCallbacks(GLFWwindow* glfwWindow) {

    glfwSetMouseButtonCallback(glfwWindow, GLFWInputManager::GLFWMouseButtonCallback);
    // glfwSetCursorPosCallback -> in initMouseMotionInput()
    glfwSetScrollCallback(glfwWindow, GLFWInputManager::GLFWScrollWheelCallback);
    glfwSetKeyCallback(glfwWindow, GLFWInputManager::GLFWKeyCallback);

    // TEMPORARY?
    glfwSetCursorEnterCallback(glfwWindow, GLFWInputManager::CursorEnterCallback);
    glfwSetCharCallback(glfwWindow, GLFWInputManager::CharCallback);

    // TODO: probably re-factor key callback creation code
}

void GLFWInputManager::unregisterGLFWCallbacks(GLFWwindow* glfwWindow) {

    glfwSetMouseButtonCallback(glfwWindow, nullptr);
    glfwSetCursorPosCallback(glfwWindow, nullptr);
    glfwSetScrollCallback(glfwWindow, nullptr);
    glfwSetKeyCallback(glfwWindow, nullptr);
}

/// Private Static Member Functions ///

void GLFWInputManager::GLFWMouseButtonCallback(GLFWwindow* glfwWindow,
											   int button,
											   int action,
											   int mods) {

    ImGui_ImplGlfw_MouseButtonCallback(glfwWindow, button, action, mods);

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

void GLFWInputManager::GLFWCursorPositionCallback(GLFWwindow* glfwWindow,
												  double xPos,
												  double yPos) {

    ImGui_ImplGlfw_CursorPosCallback(glfwWindow, xPos, yPos);

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

void GLFWInputManager::GLFWScrollWheelCallback(GLFWwindow* glfwWindow,
											   double xOffset,
											   double yOffset) {

    ImGui_ImplGlfw_ScrollCallback(glfwWindow, xOffset, yOffset);

    auto inputManager = InputManagerFromGLFWWindow(glfwWindow);

    inputManager->_mouseScrollWheelDelta.x += (float)xOffset;
    inputManager->_mouseScrollWheelDelta.y += (float)yOffset;
}

void GLFWInputManager::GLFWKeyCallback(GLFWwindow* glfwWindow,
									   int key,
									   int scancode,
									   int action,
									   int mods) {

    ImGui_ImplGlfw_KeyCallback(glfwWindow, key, scancode, action, mods);

    if (!ImGui::GetIO().WantCaptureKeyboard) {

        auto inputManager = InputManagerFromGLFWWindow(glfwWindow);

        if (action == GLFW_PRESS) {
            inputManager->_keysDown.insert(static_cast<Key>(key));

            // if key is in "cleared" it means the client already read it, so don't add it again until
            // we get key up, and then back down again
            if (inputManager->_keysPressedCleared.count(static_cast<Key>(key)) == 0) {
                inputManager->_keysPressed.insert(static_cast<Key>(key));
            }
        } else if (action == GLFW_RELEASE) {
            inputManager->_keysDown.erase(static_cast<Key>(key));
            inputManager->_keysPressedCleared.erase(static_cast<Key>(key));
        }
    }
}

// TEMPORARY

void GLFWInputManager::CursorEnterCallback(GLFWwindow* window, int entered) {
    ImGui_ImplGlfw_CursorEnterCallback(window, entered);
}

void GLFWInputManager::CharCallback(GLFWwindow* window, unsigned int c) {
    ImGui_ImplGlfw_CharCallback(window, c);
}

/// Private Static Functions ///

GLFWInputManager* InputManagerFromGLFWWindow(GLFWwindow* glfwWindow) {

    auto window = (GLFWWindow*)glfwGetWindowUserPointer(glfwWindow);
    return dynamic_cast<GLFWInputManager*>(window->visualWorld()->scene()->inputManager());
}
