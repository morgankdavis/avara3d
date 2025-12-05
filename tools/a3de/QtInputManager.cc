//
//  QtInputManager.cc
//  avara3d
//
//  Created by Morgan Davis on 12/2/2025.
//  Copyright © 2025 Morgan K Davis. All rights reserved.
//

#include "QtInputManager.h"

#include <QEvent>
#include <QMouseEvent>

#include "QtViewport.h"

using InputManger = a3d::head::qt::QtInputManager;

/// Private Static Prototypes ///

a3d::Key a3dKeyFromQtKey(int qtKey);

/// Public Lifecycle Functions ///

InputManger::QtInputManager(QtViewport& viewport) {
	viewport.inputManager(this);
}

/// Internal Member Functions ///

void InputManger::keyPressed(int qtKey) {

	auto a3dKey = a3dKeyFromQtKey(qtKey);

	// see note at GLFWInputManager::GLFWKeyCallback()

	_keysDown.insert(static_cast<Key>(a3dKey));

	if (_keysPressedCleared.count(static_cast<Key>(a3dKey)) == 0) {
		_keysPressed.insert(static_cast<Key>(a3dKey));
	}
}

void InputManger::keyReleased(int qtKey) {

	auto a3dKey = a3dKeyFromQtKey(qtKey);

	_keysDown.erase(static_cast<Key>(a3dKey));
	_keysPressedCleared.erase(static_cast<Key>(a3dKey));

	// see note at GLFWInputManager::GLFWKeyCallback()
}

void InputManger::mouseMoved(float x, float y) {//QPointF delta) {

	_mousePositionDelta.x += x;//delta.x();
	_mousePositionDelta.y += -y;//-delta.y();
}

void InputManger::mouseButtonPressed(int qtButton) {

	auto a3dButton = static_cast<MouseButton>(qtButton);

	// see note at GLFWInputManager::GLFWMouseButtonCallback();

	_mouseButtonsDown.insert(a3dButton);

	if (_mouseButtonsPressedCleared.count(a3dButton) == 0) {
		_mouseButtonsPressed.insert(a3dButton);
	}
}

void InputManger::mouseButtonReleased(int qtButton) {

	auto a3dButton = static_cast<MouseButton>(qtButton);

	// see note at GLFWInputManager::GLFWMouseButtonCallback();

	_mouseButtonsDown.erase(a3dButton);
	_mouseButtonsPressedCleared.erase(a3dButton);
}

void InputManger::mouseWheelScrolled(int x, int y) {//QPoint delta) {

	_mouseScrollWheelDelta.x += (float)x;
	_mouseScrollWheelDelta.y += (float)y;
}

/// InputManager Internal Member Functions ///

void InputManger::update() {}

/// Private Static Functions ///

