//
//  GLFWWindow.cc
//  avara3d
//
//  Created by Morgan Davis on 10/21/16.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#include "a3d/render/context/GLFWWindow.h"

#include <iostream>

#include "a3d/render/backend/opengl/gl.h" // <- MUST be before GLFW
#include <GLFW/glfw3.h>
#include <imgui/backends/imgui_impl_glfw.h>

#include "a3d/log/Log.h"
#include "a3d/input/GLFWInputManager.h"
#include "a3d/physics/PhysicalWorld.h"
#include "a3d/render/backend/opengl/OGLRenderer.h"
#include "a3d/scene/Node.h"
#include "a3d/scene/Scene.h"
#include "a3d/util/filesystem.h"
#include "a3d/visual/VisualWorld.h"
#include "a3d/visual/camera/Camera.h"

using namespace a3d;
using namespace a3d::math;
using namespace std;

/// Private Static Non-Member Prototypes ///

static bool 	InitGLFW();
static void 	GLFWWindowSizeCallback(GLFWwindow* glfwWindow,
									  int width,
									  int height);
static void		GLFWWindowCloseCallback(GLFWwindow* glfwWindow);
static void		GLFWFramebufferSizeCallback(GLFWwindow* glfwWindow,
											   int width,
											   int height);
static void 	GLFWContentScaleCallback(GLFWwindow* glfwWindow,
										float xScale,
										float yScale);
static void 	GLFWErrorCallback(int error,
								 const char* description);
static bool 	GetGLFWWindowMonitor(GLFWmonitor** monitor, GLFWwindow* window);
static bool 	GetGLFWMouseMonitor(GLFWmonitor** monitor, GLFWwindow* window);

/// Public Lifescycle ///

GLFWWindow::GLFWWindow(RenderingApi renderingAPI,
					   const string& title,
					   const uvec2& size,
					   bool fullScreen,
					   bool enableHighDPI,
					   AntialiasingMode antialiasingMode):
		RenderContext{renderingAPI},
		_glfwWindow{},
		_vSyncEnabled{false},
		_highDPIEnabled{enableHighDPI},
		_open{false},
		_hidden{false},
		_cursorCaptured{false},
		_inputManager{} {
	log::d();

	_antialiasingMode = antialiasingMode; // see above (?)

	if (InitGLFW()) {
#ifdef A3D_GL_DESKTOP
		// TODO: move these version numbers
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE); // needed for macOS
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_SAMPLES, static_cast<int>(antialiasingMode));
		glfwWindowHint(GLFW_SCALE_TO_MONITOR, (enableHighDPI ? GLFW_TRUE : GLFW_FALSE));
		glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
	#ifdef A3D_LINUX
		// check if X or Wayland...?
		// TODO: change these
		glfwWindowHintString(GLFW_WAYLAND_APP_ID, "avara3d");
		glfwWindowHintString(GLFW_X11_CLASS_NAME, "avara3d");
		auto execName = util::filesystem::ExecutableName();
		if (execName != nullopt) {
			glfwWindowHintString(GLFW_X11_INSTANCE_NAME, (*execName).c_str());
		}
	#endif
	#ifdef A3D_MACOS
		// the documentation says this has the same affect as GLFW_SCALE_TO_MONITOR, but if you don't also
		// set GLFW_COCOA_RETINA_FRAMEBUFFER to GLFW_FALSE, retina framebuffer isn't actually disabled.
		glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER, (enableHighDPI ? GLFW_TRUE : GLFW_FALSE));
	#endif
#else // OpenGL ES
		glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
