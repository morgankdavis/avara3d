//
//  Window.cc
//  avara3d
//
//  Created by Morgan Davis on 10/21/16.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#include "a3d/rendering/context/Window.h"

#include <iostream>
#include <sstream>

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "a3d/Utilities.h"
#include "a3d/diagnostic/exception/Exception.h"
#include "a3d/diagnostic/logging/Logger.h"
#include "a3d/input/WindowInputManager.h"
#include "a3d/physics/PhysicalWorld.h"
#include "a3d/rendering/VisualWorld.h"
#include "a3d/rendering/camera/Camera.h"
#include "a3d/rendering/renderer/Renderer.h"
#include "a3d/scene/Node.h"
#include "a3d/scene/Scene.h"


using namespace a3d;
using namespace glm;
using namespace std;


/*********************************************************************************************
	Private Static Non-Member Prototypes
 *********************************************************************************************/

static bool 	InitGLFW();
static bool 	InitGLAD();
static void 	LogGLInfo();
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
bool 			GetGLFWWindowMonitor(GLFWmonitor** monitor, GLFWwindow* window);
bool 			GetGLFWMouseMonitor(GLFWmonitor** monitor, GLFWwindow* window);

/*********************************************************************************************
	Public Lifescycle
 *********************************************************************************************/

Window::Window(RenderingApi renderingAPI,
			   const string& title,
			   const glm::uvec2& size,
			   bool fullScreen,
			   bool enableHighDPI,
			   AntialiasingMode antialiasingMode):
		RenderContext{renderingAPI},
		_glfwWindow{},
		_highDPIEnabled{enableHighDPI},
		_open{false},
		_hidden{false},
		_cursorCaptured{false} {
	A3D_LOG_D("");

	if (InitGLFW()) {
#ifdef OPENGL_DESKTOP
		// TODO: move these version numbers
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE); // needed for macOS
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_SAMPLES, static_cast<int>(antialiasingMode));
		glfwWindowHint(GLFW_SCALE_TO_MONITOR, (enableHighDPI ? GLFW_TRUE : GLFW_FALSE));
		glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
	#ifdef LINUX
		// check if X or Wayland...?
		// TODO: change these
		glfwWindowHintString(GLFW_WAYLAND_APP_ID, "avara3d");
		glfwWindowHintString(GLFW_X11_CLASS_NAME, "avara3d");
		auto execName = utils::ExecutableName();
		if (execName != nullopt) {
			glfwWindowHintString(GLFW_X11_INSTANCE_NAME, (*execName).c_str());
		}
	#endif
	#ifdef MACOS
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
			vSyncEnabled(false);

			if (InitGLAD()) {
				RenderContext::renderer()->initialize(*this);
			}
			else {
				glfwTerminate();
				throw Exception("Failed to initialize GLAD.");
			}
		}
		else {
			glfwTerminate();
			throw Exception("Couldn't create GLFW Window.");
		}
	}
	else {
		auto errStr = "Couldn't create GLFW Window.";
		A3D_LOG_F(errStr);
		throw Exception(errStr);
	}
}

Window::~Window() {
	A3D_LOG_D("Destroying Window {:p}", static_cast<void*>(this));

	close(); // meh?

	// TODO: must move to support multiple windows
	glfwSetErrorCallback(nullptr);
	glfwTerminate();
}

/*********************************************************************************************
	Public Members
 *********************************************************************************************/

void Window::open() {
	A3D_LOG_I("");

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
		throw Exception("Window has no scene.");
	}
}

void Window::close() {

	if (_recordingGIF) {
		stopGIFRecording();
	}

	if (_visualWorld && _visualWorld->scene()) {
		auto scene = _visualWorld->scene();
		if (scene->running()) {
			scene->stop();
		}
	}

	glfwSetWindowSizeCallback(_glfwWindow.get(), nullptr);
	glfwSetWindowCloseCallback(_glfwWindow.get(), nullptr);
	glfwSetFramebufferSizeCallback(_glfwWindow.get(), nullptr);
	glfwSetWindowContentScaleCallback(_glfwWindow.get(), nullptr);

	cursorCaptured(false);

	glfwSetWindowShouldClose(_glfwWindow.get(), true);

	_open = false;
}

bool Window::isOpen() const {
	// GLFW_VISIBLE is still true after the window is closes... ?
	// return glfwGetWindowAttrib(_glfwWindow.get(), GLFW_VISIBLE) == GLFW_TRUE;
	return _open;
}

string Window::title() const {
	return glfwGetWindowTitle(_glfwWindow.get());
}

void Window::title(const string& title) {
	glfwSetWindowTitle(_glfwWindow.get(), title.c_str());
}

