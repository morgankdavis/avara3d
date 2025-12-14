//
//  QtViewport.cpp
//  avara3d
//
//  Created by Morgan Davis on 12/2/2025.
//  Copyright © 2025 Morgan K Davis. All rights reserved.
//

#include "QtViewport.h"

#include <QDateTime>
#include <QEvent>
#include <QMouseEvent>
#include <QOpenGLFunctions_3_3_Core>
#include <QWidget>1
#include <QWindow>

#include "imgui.h"

#include "a3d/a3d.h"

#include "QtInputManager.h"

using namespace a3d;
using namespace a3d::math;
using namespace std;
using Viewport = a3d::head::qt::QtViewport;

/// Private Static Non-Member Prototypes ///

static ImGuiKey ImGuiKeyFromQtKey(int qtKey);

/// Public Lifecycle Functions ///

Viewport::QtViewport(RenderingApi renderingApi,
					 AntialiasingMode antialiasingMode,
					 QWidget* parent):
		RenderContext(renderingApi),
		QOpenGLWidget(parent),
		_scene{},
		_cursorCaptured{false},
		_lastCursorPosition{},
		_lastCapturedCursorPosition{},
		_inputManager{nullptr},
		_warpingCursor{false} {

	setMinimumSize(320, 240);

	QSurfaceFormat fmt;
//	fmt.setDepthBufferSize(24);
//	fmt.setStencilBufferSize(8);
	fmt.setVersion(3, 3);
	fmt.setProfile(QSurfaceFormat::CoreProfile);
	fmt.setSamples(static_cast<underlying_type<AntialiasingMode>::type>(antialiasingMode));
	setFormat(fmt);
	_antialiasingMode = antialiasingMode;

	setMouseTracking(true);
	setFocusPolicy(Qt::StrongFocus); // tab + click focus
	// setFocusPolicy(Qt::ClickFocus);
	setFocus();
}

//Viewport::~QtViewport() {
//	//ImGui::DestroyContext();
//}

/// Public Member Functions ///

Scene* Viewport::scene() const {
	return _scene;
}

void Viewport::scene(Scene* scene) {
	_scene = scene;
}

bool Viewport::cursorCaptured() const {
	return _cursorCaptured;
}

void Viewport::cursorCaptured(bool captured) {
	// according to CGPT, Wayland has a mechanism to "freeze" the cursor
	// (not to warp it) but as of 6.10.1, Qt does not provide an API for it.

	_cursorCaptured = captured;

	if (!windowHandle()) {
		this->winId(); // forces creation
	}

	windowHandle()->setMouseGrabEnabled(captured);
	//setMouseTracking(captured); // HMMMMMMMMMMMMMMMMMMMMMMMMMMMMM

	if (captured) {

		if (!_cursorCaptured) {
			_lastCapturedCursorPosition = {};
		}

		setCursor(Qt::BlankCursor);
		grabMouse();
		grabKeyboard();

		centerCursor();

		_cursorCaptured = true;
	}
	else {

		setCursor(Qt::ArrowCursor);
		//QGuiApplication::restoreOverrideCursor();
		releaseMouse();
		releaseKeyboard();

		_cursorCaptured = false;
	}
}

/// Internal Member Functions ///

void Viewport::inputManager(QtInputManager* manager) {
	_inputManager = manager;
}

/// RenderContext Public Member Functions ///

bool Viewport::vSyncEnabled() const {
	return true;
}

void Viewport::vSyncEnabled(bool enabled) {
	throw Exception("Qt forces vsync.");
}

/// RenderContext Internal Member Functions ///

void Viewport::beginFrame(const a3d::Scene& scene) {}

void Viewport::endFrame(const a3d::Scene& scene) {}

void Viewport::swapBuffers() {}

uvec2 Viewport::viewportLogicalSize() const {
	return uvec2(width(), height());
}

vec2 Viewport::viewportScale() const {
	auto s = devicePixelRatioF();
	return vec2(s, s);
}

unsigned Viewport::defaultFramebuffer() const {
	return static_cast<unsigned>(defaultFramebufferObject());
}

/// QWidget Protected Member Functions ///

