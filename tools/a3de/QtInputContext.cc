//
//  QtInputContext.cc
//  avara3d
//
//  Created by Morgan Davis on 12/2/2025.
//  Copyright © 2025 Morgan K Davis. All rights reserved.
//

#include "QtInputContext.h"

#include <QMouseEvent>

#include "a3d/scene/Scene.h"
#include "a3d/visual/VisualWorld.h"

#include "QtViewport.h"

using namespace a3d;
using namespace std;

using QtInput = qt::QtInputContext;
using Key = DesktopInputContext::Key;
using MouseButton = DesktopInputContext::MouseButton;

/// Private Static Non-Member Prototypes ///

static Key A3DKeyFromQtKey(int qtKey, Qt::KeyboardModifiers mods = Qt::NoModifier);

/// Public Lifecycle Functions ///

QtInput::QtInputContext() {}

/// Internal Member Functions ///

void QtInput::keyPressed(int qtKey, int modifiers) {

    const auto a3dKey = A3DKeyFromQtKey(qtKey, static_cast<Qt::KeyboardModifier>(modifiers));

    // see note at GLFWInputContext::GLFWKeyCallback()

    _keysDown.insert(a3dKey);

    if (_keysPressedCleared.count(a3dKey) == 0) {
        _keysPressed.insert(a3dKey);
    }
}

void QtInput::keyReleased(int qtKey, int modifiers) {

    const auto a3dKey = A3DKeyFromQtKey(qtKey, static_cast<Qt::KeyboardModifier>(modifiers));

    _keysDown.erase(a3dKey);
    _keysPressedCleared.erase(a3dKey);
    _keysReleased.insert(a3dKey);

    // see note at GLFWInputContext::GLFWKeyCallback()
}

void QtInput::mouseMoved(float x, float y) {

    _pendingMousePositionDelta.x += x;
    _pendingMousePositionDelta.y += -y;
}

void QtInput::mouseButtonPressed(int qtButton) {

    const auto a3dButton = static_cast<MouseButton>(qtButton);

    // see note at GLFWInputContext::GLFWMouseButtonCallback();

    _mouseButtonsDown.insert(a3dButton);

    if (_mouseButtonsPressedCleared.count(a3dButton) == 0) {
        _mouseButtonsPressed.insert(a3dButton);
    }
}

void QtInput::mouseButtonReleased(int qtButton) {

    const auto a3dButton = static_cast<MouseButton>(qtButton);

    // see note at GLFWInputContext::GLFWMouseButtonCallback();

    _mouseButtonsDown.erase(a3dButton);
    _mouseButtonsPressedCleared.erase(a3dButton);
    _mouseButtonsReleased.insert(a3dButton);
}

void QtInput::mouseWheelScrolled(int x, int y) {

    _pendingMouseScrollWheelDelta.x += static_cast<float>(x);
    _pendingMouseScrollWheelDelta.y += static_cast<float>(y);
}

/// InputContext Internal Member Functions ///

void QtInput::attachedToScene(Scene& scene) {

    if (auto visualWorld = scene.visualWorld()) {
        viewport(static_cast<QtViewport*>(visualWorld->renderContext()));
    }
}

void QtInput::visualWorldAttachedToScene(Scene& scene) {

    if (auto visualWorld = scene.visualWorld()) {
        viewport(static_cast<QtViewport*>(visualWorld->renderContext()));
    }
}

/// Private Member Functions ///

void QtInput::viewport(QtViewport* viewport) {
    _viewport = viewport;
    _viewport->inputContext(this);
}

qt::QtViewport* QtInput::viewport() const {
    return _viewport;
}

/// Private Static Non-Member Functions ///

