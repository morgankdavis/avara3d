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
#include <QWidget>
#include <QWindow>

#include "imgui.h"

#include "a3d/a3d.h"

#include "QtInputManager.h"

using namespace a3d;
using namespace std;
using Viewport = a3d::head::qt::QtViewport;

/*********************************************************************************************
	Private Static Non-Member Prototypes
 *********************************************************************************************/

static ImGuiKey ImGuiKeyFromQtKey(int qt_key);

/*********************************************************************************************
	Public Lifecycle Functions
 *********************************************************************************************/

Viewport::QtViewport(RenderingApi renderingApi, QWidget* parent):
		RenderContext(renderingApi),
		QOpenGLWidget(parent),
		_scene{},
		_cursorCaptured{false},
		_lastCapturedCursorPosition{},
		_inputManager{nullptr},
		_warpingCursor{false} {

	// optional: better default size
	setMinimumSize(1280, 768);

	setMouseTracking(true);

	setFocusPolicy(Qt::StrongFocus); // tab + click focus
	// setFocusPolicy(Qt::ClickFocus);
	setFocus();
}

Viewport::~QtViewport() {
	//ImGui::DestroyContext();
}

/*********************************************************************************************
	Public Member Functions
 *********************************************************************************************/

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

/*********************************************************************************************
	Public Member Functions
 *********************************************************************************************/

void Viewport::inputManager(QtInputManager* manager) {
	_inputManager = manager;
}

/*********************************************************************************************
	RenderContext Public Member Functions
 *********************************************************************************************/

bool Viewport::vSyncEnabled() const {
	return true;
}

void Viewport::vSyncEnabled(bool enabled) {
	throw Exception("Qt forces vsync.");
}

/*********************************************************************************************
	RenderContext Internal Member Functions
 *********************************************************************************************/

void Viewport::beginFrame(const a3d::Scene& scene) {}

void Viewport::endFrame(const a3d::Scene& scene) {}

void Viewport::swapBuffers() {}

glm::uvec2 Viewport::framebufferSize() const {
	// note that GLFW handles scale a little differently and
	// expects framebufferSize without the multiplied scale factor.
	auto s = devicePixelRatioF();
	return {width() * s, height() * s};
}

glm::vec2 Viewport::framebufferScale() const {
	auto s = devicePixelRatioF();
	return {s, s};
}

unsigned Viewport::defaultFramebuffer() const {
	return static_cast<unsigned>(defaultFramebufferObject());
}

/*********************************************************************************************
	QWidget Protected Member Functions
 *********************************************************************************************/

bool Viewport::event(QEvent* e) {

	if (_renderer->isInitialized()) {

		ImGuiIO& io = ImGui::GetIO();

		auto type = e->type();
		switch (type) {
			case QEvent::MouseMove: {
				auto* ev = static_cast<QMouseEvent*>(e);
				const QPointF p = ev->position();
				io.MousePos = ImVec2(float(p.x()), float(p.y()));
				break;
			}
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
				if (buttonIndex >= 0 && buttonIndex < IM_ARRAYSIZE(io.MouseDown)) {
					io.MouseDown[buttonIndex] = down;
				}
				if (_inputManager) {
					if (type == QEvent::MouseButtonPress) {
						_inputManager->mouseButtonPressed(buttonIndex);
					}
					else {
						_inputManager->mouseButtonReleased(buttonIndex);
					}
				}
				break;
			}
			case QEvent::Wheel: {
				auto* ev = static_cast<QWheelEvent*>(e);
				const QPoint numDegrees = ev->angleDelta() / 8;
				if (numDegrees.y() != 0) {
					io.MouseWheel += float(numDegrees.y()) / 120.0f;
				}
				if (numDegrees.x() != 0) {
					io.MouseWheelH += float(numDegrees.x()) / 120.0f;
				}
				if (_inputManager) {
					_inputManager->mouseWheelScrolled(numDegrees.x(), numDegrees.y());
				}
				break;
			}
			default:
				break;
		}
	}

//	if (_inputManager) {
//		_inputManager->event(e);
//	}

	return QOpenGLWidget::event(e);
}

