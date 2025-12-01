//
//  WindowInputManager.cc
//  avara3d
//
//  Created by Morgan Davis on 4/17/2024.
//  Copyright © 2024-2024 Morgan K Davis. All rights reserved.
//

#include "a3d/input/GlfwInputManager.h"

#ifdef MACOS
#include <IOKit/hid/IOHIDLib.h> // for kIOReturnNotPermitted
#endif

#include "GLFW/glfw3.h"
#include "manymouse.h"

#include "a3d/diagnostic/exception/Exception.h"
#include "a3d/diagnostic/log/Log.h"
#include "a3d/rendering/VisualWorld.h"
#include "a3d/rendering/context/GlfwWindow.h"
#include "a3d/scene/Scene.h"


using namespace a3d;
using namespace std;
using namespace glm;


// TODO: get rid of these/make non constant
constexpr bool FLIP_MOUSE_VERTICAL = true;
constexpr bool FLIP_MOUSE_HORIZONTAL = false;


/*********************************************************************************************
	Private Static Non-Member Prototypes
 *********************************************************************************************/

static GlfwInputManager* InputManagerFromGLFWWindow(GLFWwindow* glfwWindow);

/*********************************************************************************************
	Public Lifecycle Functions
 *********************************************************************************************/

GlfwInputManager::GlfwInputManager(GlfwWindow* window):
    InputManager{},
    _usingManyMouse{false},
    _window{window},
    _errorMask{WindowInputManagerErrorMask::None} {
    registerGLFWCallbacks(window->glfwWindow());
    initMouseInput();
}

GlfwInputManager::~GlfwInputManager() {
    A3D_LOG_D("Destroying WindowInputManager {:p}", static_cast<void*>(this));

    quitManyMouse();
    if (_window) {
        unregisterGLFWCallbacks(_window->glfwWindow());
    }
}

/*********************************************************************************************
    Public Member Functions
 *********************************************************************************************/

WindowInputManagerErrorMask GlfwInputManager::errorMask() const {
    return _errorMask;
}

/*********************************************************************************************
	InputManager Internal Member Functions
 *********************************************************************************************/

void GlfwInputManager::update() {

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
	Private Member Functions
 *********************************************************************************************/

void GlfwInputManager::initMouseInput() {
    // starting with macOS 10.15 Catalina, GLFW 3.3 raw mouse input never works, and ManyMouse
    // requires the user manually allow "Input Monitoring" in System Preferences ->
    // Privacy & Security -> Input Monitoring, or IOHIDDeviceOpen() in ManyMouse will fail with
    // message "TCC deny IOHIDDeviceOpen" / kIOReturnNotPermitted.  ManyMouse was modified to
    // surface kIOReturnNotPermitted and set _errorMask for the clien tot check.

    if (glfwRawMouseMotionSupported()) {
        A3D_LOG_I("Using GLFW raw mouse input.");
        glfwSetInputMode(_window->glfwWindow(), GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
        glfwSetCursorPosCallback(_window->glfwWindow(),
                                 GlfwInputManager::GLFWCursorPositionCallback);
        _usingManyMouse = false;
    }
    else {
        A3D_LOG_W("GLFW raw mouse input unavailable.  Using ManyMouse.");
        initManyMouse();
        _usingManyMouse = true;
    }
}

void GlfwInputManager::initManyMouse() {
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
            _errorMask = WindowInputManagerErrorMask::PermissionDenied;
        }
        else {
            _errorMask = WindowInputManagerErrorMask::UnknownError;
        }
#else
        _errorMask = WindowInputManagerErrorMask::UnknownError;
#endif
    }
    else if (availableMice == 0) {
        A3D_LOG_W("No available mice.");
        _errorMask = WindowInputManagerErrorMask::NoMice;
    }
    else {
        A3D_LOG_I("ManyMouse driver: {}", ManyMouse_DriverName());
        for (unsigned m = 0; m < availableMice; ++m) {
            A3D_LOG_I("Mouse[{}]: {}", m, ManyMouse_DeviceName(m));
        }
    }
}

void GlfwInputManager::quitManyMouse() {
    ManyMouse_Quit();
}

void GlfwInputManager::registerGLFWCallbacks(GLFWwindow* glfwWindow) {
    glfwSetMouseButtonCallback(glfwWindow, GlfwInputManager::GLFWMouseButtonCallback);
    // glfwSetCursorPosCallback -> in initMouseMotionInput()
    glfwSetScrollCallback(glfwWindow, GlfwInputManager::GLFWScrollWheelCallback);
    glfwSetKeyCallback(glfwWindow, GlfwInputManager::GLFWKeyCallback);

    // TODO: probably re-factor key callback creation code
}

void GlfwInputManager::unregisterGLFWCallbacks(GLFWwindow* glfwWindow) {
    glfwSetMouseButtonCallback(glfwWindow, nullptr);
    glfwSetCursorPosCallback(glfwWindow, nullptr);
    glfwSetScrollCallback(glfwWindow, nullptr);
    glfwSetKeyCallback(glfwWindow, nullptr);
}

/*********************************************************************************************
	Private Static Member Functions
 *********************************************************************************************/

void GlfwInputManager::GLFWMouseButtonCallback(GLFWwindow* glfwWindow,
                                               int button,
                                               int action,
                                               int mods) {
    auto inputManager = InputManagerFromGLFWWindow(glfwWindow);

    auto a3dButton = static_cast<MouseButton>(button);

    if (action == GLFW_PRESS) {
        inputManager->_mouseButtonsDown.insert(a3dButton);

        // if button is in "cleared" it means the client already read it, so don't add it again until
        // we get button up, and then back down again
        if (inputManager->_mouseButtonsPressedCleared.count(a3dButton) == 0)
        {
            inputManager->_mouseButtonsPressed.insert(a3dButton);
        }
    }
    else if (action == GLFW_RELEASE) {
        inputManager->_mouseButtonsDown.erase(a3dButton);
        inputManager->_mouseButtonsPressedCleared.erase(a3dButton);
    }
}

void GlfwInputManager::GLFWCursorPositionCallback(GLFWwindow* glfwWindow,
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

void GlfwInputManager::GLFWScrollWheelCallback(GLFWwindow* glfwWindow,
                                               double xOffset,
                                               double yOffset) {
    auto inputManager = InputManagerFromGLFWWindow(glfwWindow);

    inputManager->_mouseScrollWheelDelta.x += (float)xOffset;
    inputManager->_mouseScrollWheelDelta.y += (float)yOffset;
}

void GlfwInputManager::GLFWKeyCallback(GLFWwindow* glfwWindow,
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
	Private Static Functions
 *********************************************************************************************/

GlfwInputManager* InputManagerFromGLFWWindow(GLFWwindow* glfwWindow) {
    auto window = (GlfwWindow*)glfwGetWindowUserPointer(glfwWindow);
    return dynamic_cast<GlfwInputManager*>(window->visualWorld()->scene()->inputManager());
}