#endif

		if (fullScreen) {
			GLFWmonitor* monitor = glfwGetPrimaryMonitor();
			const GLFWvidmode* vmode = glfwGetVideoMode(monitor);
			_glfwWindow = unique_ptr<GLFWwindow, DestroyGLFWWindow>(
					glfwCreateWindow(vmode->width,
									 vmode->height,
									 title.c_str(),
									 monitor,
									 nullptr));
		}
		else {
			_glfwWindow = unique_ptr<GLFWwindow, DestroyGLFWWindow>(
					glfwCreateWindow((int)size.x,
									 (int)size.y,
									 title.c_str(),
									 nullptr,
									 nullptr));
		}

		if (_glfwWindow) {
			glfwSetWindowUserPointer(_glfwWindow.get(), static_cast<void*>(this));

			glfwMakeContextCurrent(_glfwWindow.get());
			GLFWWindow::vSyncEnabled(false);

			if (OGLRenderer::InitGL((GLADloadproc)glfwGetProcAddress)) {
				RenderContext::renderer()->initialize(*this);
				ImGui_ImplGlfw_InitForOpenGL(_glfwWindow.get(), true);
				registerGLFWCallbacks();
			}
			else {
				// TODO: move
				glfwTerminate();
				ImGui_ImplGlfw_Shutdown();
				throw std::runtime_error("Failed to initialize GLAD.");
			}
		}
		else {
			// TODO: move
			glfwTerminate();
			ImGui_ImplGlfw_Shutdown();
			throw std::runtime_error("Couldn't create GLFW Window.");
		}
	}
	else {
		throw std::runtime_error("Couldn't initialize GLFW.");
	}
}

GLFWWindow::~GLFWWindow() {
	log::d()("Destroying GLFWWindow {:p}", static_cast<void*>(this));

	close(); // meh?

	// TODO: must modify to support multiple windows
	unregisterGLFWCallbacks();
	glfwSetErrorCallback(nullptr);
	glfwTerminate();
	ImGui_ImplGlfw_Shutdown();
}

/// Public Member Functions ///

void GLFWWindow::open() {
	log::i();

	if (_visualWorld && _visualWorld->scene()) {
		glfwMakeContextCurrent(_glfwWindow.get());
		
		glfwSetWindowSizeCallback(_glfwWindow.get(), GLFWWindowSizeCallback);
		glfwSetWindowCloseCallback(_glfwWindow.get(), GLFWWindowCloseCallback);
		glfwSetFramebufferSizeCallback(_glfwWindow.get(), GLFWFramebufferSizeCallback);
		glfwSetWindowContentScaleCallback(_glfwWindow.get(), GLFWContentScaleCallback);

		glfwShowWindow(_glfwWindow.get());

		cursorCaptured(cursorCaptured()); // needs to be set after windows is made current

		_open = true;
	}
	else {
		throw std::runtime_error("Window has no scene.");
	}
}

void GLFWWindow::close() {

	if (_recordingGIF) {
		stopGIFRecording();
	}

//	if (_visualWorld && _visualWorld->scene()) {
//		auto scene = _visualWorld->scene();
//		if (scene->running()) {
//			scene->stop();
//		}
//	}

	glfwSetWindowSizeCallback(_glfwWindow.get(), nullptr);
	glfwSetWindowCloseCallback(_glfwWindow.get(), nullptr);
	glfwSetFramebufferSizeCallback(_glfwWindow.get(), nullptr);
	glfwSetWindowContentScaleCallback(_glfwWindow.get(), nullptr);

	cursorCaptured(false);

	glfwSetWindowShouldClose(_glfwWindow.get(), true);

	_open = false;
}

bool GLFWWindow::isOpen() const {
	// GLFW_VISIBLE is still true after the window is closed... ?
	// return glfwGetWindowAttrib(_glfwWindow.get(), GLFW_VISIBLE) == GLFW_TRUE;
	return _open;
}

string GLFWWindow::title() const {
	return glfwGetWindowTitle(_glfwWindow.get());
}

void GLFWWindow::title(const string& title) {
	glfwSetWindowTitle(_glfwWindow.get(), title.c_str());
}

uvec2 GLFWWindow::size() const {
	ivec2 size;
	glfwGetWindowSize(_glfwWindow.get(), &size.x, &size.y);
	return uvec2(size.x, size.y);
}

void GLFWWindow::size(const uvec2& size) {
	glfwSetWindowSize(_glfwWindow.get(), (int)size.x, (int)size.y);
}

uvec2 GLFWWindow::position() const {
	ivec2 pos;
	glfwGetWindowPos(_glfwWindow.get(), &pos.x, &pos.y);
	return uvec2(pos.x, pos.y);
}