bool Viewport::event(QEvent* e) {

	if (_renderer->isInitialized()) {

		ImGuiIO& io = ImGui::GetIO();

		auto type = e->type();
		switch (type) {
			case QEvent::MouseMove: {
				if (!_cursorCaptured) {
					auto *ev = static_cast<QMouseEvent *>(e);
					const QPointF p = ev->position();
					io.MousePos = ImVec2(float(p.x()), float(p.y()));
				}
				break; }
			case QEvent::MouseButtonPress:
			case QEvent::MouseButtonRelease: {
				auto* ev = static_cast<QMouseEvent*>(e);
				const bool down = (e->type() == QEvent::MouseButtonPress);
				int buttonIndex = 0;
				switch (ev->button()) {
					case Qt::LeftButton:
						buttonIndex = 0;
						break;
					case Qt::RightButton:
						buttonIndex = 1;
						break;
					case Qt::MiddleButton:
						buttonIndex = 2;
						break;
					default:
						break;
				}
				if (_cursorCaptured && _inputManager) {
					if (type == QEvent::MouseButtonPress) {
						_inputManager->mouseButtonPressed(buttonIndex);
					}
					else {
						_inputManager->mouseButtonReleased(buttonIndex);
					}
				}
				else if (buttonIndex >= 0 && buttonIndex < IM_ARRAYSIZE(io.MouseDown)) {
					io.MouseDown[buttonIndex] = down;
				}
				break; }
			case QEvent::Wheel: {
				auto* ev = static_cast<QWheelEvent*>(e);
				const QPoint numDegrees = ev->angleDelta() / 8;
				if (_cursorCaptured && _inputManager) {
					_inputManager->mouseWheelScrolled(numDegrees.x(), numDegrees.y());
				}
				else {
					if (numDegrees.y() != 0) {
						io.MouseWheel += float(numDegrees.y()) / 120.0f;
					}
					if (numDegrees.x() != 0) {
						io.MouseWheelH += float(numDegrees.x()) / 120.0f;
					}
				}
				break; }
			default:
				break;
		}
	}

	return QOpenGLWidget::event(e);
}

void Viewport::keyPressEvent(QKeyEvent* e) {

	// e->accept() == don't propagate further -- what about QOpenGLWidget::keyPressEvent(e)?

	if (_renderer->isInitialized()) {

		int key = e->key();
		int mods = e->modifiers();

		if (!ImGui::GetIO().WantCaptureKeyboard && _inputManager) {
			_inputManager->keyPressed(key, mods);
		}
		else if (!_cursorCaptured) {

			ImGuiIO& io = ImGui::GetIO();
			const QString text = e->text();

			if (!text.isEmpty()) {
				QByteArray utf8 = text.toUtf8();
				io.AddInputCharactersUTF8(utf8.constData());
			}

			io.AddKeyEvent(ImGuiKeyFromQtKey(key), true);
			auto mods = e->modifiers();
			io.AddKeyEvent(ImGuiMod_Ctrl,  mods.testFlag(Qt::ControlModifier));
			io.AddKeyEvent(ImGuiMod_Shift, mods.testFlag(Qt::ShiftModifier));
			io.AddKeyEvent(ImGuiMod_Alt,   mods.testFlag(Qt::AltModifier));
			io.AddKeyEvent(ImGuiMod_Super, mods.testFlag(Qt::MetaModifier));
		}
	}
	QOpenGLWidget::keyPressEvent(e);
}

void Viewport::keyReleaseEvent(QKeyEvent* e) {

	// e->accept() == don't propagate further -- what about QOpenGLWidget::keyPressEvent(e)?

	if (_renderer->isInitialized()) {

		int key = e->key();
		int mods = e->modifiers();

		if (!ImGui::GetIO().WantCaptureKeyboard && _inputManager) {
			_inputManager->keyReleased(key, mods);
		}
		else if (!_cursorCaptured) {

			ImGuiIO& io = ImGui::GetIO();

			io.AddKeyEvent(ImGuiKeyFromQtKey(key), false);
			auto mods = e->modifiers();
			io.AddKeyEvent(ImGuiMod_Ctrl,  mods.testFlag(Qt::ControlModifier));
			io.AddKeyEvent(ImGuiMod_Shift, mods.testFlag(Qt::ShiftModifier));
			io.AddKeyEvent(ImGuiMod_Alt,   mods.testFlag(Qt::AltModifier));
			io.AddKeyEvent(ImGuiMod_Super, mods.testFlag(Qt::MetaModifier));
		}
	}

	QOpenGLWidget::keyReleaseEvent(e);
}