Key A3DKeyFromQtKey(int qtKey, Qt::KeyboardModifiers mods) {

    using K = Key;
    const bool keypad = mods.testFlag(Qt::KeypadModifier);

    // digits: distinguish top row vs keypad with KeypadModifier
    if (qtKey >= Qt::Key_0 && qtKey <= Qt::Key_9) {
        if (keypad) {
            int offset = qtKey - Qt::Key_0;
            return static_cast<K>(static_cast<int>(K::Keypad0) + offset);
        }
        else {
            // zero..nine share ASCII codes
            return static_cast<K>(qtKey);
        }
    }

    // letters A..Z (Qt uses ASCII codes for these)
    if (qtKey >= Qt::Key_A && qtKey <= Qt::Key_Z) {
        return static_cast<K>(qtKey);
    }

    switch (qtKey) {
        // basic punctuation
        case Qt::Key_Space:
            return K::Space;
        case Qt::Key_Apostrophe:
            return K::Apostrophe;   // '
        case Qt::Key_Comma:
            return K::Comma;
        case Qt::Key_Minus:
            return keypad ? K::KeypadSubtract : K::Minus;
        case Qt::Key_Period:
            return keypad ? K::KeypadDecimal : K::Period;
        case Qt::Key_Slash:
            return keypad ? K::KeypadDivide : K::Slash;
        case Qt::Key_Semicolon:
            return K::Semicolon;
        case Qt::Key_Equal:
            return keypad ? K::KeypadEqual : K::Equal;
        case Qt::Key_BracketLeft:
            return K::LeftBracket;
        case Qt::Key_Backslash:
            return K::Backslash;
        case Qt::Key_BracketRight:
            return K::RightBracket;
        case Qt::Key_QuoteLeft:
            return K::GraveAccent;

        // world keys (Qt defines World_0..World_35; map a couple)
//		case Qt::Key_World_1:     return K::World1;
//		case Qt::Key_World_2:     return K::World2;

        // function / navigation / editing
        case Qt::Key_Escape:
            return K::Escape;
        case Qt::Key_Tab:
            return K::Tab;
        case Qt::Key_Backspace:
            return K::Backspace;
        case Qt::Key_Insert:
            return K::Insert;
        case Qt::Key_Delete:
            return K::ForwardDelete;
        case Qt::Key_Return:
            return K::Enter;         // main Enter
        case Qt::Key_Enter:
            return K::KeypadEnter;   // keypad Enter

        case Qt::Key_Right:
            return K::Right;
        case Qt::Key_Left:
            return K::Left;
        case Qt::Key_Down:
            return K::Down;
        case Qt::Key_Up:
            return K::Up;
        case Qt::Key_PageUp:
            return K::PageUp;
        case Qt::Key_PageDown:
            return K::PageDown;
        case Qt::Key_Home:
            return K::Home;
        case Qt::Key_End:
            return K::End;

        case Qt::Key_CapsLock:
            return K::CapsLock;
        case Qt::Key_ScrollLock:
            return K::ScrollLock;
        case Qt::Key_NumLock:
            return K::NumLock;
        case Qt::Key_Print:
            return K::PrintScreen;
        case Qt::Key_Pause:
            return K::Pause;

        // function keys
        case Qt::Key_F1:
            return K::F1;
        case Qt::Key_F2:
            return K::F2;
        case Qt::Key_F3:
            return K::F3;
        case Qt::Key_F4:
            return K::F4;
        case Qt::Key_F5:
            return K::F5;
        case Qt::Key_F6:
            return K::F6;
        case Qt::Key_F7:
            return K::F7;
        case Qt::Key_F8:
            return K::F8;
        case Qt::Key_F9:
            return K::F9;
        case Qt::Key_F10:
            return K::F10;
        case Qt::Key_F11:
            return K::F11;
        case Qt::Key_F12:
            return K::F12;
        case Qt::Key_F13:
            return K::F13;
        case Qt::Key_F14:
            return K::F14;
        case Qt::Key_F15:
            return K::F15;
        case Qt::Key_F16:
            return K::F16;
        case Qt::Key_F17:
            return K::F17;
        case Qt::Key_F18:
            return K::F18;
        case Qt::Key_F19:
            return K::F19;
        case Qt::Key_F20:
            return K::F20;
        case Qt::Key_F21:
            return K::F21;
        case Qt::Key_F22:
            return K::F22;
        case Qt::Key_F23:
            return K::F23;
        case Qt::Key_F24:
            return K::F24;
        case Qt::Key_F25:
            return K::F25;

        // keypad operators (when Qt didn't already get caught above)
        case Qt::Key_Plus:
            return keypad ? K::KeypadAdd : K::Equal; // same physical key as '=' on many layouts
        case Qt::Key_Asterisk:
            return keypad ? K::KeypadMultiply : K::Unknown;

        // modifiers: Qt doesn't distinguish left/right; map to left by default.
        case Qt::Key_Shift:
            return K::LeftShift;
        case Qt::Key_Control:
            return K::LeftControl;
        case Qt::Key_Alt:
            return K::LeftAlt;
        case Qt::Key_Meta:
            return K::LeftSuper;

        case Qt::Key_Menu:
            return K::Menu;

        default:
            break;
    }

    return K::Unknown;
}