uvec2 Window::size() const {
	ivec2 size;
	glfwGetWindowSize(_glfwWindow.get(), &size.x, &size.y);
	return {size.x, size.y};
}

void Window::size(const uvec2& size) {
	glfwSetWindowSize(_glfwWindow.get(), (int)size.x, (int)size.y);
}

uvec2 Window::position() const {
	ivec2 pos;
	glfwGetWindowPos(_glfwWindow.get(), &pos.x, &pos.y);
	return {pos.x, pos.y};
}

void Window::position(const uvec2& pos) {
	glfwSetWindowPos(_glfwWindow.get(), (int)pos.x, (int)pos.y);
}

void Window::center() {
	// as of GLFW 3.3, there is no "get the monitor this window is on" function.
	// glfwGetWindowMonitor() only applies to full-screen windows.

	GLFWmonitor* monitor = nullptr;
	if (GetGLFWWindowMonitor(&monitor, _glfwWindow.get())) {

		ivec2 screenSize;
		ivec2 screenPos;
		glfwGetMonitorWorkarea(monitor, &screenPos.x, &screenPos.y, &screenSize.x, &screenSize.y);

		auto winSize = this->size();

		this->position({ screenPos.x + ((screenSize.x/2.0) - (winSize.x/2.0)),
						 screenPos.y + ((screenSize.y/2.0) - (winSize.y/2.0)) });
	}
	else {
		A3D_LOG_E("Can't get window monitor.");
	}
}

bool Window::hidden() const {
	// GLFW_VISIBLE seems yp have a mind of its own..
	// return glfwGetWindowAttrib(_glfwWindow.get(), GLFW_VISIBLE) == GLFW_TRUE;
	return _hidden;
}

void Window::hidden(bool hidden) {

	if (hidden) {
		glfwHideWindow(_glfwWindow.get());
	}
	else {
		glfwShowWindow(_glfwWindow.get());
	}
	_hidden = hidden;
}

bool Window::cursorCaptured() const {
	return _cursorCaptured;
}

void Window::cursorCaptured(bool captured) {
	_cursorCaptured = captured;
	glfwSetInputMode(_glfwWindow.get(),
					 GLFW_CURSOR,
					 (captured ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL));
}

bool Window::highDPIEnabled() const {
	return _highDPIEnabled;
}

/*********************************************************************************************
	RenderContext Public Members
 *********************************************************************************************/

bool Window::vSyncEnabled() const {
	return _vSyncEnabled;
}

void Window::vSyncEnabled(bool enabled) {
	RenderContext::vSyncEnabled(enabled);

	if (enabled) {
		glfwSwapInterval(1);
	}
	else {
		glfwSwapInterval(0);
	}
}

/*********************************************************************************************
	RenderContext Internal Members
 *********************************************************************************************/

void Window::swapBuffers() {
	glfwSwapBuffers(_glfwWindow.get());
}

glm::uvec2 Window::framebufferSize() const {
	ivec2 size;
	glfwGetFramebufferSize(_glfwWindow.get(), &size.x, &size.y);
	return {size.x, size.y};
}


glm::vec2 Window::framebufferScale() const {
	vec2 scale;
	glfwGetWindowContentScale(_glfwWindow.get(), &scale.x, &scale.y);
	return scale;
}

/*********************************************************************************************
	Internal Members
 *********************************************************************************************/

void Window::pollInput() {
	glfwPollEvents();
}

GLFWwindow* Window::glfwWindow() const {
	return _glfwWindow.get();
}

/*********************************************************************************************
	Internal Static Members
 *********************************************************************************************/

void Window::Destroy(GLFWwindow* window) {
	glfwDestroyWindow(window);
}

/*********************************************************************************************
	Private Static Non-Members
 *********************************************************************************************/

static bool InitGLFW() {
	
	static bool initialized = false;
	if (!initialized) {
		A3D_LOG_I("");
		
		int glfwMajVers, glfwMinVers, glfwRev;
		glfwGetVersion(&glfwMajVers, &glfwMinVers, &glfwRev);
		A3D_LOG_I("Starting GLFW version {}.{}.{}...", glfwMajVers, glfwMinVers, glfwRev);

		// TODO: must move to support multiple windows
		glfwSetErrorCallback(GLFWErrorCallback);
		
		if (glfwInit()) {
			A3D_LOG_I("GLFW Initialized.");
		}
		else {
			A3D_LOG_F("Error initializing GLFW.");
			return false;
		}
		
		srand(time(nullptr)); // where else can we put this?
		
		initialized = true;
	}
	return true;
}

static bool InitGLAD() {
	A3D_LOG_I("");

	// NOTE: OpenGL context must be setup first

	static bool initialized = false;
	if (!initialized) {

		auto initStatus = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		if (initStatus != 0) {

			LogGLInfo();
			initialized = true;
		}
		else {
			A3D_LOG_F("Failed to initialize GLAD: {}", initStatus);
			return false;
		}
	}
	return true;
}

