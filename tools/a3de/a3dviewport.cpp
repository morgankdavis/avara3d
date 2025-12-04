#include "a3dviewport.h"

#include <QDateTime>
#include <QEvent>
#include <QMouseEvent>
#include <QWidget>
#include <QWindow>

#include "imgui.h"

#include "a3d/a3d.h"

#include "QtInputManager.h"


using namespace a3d;
using namespace a3de;
using namespace std;


/*********************************************************************************************
	Private Static Non-Member Prototypes
 *********************************************************************************************/

static ImGuiKey ImGuiKeyFromQtKey(int qt_key);

/*********************************************************************************************
	Public Lifecycle Functions
 *********************************************************************************************/

A3DViewport::A3DViewport(RenderingApi renderingApi, QWidget* parent):
		RenderContext(renderingApi),
		QOpenGLWidget(parent),
		_scene{},
		_cursorCaptured{false},
		_lastCapturedCursorPosition{},
		_inputManager{nullptr} {

	// optional: better default size
	setMinimumSize(1280, 768);

	//setMouseTracking(true);

	setFocusPolicy(Qt::StrongFocus); // tab + click focus
	// setFocusPolicy(Qt::ClickFocus);
	setFocus();
}

A3DViewport::~A3DViewport() {
	//ImGui::DestroyContext();
}

/*********************************************************************************************
	Public Member Functions
 *********************************************************************************************/

Scene* A3DViewport::scene() const {
	return _scene;
}

void A3DViewport::scene(Scene* scene) {
	_scene = scene;
}

bool A3DViewport::cursorCaptured() const {
	return _cursorCaptured;
}

void A3DViewport::cursorCaptured(bool captured) {
	// according to CGPT, Wayland has a mechanism to "freeze" the cursor
	// (not to warp it) but as of 6.10.1, Qt does not provide an API for it.

	_cursorCaptured = captured;

	if (!windowHandle()) {
		this->winId(); // forces creation
	}

	windowHandle()->setMouseGrabEnabled(captured);
	setMouseTracking(captured);

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

void A3DViewport::inputManager(QtInputManager* manager) {
	_inputManager = manager;
}

/*********************************************************************************************
	RenderContext Public Member Functions
 *********************************************************************************************/

bool A3DViewport::vSyncEnabled() const {
	return true;
}

void A3DViewport::vSyncEnabled(bool enabled) {
	throw Exception("Qt forces vsync.");
}

/*********************************************************************************************
	RenderContext Internal Member Functions
 *********************************************************************************************/

void A3DViewport::beginFrame(const a3d::Scene& scene) {}

void A3DViewport::endFrame(const a3d::Scene& scene) {}

void A3DViewport::swapBuffers() {} // nada

glm::uvec2 A3DViewport::framebufferSize() const {
	// note that GLFW handles scale a little differently and
	// expects framebufferSize without the multiplied scale factor.
	auto s = devicePixelRatioF();
	return {width() * s, height() * s};
}

glm::vec2 A3DViewport::framebufferScale() const {
	auto s = devicePixelRatioF();
	return {s, s};
}

unsigned A3DViewport::defaultFramebuffer() const {
	return static_cast<unsigned>(defaultFramebufferObject());
}

/*********************************************************************************************
	QWidget Protected Member Functions
 *********************************************************************************************/

bool A3DViewport::event(QEvent* e) {

	if (_renderer->isInitialized()) {

		ImGuiIO& io = ImGui::GetIO();

		switch (e->type()) {
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
				break;
			}
			default:
				break;
		}
	}

	if (_inputManager) {
		_inputManager->event(e);
	}

	return QOpenGLWidget::event(e);
}

void A3DViewport::keyPressEvent(QKeyEvent* e) {

	if (_renderer->isInitialized()) {

		ImGuiIO& io = ImGui::GetIO();

		const QString text = e->text();
		if (!text.isEmpty()) {
			QByteArray utf8 = text.toUtf8();
			io.AddInputCharactersUTF8(utf8.constData());
		}

		int key = e->key();
//		A3D_LOG_I("KEY: {}", key);
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

void A3DViewport::keyReleaseEvent(QKeyEvent* e) {

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

void A3DViewport::mouseMoveEvent(QMouseEvent *e) {

	if (e->source() == Qt::MouseEventNotSynthesized) {

		auto pos = e->position();

		if (!_lastCapturedCursorPosition.has_value()) {
			_lastCapturedCursorPosition = pos;
		}

		QPointF delta = pos - *_lastCapturedCursorPosition;
		if (_inputManager) {
			_inputManager->mouseMoved(delta);
		}

		_lastCapturedCursorPosition = pos;

//		A3D_LOG_I("position: ({}, {})", e->position().x(), e->position().y());
//		A3D_LOG_I("delta: ({}, {})", delta.x(), delta.y());

		if (_cursorCaptured) {
			centerCursor();
		}
	}
}

/*********************************************************************************************
	QOpenGLWidget Protected Member Functions
 *********************************************************************************************/

void A3DViewport::initializeGL() {

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

void A3DViewport::resizeGL(int w, int h) {}

void A3DViewport::paintGL() {

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

void A3DViewport::centerCursor() {
	QCursor::setPos(round(width()/2.0), round(height()/2.0));
}

/*********************************************************************************************
	Private Static Non-Member Functions
 *********************************************************************************************/

static ImGuiKey ImGuiKeyFromQtKey(int qt_key) {

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
