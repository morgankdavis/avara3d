//
//  DesktopInputContext.cc
//  avara3d
//
//  Created by Morgan Davis on 12/2/2025.
//  Copyright © 2025 Morgan K Davis. All rights reserved.
//

#include "a3d/input/DesktopInputContext.h"

#include <utility>

#include "a3d/log/Log.h"

using namespace a3d;
using namespace std;
using namespace a3d::math;

using Key = DesktopInputContext::Key;
using MouseButton = DesktopInputContext::MouseButton;

/// Public Lifecycle Functions ///

DesktopInputContext::DesktopInputContext():
    _keysDown {},
    _mouseButtonsDown {},
    _keysPressed {},
    _keysPressedCleared {},
    _mouseButtonsPressed {},
    _mouseButtonsPressedCleared {},
    _mousePosition {0.0f, 0.0f},
    _mousePositionDelta {0.0f, 0.0f},
    _pendingMousePositionDelta {0.0f, 0.0f},
    _mouseScrollWheelDelta {0.0f, 0.0f},
    _pendingMouseScrollWheelDelta {0.0f, 0.0f} {}

DesktopInputContext::~DesktopInputContext() {
    log::d()("Destroying InputContext {:p}", static_cast<void*>(this));
}

/// Public Member Functions ///

bool DesktopInputContext::keyDown(Key key) {
    return _keysDown.count(key);
}

bool DesktopInputContext::mouseButtonDown(MouseButton button) {
    return _mouseButtonsDown.count(button);
}

bool DesktopInputContext::keyPressed(Key key) {
    bool pressed = _keysPressed.count(key);
    if (pressed) {
        _keysPressed.erase(key);
        _keysPressedCleared.insert(key);
    }
    return pressed;
}

bool DesktopInputContext::mouseButtonPressed(MouseButton button) {
    bool pressed = _mouseButtonsPressed.count(button);
    if (pressed) {
        _mouseButtonsPressed.erase(button);
        _mouseButtonsPressedCleared.insert(button);
    }
    return pressed;
}

unordered_set<Key> DesktopInputContext::keysDown() {
    auto keysDownCopy = _keysDown;
    return keysDownCopy;
}

unordered_set<MouseButton> DesktopInputContext::mouseButtonsDown() {
    auto mouseButtonsDownCopy = _mouseButtonsDown;
    return mouseButtonsDownCopy;
}

unordered_set<Key> DesktopInputContext::keysPressed() {
    auto keysPressedCopy = _keysPressed;
    _keysPressed.clear();
    return keysPressedCopy;
}

unordered_set<MouseButton> DesktopInputContext::mouseButtonsPressed() {
    auto mouseButtonsPressedCopy = _mouseButtonsPressed;
    _mouseButtonsPressed.clear();
    return mouseButtonsPressedCopy;
}

vec2 DesktopInputContext::mousePosition() const {

    return _mousePosition;
}

vec2 DesktopInputContext::mousePositionDelta() const {

    return _mousePositionDelta;
}

vec2 DesktopInputContext::mouseScrollWheelDelta() const {

    return _mouseScrollWheelDelta;
}

/// InputContext Internal Member Functions ///

void DesktopInputContext::update(const InputContext::UpdateInfo&) {

    _mousePositionDelta =
        exchange(_pendingMousePositionDelta, vec2 {0.0f, 0.0f});

    _mouseScrollWheelDelta =
        exchange(_pendingMouseScrollWheelDelta, vec2 {0.0f, 0.0f});
}