static void LogGLInfo() {

	const GLubyte* renderer = glGetString(GL_RENDERER);
	const GLubyte* version = glGetString(GL_VERSION);

	// is this cool?
	A3D_LOG_I("Renderer: {}", reinterpret_cast<const char*>(renderer));
	A3D_LOG_I("Version: {}", reinterpret_cast<const char*>(version));

	// extensions

	GLint numExtensions;
	glGetIntegerv(GL_NUM_EXTENSIONS, &numExtensions);
	ostringstream extensionsStream;
	extensionsStream << "Extensions:" << endl;
	for (GLint e=0; e < numExtensions; ++e) {
		extensionsStream << "\t" << glGetStringi(GL_EXTENSIONS, e);
		if (e < numExtensions-1) extensionsStream << endl;
	}
	A3D_LOG_I("{}", extensionsStream.str());

	// context info

	GLenum contextParams[] = {
			GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS,
			GL_MAX_CUBE_MAP_TEXTURE_SIZE,
			GL_MAX_DRAW_BUFFERS,
			GL_MAX_FRAGMENT_UNIFORM_COMPONENTS,
			GL_MAX_TEXTURE_IMAGE_UNITS,
			GL_MAX_TEXTURE_SIZE,
			GL_MAX_VARYING_FLOATS,
			GL_MAX_VERTEX_ATTRIBS,
			GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS,
			GL_MAX_VERTEX_UNIFORM_COMPONENTS,
			GL_MAX_VIEWPORT_DIMS,
			GL_STEREO,
	};
	const char* contextParamNames[] = {
			"GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS",
			"GL_MAX_CUBE_MAP_TEXTURE_SIZE",
			"GL_MAX_DRAW_BUFFERS",
			"GL_MAX_FRAGMENT_UNIFORM_COMPONENTS",
			"GL_MAX_TEXTURE_IMAGE_UNITS",
			"GL_MAX_TEXTURE_SIZE",
			"GL_MAX_VARYING_FLOATS",
			"GL_MAX_VERTEX_ATTRIBS",
			"GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS",
			"GL_MAX_VERTEX_UNIFORM_COMPONENTS",
			"GL_MAX_VIEWPORT_DIMS",
			"GL_STEREO",
	};

	// (integers)

	ostringstream contextParamsStream;
	contextParamsStream << "Context parameters:" << endl;
	const int numIntParams = 10;
	for (int p=0; p<numIntParams; ++p) {
		GLint intValue = 0;
		glGetIntegerv(contextParams[p], &intValue);
		contextParamsStream << "\t" << contextParamNames[p] << ": " << intValue << endl;
	}

	// (int vec2)

	GLint maxViewportDims[2];
	glGetIntegerv(contextParams[10], maxViewportDims);
	contextParamsStream << "\t" << contextParamNames[10] << ": " << maxViewportDims[0]
		<< ", " << maxViewportDims[0] << endl;

	// (boolean)

	GLboolean stereo = 0;
	glGetBooleanv(contextParams[11], &stereo);
	contextParamsStream << "\t" << contextParamNames[11] << ": " << (stereo ? "true" : "false");

	A3D_LOG_I("{}", contextParamsStream.str());
}

void GLFWWindowSizeCallback(GLFWwindow* glfwWindow, int width, int height) {
	A3D_LOG_D("glfwWindow: {:p}, width: {}, height: {}",
			  static_cast<void*>(glfwWindow), width, height);

	auto window = (Window*)glfwGetWindowUserPointer(glfwWindow);
	window->size({width, height});
}

void GLFWWindowCloseCallback(GLFWwindow* glfwWindow) {
	A3D_LOG_I("glfwWindow: {:p}", static_cast<void*>(glfwWindow));

	auto window = (Window*)glfwGetWindowUserPointer(glfwWindow);

	window->close();
}

void GLFWFramebufferSizeCallback(GLFWwindow* glfwWindow, int width, int height) {
	A3D_LOG_D("glfwWindow: {:p}, width: {}, height: {}",
			  static_cast<void*>(glfwWindow), width, height);
}

void GLFWContentScaleCallback(GLFWwindow* glfwWindow, float xScale, float yScale) {
	A3D_LOG_D("glfwWindow: {:p}, xScale: {}, yScale: {}",
			  static_cast<void*>(glfwWindow), xScale, yScale);
}

void GLFWErrorCallback(int error, const char* description) {
	A3D_LOG_E("error: {}, description: {}", error, description);
}

bool GetGLFWWindowMonitor(GLFWmonitor** monitor, GLFWwindow* window) {
	// I had the same idea, but somebody else saved me 45 minutes.
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