void Viewport::keyPressEvent(QKeyEvent* e) {

	if (_renderer->isInitialized()) {

		ImGuiIO& io = ImGui::GetIO();

		const QString text = e->text();
		if (!text.isEmpty()) {
			QByteArray utf8 = text.toUtf8();
			io.AddInputCharactersUTF8(utf8.constData());
		}

		int key = e->key();
		if (key >= 0 && key < IM_ARRAYSIZE(io.KeysDown)) {
			io.KeysDown[key] = true;
		}

		io.KeyCtrl = e->modifiers().testFlag(Qt::ControlModifier);
		io.KeyShift = e->modifiers().testFlag(Qt::ShiftModifier);
		io.KeyAlt = e->modifiers().testFlag(Qt::AltModifier);
		io.KeySuper = e->modifiers().testFlag(Qt::MetaModifier);

		if (io.WantCaptureKeyboard) {
			e->accept();
		}
		else {
			if (_inputManager) {
				_inputManager->keyPressed(key);
			}
			QOpenGLWidget::keyPressEvent(e);
		}
	}
	else {
		if (_inputManager) {
			_inputManager->keyPressed(e->key());
		}
		QOpenGLWidget::keyPressEvent(e);
	}
}

void Viewport::keyReleaseEvent(QKeyEvent* e) {

	if (_renderer->isInitialized()) {

		ImGuiIO& io = ImGui::GetIO();

		int key = e->key();
		if (key >= 0 && key < IM_ARRAYSIZE(io.KeysDown)) {
			io.KeysDown[key] = false;
		}

		io.KeyCtrl = e->modifiers().testFlag(Qt::ControlModifier);
		io.KeyShift = e->modifiers().testFlag(Qt::ShiftModifier);
		io.KeyAlt = e->modifiers().testFlag(Qt::AltModifier);
		io.KeySuper = e->modifiers().testFlag(Qt::MetaModifier);

		if (io.WantCaptureKeyboard) {
			e->accept();
		}
		else {
			if (_inputManager) {
				_inputManager->keyReleased(key);
			}
			QOpenGLWidget::keyReleaseEvent(e);
		}
	}
	else {
		if (_inputManager) {
			_inputManager->keyReleased(e->key());
		}
		QOpenGLWidget::keyReleaseEvent(e);
	}
}

void Viewport::mouseMoveEvent(QMouseEvent *e) {

	if (e->source() == Qt::MouseEventNotSynthesized) {

		auto pos = e->position();
		QPointF center(width() / 2.0, height() / 2.0);

		if (_warpingCursor) {
			_warpingCursor = false;
			_lastCapturedCursorPosition = center;
			return;
		}

		if (!_lastCapturedCursorPosition.has_value()) {
			_lastCapturedCursorPosition = pos;
		}

		QPointF delta = pos - *_lastCapturedCursorPosition;
		if (_cursorCaptured && _inputManager) {
			_inputManager->mouseMoved(float(delta.x()), float(delta.y()));
		}

		_lastCapturedCursorPosition = pos;

		if (_cursorCaptured) {
			_warpingCursor = true;
			centerCursor();
			_lastCapturedCursorPosition = center;
		}
	}

	e->accept();
}

/*********************************************************************************************
	QOpenGLWidget Protected Member Functions
 *********************************************************************************************/

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

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		io.IniFilename = nullptr;
		ImGui::StyleColorsDark();
		io.DisplaySize = ImVec2(float(width()), float(height()));

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

	ImGuiIO& io = ImGui::GetIO();

	const float w  = float(width());
	const float h  = float(height());
	const float dpr = float(devicePixelRatioF());

	io.DisplaySize = ImVec2(w, h);
	io.DisplayFramebufferScale = ImVec2(dpr, dpr);

	io.DeltaTime = (dt > 0.0) ? float(dt) : 1.0f/60.0f;

	_scene->update();

	update();
}

/*********************************************************************************************
	Private Member Functions
 *********************************************************************************************/

void Viewport::centerCursor() {
//	QCursor::setPos(round(width()/2.0), round(height()/2.0));
	QPoint center(width() / 2, height() / 2);
	QCursor::setPos(mapToGlobal(center));
}

/*********************************************************************************************
	Private Static Non-Member Functions
 *********************************************************************************************/

ImGuiKey ImGuiKeyFromQtKey(int qt_key) {

	using IK = ImGuiKey;

	switch (qt_key) {
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