void GLFWWindow::position(const uvec2& pos) {
	glfwSetWindowPos(_glfwWindow.get(), (int)pos.x, (int)pos.y);
}

void GLFWWindow::center() {
	// as of GLFW 3.3, there is no "get the monitor this window is on" function.
	// glfwGetWindowMonitor() only applies to full-screen windows.

	GLFWmonitor* monitor = nullptr;
	if (GetGLFWWindowMonitor(&monitor, _glfwWindow.get())) {

		ivec2 screenSize;
		ivec2 screenPos;
		glfwGetMonitorWorkarea(monitor, &screenPos.x, &screenPos.y, &screenSize.x, &screenSize.y);

		auto winSize = this->size();

		this->position(uvec2(screenPos.x + ((screenSize.x/2.0) - (winSize.x/2.0)),
							 screenPos.y + ((screenSize.y/2.0) - (winSize.y/2.0))));
	}
	else {
		log::e()("Can't get window monitor.");
	}
}

bool GLFWWindow::hidden() const {
	// GLFW_VISIBLE seems yp have a mind of its own..
	// return glfwGetWindowAttrib(_glfwWindow.get(), GLFW_VISIBLE) == GLFW_TRUE;
	return _hidden;
}

void GLFWWindow::hidden(bool hidden) {

	if (hidden) {
		glfwHideWindow(_glfwWindow.get());
	}
	else {
		glfwShowWindow(_glfwWindow.get());
	}
	_hidden = hidden;
}

bool GLFWWindow::cursorCaptured() const {
	return _cursorCaptured;
}

static GLFWcursor* invisible = nullptr;
static GLFWcursor* getInvisibleCursor()
{
	if (invisible) return invisible;

	const int w = 16, h = 16;
	static unsigned char pixels[w * h * 4] = {}; // all zero = transparent RGBA
	GLFWimage img{ w, h, pixels };
	invisible = glfwCreateCursor(&img, 0, 0);
	return invisible;
}


void GLFWWindow::cursorCaptured(bool captured) {

	_cursorCaptured = captured;

	auto window = _glfwWindow.get();

	if (captured) {
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		// hack.
		// GLFW_CURSOR_DISABLED is supposed to:
		// "hide the cursor and lock it to the specified window"
		// [www.glfw.org/docs/latest/input_guide.html]
		// but at least on Wayland + Kwin, it doesn't actually hide, it just freezes.
		static const int w = 16, h = 16;
		static unsigned char pixels[w * h * 4] = {};
		static GLFWimage img{ w, h, pixels };
		static auto invCursor = glfwCreateCursor(&img, 0, 0);
		glfwSetCursor(window, invCursor);
	}
	else {
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		glfwSetCursor(window, nullptr);
	}
}

bool GLFWWindow::highDPIEnabled() const {
	return _highDPIEnabled;
}

/// RenderContext Public Member Functions ///

bool GLFWWindow::vSyncEnabled() const {
	return _vSyncEnabled;
}

void GLFWWindow::vSyncEnabled(bool enabled) {

	if (enabled) {
		glfwSwapInterval(1);
	}
	else {
		glfwSwapInterval(0);
	}
	_vSyncEnabled = enabled;
}

/// RenderContext Internal Member Functions ///

void GLFWWindow::beginFrame(const Scene& scene) {
	pollInput();
	ImGui_ImplGlfw_NewFrame();
}

void GLFWWindow::endFrame(const Scene& scene) { }

void GLFWWindow::swapBuffers() {
	glfwSwapBuffers(_glfwWindow.get());
}

uvec2 GLFWWindow::viewportLogicalSize() const {
	// glfwGetWindowSize (what size() uses) return the logical size on Linux and macOS.
	// on Windows and X11 it returns the pixel size (screen coords <-> pixels 1:1)
	// Windows and X11, glfwGetWindowContentScale() will report something other than 1,
	// but it's intended as more of a UI-scaling thing, apparently.
	// see note in RenderContext::viewportScale()
	return size();
}