a3d::Key a3dKeyFromQtKey(int qtKey) {

	using K = a3d::Key;

	// printable ASCII range – these align numerically
	if (qtKey >= 32 && qtKey <= 126) {
		switch (qtKey) {
			case Qt::Key_Space:        return K::Space;
			case Qt::Key_Apostrophe:   return K::Apostrophe;
			case Qt::Key_Comma:        return K::Comma;
			case Qt::Key_Minus:        return K::Minus;
			case Qt::Key_Period:       return K::Period;
			case Qt::Key_Slash:        return K::Slash;
			case Qt::Key_0:            return K::Zero;
			case Qt::Key_1:            return K::One;
			case Qt::Key_2:            return K::Two;
			case Qt::Key_3:            return K::Three;
			case Qt::Key_4:            return K::Four;
			case Qt::Key_5:            return K::Five;
			case Qt::Key_6:            return K::Six;
			case Qt::Key_7:            return K::Seven;
			case Qt::Key_8:            return K::Eight;
			case Qt::Key_9:            return K::Nine;
			case Qt::Key_Semicolon:    return K::Semicolon;
			case Qt::Key_Equal:        return K::Equal;
			case Qt::Key_A:            return K::A;
			case Qt::Key_B:            return K::B;
			case Qt::Key_C:            return K::C;
			case Qt::Key_D:            return K::D;
			case Qt::Key_E:            return K::E;
			case Qt::Key_F:            return K::F;
			case Qt::Key_G:            return K::G;
			case Qt::Key_H:            return K::H;
			case Qt::Key_I:            return K::I;
			case Qt::Key_J:            return K::J;
			case Qt::Key_K:            return K::K;
			case Qt::Key_L:            return K::L;
			case Qt::Key_M:            return K::M;
			case Qt::Key_N:            return K::N;
			case Qt::Key_O:            return K::O;
			case Qt::Key_P:            return K::P;
			case Qt::Key_Q:            return K::Q;
			case Qt::Key_R:            return K::R;
			case Qt::Key_S:            return K::S;
			case Qt::Key_T:            return K::T;
			case Qt::Key_U:            return K::U;
			case Qt::Key_V:            return K::V;
			case Qt::Key_W:            return K::W;
			case Qt::Key_X:            return K::X;
			case Qt::Key_Y:            return K::Y;
			case Qt::Key_Z:            return K::Z;
			case Qt::Key_BracketLeft:  return K::LeftBracket;
			case Qt::Key_Backslash:    return K::Backslash;
			case Qt::Key_BracketRight: return K::RightBracket;
			case Qt::Key_QuoteLeft:    return K::GraveAccent;
			default:
				break; // fall through to Unknown if we don't have it
		}
	}

	// non-printable / special keys – explicit mapping
	switch (qtKey) {
		case Qt::Key_Escape:      return K::Escape;
		case Qt::Key_Tab:         return K::Tab;
		case Qt::Key_Backspace:   return K::Backspace;
		case Qt::Key_Return:
		case Qt::Key_Enter:       return K::Enter;
		case Qt::Key_Insert:      return K::Insert;
		case Qt::Key_Delete:      return K::ForwardDelete;
		case Qt::Key_Right:       return K::Right;
		case Qt::Key_Left:        return K::Left;
		case Qt::Key_Down:        return K::Down;
		case Qt::Key_Up:          return K::Up;
		case Qt::Key_PageUp:      return K::PageUp;
		case Qt::Key_PageDown:    return K::PageDown;
		case Qt::Key_Home:        return K::Home;
		case Qt::Key_End:         return K::End;
		case Qt::Key_CapsLock:    return K::CapsLock;
		case Qt::Key_ScrollLock:  return K::ScrollLock;
		case Qt::Key_NumLock:     return K::NumLock;
		case Qt::Key_Print:       return K::PrintScreen;
		case Qt::Key_Pause:       return K::Pause;

		case Qt::Key_F1:          return K::F1;
		case Qt::Key_F2:          return K::F2;
		case Qt::Key_F3:          return K::F3;
		case Qt::Key_F4:          return K::F4;
		case Qt::Key_F5:          return K::F5;
		case Qt::Key_F6:          return K::F6;
		case Qt::Key_F7:          return K::F7;
		case Qt::Key_F8:          return K::F8;
		case Qt::Key_F9:          return K::F9;
		case Qt::Key_F10:         return K::F10;
		case Qt::Key_F11:         return K::F11;
		case Qt::Key_F12:         return K::F12;
		case Qt::Key_F13:         return K::F13;
		case Qt::Key_F14:         return K::F14;
		case Qt::Key_F15:         return K::F15;
		case Qt::Key_F16:         return K::F16;
		case Qt::Key_F17:         return K::F17;
		case Qt::Key_F18:         return K::F18;
		case Qt::Key_F19:         return K::F19;
		case Qt::Key_F20:         return K::F20;
		case Qt::Key_F21:         return K::F21;
		case Qt::Key_F22:         return K::F22;
		case Qt::Key_F23:         return K::F23;
		case Qt::Key_F24:         return K::F24;
		case Qt::Key_F25:         return K::F25;

		case Qt::Key_Shift: // *** may need to use modifiers instead ***
			// probably want to use event->modifiers() for L/R variants
			break;

		case Qt::Key_Menu:        return K::Menu;

		default:
			break;
	}

	return K::Unknown;
}