void Viewport::mouseMoveEvent(QMouseEvent *e) {

	auto pos = e->position();
	if (!_lastCursorPosition) {
		_lastCursorPosition = pos;
	}

	if (_renderer->isInitialized()) {

		if (e->source() == Qt::MouseEventNotSynthesized) {

			QPointF center(width() / 2.0, height() / 2.0);

			if (_warpingCursor) {
				_warpingCursor = false;
				_lastCapturedCursorPosition = center;
				return;
			}

			if (!_lastCapturedCursorPosition) {
				_lastCapturedCursorPosition = pos;
			}

			QPointF delta = *_lastCursorPosition - *_lastCapturedCursorPosition;

			if (_cursorCaptured && _inputManager) {
				_inputManager->mouseMoved(float(delta.x()), float(delta.y()));
			}
			else {
				// imgui handled in event()
			}

			_lastCapturedCursorPosition = pos;

			if (_cursorCaptured) {
				_warpingCursor = true;
				centerCursor();
				_lastCapturedCursorPosition = center;
			}
		}
	}

	_lastCursorPosition = pos;

	e->accept();
}

/// QOpenGLWidget Protected Member Functions ///

void Viewport::initializeGL() {

	auto loader = [](const char* name) -> void* {
		// QOpenGLContext::getProcAddress returns a function pointer
		// that we can reinterpret_cast to void* for GLAD.
		auto* ctx = QOpenGLContext::currentContext();
		if (!ctx) return nullptr;
		QFunctionPointer fp = ctx->getProcAddress(name);
		return reinterpret_cast<void*>(fp);
	};

	if (a3d::OpenGLRenderer::InitGL(loader)) {
		_renderer->initialize(*this);
	}
	else {
		A3D_LOG_F("Failed to initialize OpenGL function loader.");
	}
}

void Viewport::resizeGL(int w, int h) {}

void Viewport::paintGL() {

	static qint64 lastNs = 0;
	qint64 nowNs = QDateTime::currentMSecsSinceEpoch() * 1000000ll;
	if (lastNs == 0) lastNs = nowNs;
	double dt = double(nowNs - lastNs) / 1e9;
	lastNs = nowNs;

//	ImGuiIO& io = ImGui::GetIO();
//	const auto w  = float(width());
//	const auto h  = float(height());
//	const auto dpr = float(devicePixelRatioF());
//	io.DisplaySize = ImVec2(w, h);
//	io.DisplayFramebufferScale = ImVec2(dpr, dpr);
//	io.DeltaTime = (dt > 0.0) ? float(dt) : 1.0f/60.0f;

	_scene->update();

	update();
}

/// Private Member Functions ///

void Viewport::centerCursor() {
//	QCursor::setPos(round(width()/2.0), round(height()/2.0));
	QPoint center(width() / 2, height() / 2);
	QCursor::setPos(mapToGlobal(center));
}

/// Private Static Non-Member Functions ///

ImGuiKey ImGuiKeyFromQtKey(int qtKey) {

	using IK = ImGuiKey;

	switch (qtKey) {
		case Qt::Key_Backspace: return ImGuiKey_Backspace;
		case Qt::Key_Delete:    return ImGuiKey_Delete;
		case Qt::Key_Tab:       return ImGuiKey_Tab;
		case Qt::Key_Left:      return ImGuiKey_LeftArrow;
		case Qt::Key_Right:     return ImGuiKey_RightArrow;
		case Qt::Key_Up:        return ImGuiKey_UpArrow;
		case Qt::Key_Down:      return ImGuiKey_DownArrow;
		case Qt::Key_Home:      return ImGuiKey_Home;
		case Qt::Key_End:       return ImGuiKey_End;
		case Qt::Key_PageUp:    return ImGuiKey_PageUp;
		case Qt::Key_PageDown:  return ImGuiKey_PageDown;
		case Qt::Key_Return:
		case Qt::Key_Enter:     return ImGuiKey_Enter;
		case Qt::Key_Escape:    return ImGuiKey_Escape;
		default:                break;
	}

	return ImGuiKey_None;
}