math::uvec2 GLFWWindow::framebufferSize() const {
	ivec2 size;
	glfwGetFramebufferSize(_glfwWindow.get(), &size.x, &size.y);
	return uvec2(size.x, size.y);
}

unsigned GLFWWindow::defaultFramebuffer() const {
	return 0;
}

/// Internal Member Functions ///

void GLFWWindow::inputManager(GLFWInputManager* manager) {
	_inputManager = manager;
}

void GLFWWindow::pollInput() {
	glfwPollEvents();
}

GLFWwindow* GLFWWindow::glfwWindow() const {
	return _glfwWindow.get();
}

/// Private Member Functions ///

void GLFWWindow::registerGLFWCallbacks() {

	glfwSetMouseButtonCallback(_glfwWindow.get(), GLFWWindow::GLFWMouseButtonCallback);
	glfwSetCursorPosCallback(_glfwWindow.get(), GLFWWindow::GLFWCursorPositionCallback);
	glfwSetScrollCallback(_glfwWindow.get(), GLFWWindow::GLFWScrollWheelCallback);
	glfwSetKeyCallback(_glfwWindow.get(), GLFWWindow::GLFWKeyCallback);
}

void GLFWWindow::unregisterGLFWCallbacks() {

	glfwSetMouseButtonCallback(_glfwWindow.get(), nullptr);
	glfwSetCursorPosCallback(_glfwWindow.get(), nullptr);
	glfwSetScrollCallback(_glfwWindow.get(), nullptr);
	glfwSetKeyCallback(_glfwWindow.get(), nullptr);
}

/// Internal Static Member Functions ///

void GLFWWindow::Destroy(GLFWwindow* window) {
	glfwDestroyWindow(window);
}

/// Private Static member Functions ///

void GLFWWindow::GLFWCursorPositionCallback(GLFWwindow* glfwWindow,
											double xPos,
											double yPos) {
	static double lastXPos = xPos;
	static double lastYPos = yPos;

	auto window = WindowFromGLFWwindow(glfwWindow);
	auto inputManager = window->_inputManager;

	if (window->cursorCaptured() && inputManager) {
		inputManager->glfwMouseDeltaEvent(-(lastXPos - xPos), (lastYPos - yPos));
	}
	else {
		ImGui_ImplGlfw_CursorPosCallback(glfwWindow, xPos, yPos);
//		if (!ImGui::GetIO().WantCaptureMouse && inputManager) {
//			inputManager->glfwMouseDeltaEvent(-(lastXPos - xPos), (lastYPos - yPos));
//		}
	}

	lastXPos = xPos;
	lastYPos = yPos;
}

void GLFWWindow::GLFWMouseButtonCallback(GLFWwindow* glfwWindow,
										 int button,
										 int action,
										 int mods) {

	auto window = WindowFromGLFWwindow(glfwWindow);
	auto inputManager = window->_inputManager;

	if (window->cursorCaptured() && inputManager) {
		inputManager->glfwMouseButtonEvent(button, action, mods);
	}
	else {
		ImGui_ImplGlfw_MouseButtonCallback(glfwWindow, button, action, mods);
//		if (inputManager) {
//			inputManager->glfwMouseButtonEvent(button, action, mods);
//		}
	}
}

void GLFWWindow::GLFWScrollWheelCallback(GLFWwindow* glfwWindow,
										 double xOffset,
										 double yOffset) {

	auto window = WindowFromGLFWwindow(glfwWindow);
	auto inputManager = window->_inputManager;

	if (window->cursorCaptured() && inputManager) {
		inputManager->glfwScrollEvent(xOffset, yOffset);
	}
	else {
		ImGui_ImplGlfw_ScrollCallback(glfwWindow, xOffset, yOffset);
//		if (inputManager) {
//			inputManager->glfwScrollEvent(xOffset, yOffset);
//		}
	}
}

void GLFWWindow::GLFWKeyCallback(GLFWwindow* glfwWindow,
								 int key,
								 int scanCode,
								 int action,
								 int mods) {

	auto window = WindowFromGLFWwindow(glfwWindow);
	auto inputManager = window->_inputManager;

	if (!ImGui::GetIO().WantCaptureKeyboard && inputManager) {
		inputManager->glfwKeyEvent(key, scanCode, action, mods);
	}
	else if (!window->cursorCaptured()) {
		ImGui_ImplGlfw_KeyCallback(glfwWindow, key, scanCode, action, mods);
//		if (!ImGui::GetIO().WantCaptureKeyboard && inputManager) {
//			inputManager->glfwKeyEvent(key, scanCode, action, mods);
//		}
	}
}

GLFWWindow* GLFWWindow::WindowFromGLFWwindow(GLFWwindow* glfwWindow) {
	return (GLFWWindow*)glfwGetWindowUserPointer(glfwWindow);
}

GLFWInputManager* GLFWWindow::InputManagerFromGLFwWindow(GLFWwindow* glfwWindow) {
	return WindowFromGLFWwindow(glfwWindow)->_inputManager;
}

/// Private Static Non-Member Functions ///

static bool InitGLFW() {
	
	static bool initialized = false;
	if (!initialized) {
		log::i();
		
		int glfwMajVers, glfwMinVers, glfwRev;
		glfwGetVersion(&glfwMajVers, &glfwMinVers, &glfwRev);
		log::i()("Starting GLFW version {}.{}.{}...", glfwMajVers, glfwMinVers, glfwRev);

		// TODO: must modify to support multiple windows
		glfwSetErrorCallback(GLFWErrorCallback);
		
		if (glfwInit()) {
			log::i()("GLFW Initialized.");
		}
		else {
			log::f()("Error initializing GLFW.");
			return false;
		}
		
		srand(time(nullptr)); // where else can we put this?
		
		initialized = true;
	}
	return true;
}

void GLFWWindowSizeCallback(GLFWwindow* glfwWindow, int width, int height) {
//	log::d()("glfwWindow: {:p}, width: {}, height: {}",
//			  static_cast<void*>(glfwWindow), width, height);

	auto window = (GLFWWindow*)glfwGetWindowUserPointer(glfwWindow);
	window->size(uvec2(width, height));
}

void GLFWWindowCloseCallback(GLFWwindow* glfwWindow) {
	log::i()("glfwWindow: {:p}", static_cast<void*>(glfwWindow));

	auto window = (GLFWWindow*)glfwGetWindowUserPointer(glfwWindow);
	window->close();
}

void GLFWFramebufferSizeCallback(GLFWwindow* glfwWindow, int width, int height) {
//	log::d()("glfwWindow: {:p}, width: {}, height: {}",
//			  static_cast<void*>(glfwWindow), width, height);

//	auto window = (GLFWWindow*)glfwGetWindowUserPointer(glfwWindow);
//	window->renderer()->viewportScaleChanged(*window);
}

void GLFWContentScaleCallback(GLFWwindow* glfwWindow, float xScale, float yScale) {
	log::d()("glfwWindow: {:p}, xScale: {}, yScale: {}",
			  static_cast<void*>(glfwWindow), xScale, yScale);
}

void GLFWErrorCallback(int error, const char* description) {
	log::e()("error: {}, description: {}", error, description);
}

bool GetGLFWWindowMonitor(GLFWmonitor** monitor, GLFWwindow* window) {
	// https://github.com/glfw/glfw/issues/1699#issuecomment-723692566

	bool success = false;

	int windowRect[4] = {0};
	glfwGetWindowPos(window, &windowRect[0], &windowRect[1]);
	glfwGetWindowSize(window, &windowRect[2], &windowRect[3]);

	int monitorsSize = 0;
	GLFWmonitor** monitors = glfwGetMonitors(&monitorsSize);

	GLFWmonitor* closestMonitor = NULL;
	int maxOverlapArea = 0;

	for (int i = 0; i < monitorsSize; ++i) {

		int monitorPos[2] = {0};
		glfwGetMonitorPos(monitors[i], &monitorPos[0], &monitorPos[1]);

		const GLFWvidmode* monitorVideoMode = glfwGetVideoMode(monitors[i]);

		// https://github.com/glfw/glfw/issues/1699#issuecomment-1892387147
		int monitorRect[4] = {
				monitorPos[0],
				monitorPos[1],
				monitorVideoMode->width,
				monitorVideoMode->height,
		};

		if (!(((windowRect[0] + windowRect[2]) < monitorRect[0]) ||
			  (windowRect[0] > (monitorRect[0] + monitorRect[2])) ||
			  ((windowRect[1] + windowRect[3]) < monitorRect[1]) ||
			  (windowRect[1] > (monitorRect[1] + monitorRect[3])))) {

			int intersectionRect[4] = {0};

			// x, width
			if (windowRect[0] < monitorRect[0]) {
				intersectionRect[0] = monitorRect[0];

				if ((windowRect[0] + windowRect[2]) < (monitorRect[0] + monitorRect[2])) {
					intersectionRect[2] = (windowRect[0] + windowRect[2]) - intersectionRect[0];
				}
				else {
					intersectionRect[2] = monitorRect[2];
				}
			}
			else {
				intersectionRect[0] = windowRect[0];

				if ((monitorRect[0] + monitorRect[2]) < (windowRect[0] + windowRect[2])) {
					intersectionRect[2] = (monitorRect[0] + monitorRect[2]) - intersectionRect[0];
				}
				else {
					intersectionRect[2] = windowRect[2];
				}
			}

			// y, height
			if (windowRect[1] < monitorRect[1]) {
				intersectionRect[1] = monitorRect[1];

				if ((windowRect[1] + windowRect[3]) < (monitorRect[1] + monitorRect[3])) {
					intersectionRect[3] = (windowRect[1] + windowRect[3]) - intersectionRect[1];
				}
				else {
					intersectionRect[3] = monitorRect[3];
				}
			}
			else {
				intersectionRect[1] = windowRect[1];

				if ((monitorRect[1] + monitorRect[3]) < (windowRect[1] + windowRect[3])) {
					intersectionRect[3] = (monitorRect[1] + monitorRect[3]) - intersectionRect[1];
				}
				else {
					intersectionRect[3] = windowRect[3];
				}
			}

			// https://github.com/glfw/glfw/issues/1699#issuecomment-1892387147
			//int overlap_area = intersection_rectangle[3] * intersection_rectangle[4];
			int overlapArea = intersectionRect[2] * intersectionRect[3];
			if (overlapArea > maxOverlapArea) {
				closestMonitor = monitors[i];
				maxOverlapArea = overlapArea;
			}
		}
	}

	if (closestMonitor) {
		*monitor = closestMonitor;
		success = true;
	}

	// true: monitor contains the monitor the window is most on
	// false: monitor is unmodified
	return success;
}

bool GetGLFWMouseMonitor(GLFWmonitor** monitor, GLFWwindow* window) {
	// https://github.com/glfw/glfw/issues/1699#issuecomment-723692566

	bool success = false;

	double cursorPos[2] = {0};
	glfwGetCursorPos(window, &cursorPos[0], &cursorPos[1]);

	int windowPos[2] = {0};
	glfwGetWindowPos(window, &windowPos[0], &windowPos[1]);

	int monitorsSize = 0;
	GLFWmonitor** monitors = glfwGetMonitors(&monitorsSize);

	// convert cursor position from window coordinates to screen coordinates
	cursorPos[0] += windowPos[0];
	cursorPos[1] += windowPos[1];

	for (int i = 0; ((!success) && (i < monitorsSize)); ++i) {

		int monitorPos[2] = {0};
		glfwGetMonitorPos(monitors[i], &monitorPos[0], &monitorPos[1]);

		const GLFWvidmode* monitorVideoMode = glfwGetVideoMode(monitors[i]);

		if ((cursorPos[0] < monitorPos[0]) ||
			(cursorPos[0] > (monitorPos[0] + monitorVideoMode->width)) ||
			(cursorPos[1] < monitorPos[1]) ||
			(cursorPos[1] > (monitorPos[1] + monitorVideoMode->height))) {

			*monitor = monitors[i];
			success = true;
		}
	}

	// true: monitor contains the monitor the mouse is on
	// false: monitor is unmodified
	return success;
}

