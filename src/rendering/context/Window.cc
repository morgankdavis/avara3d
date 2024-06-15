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
//static float 	ScreenScaleFactor(GLFWmonitor* monitor);
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

/*********************************************************************************************
	Public Lifescycle
 *********************************************************************************************/

Window::Window(RenderingApi renderingAPI,
			   const string& title,
//			   unsigned width,
//			   unsigned height,
			   const glm::vec2 size,
			   bool fullScreen,
			   bool enableHighDPI,
			   AntialiasingMode antialiasingMode):
		RenderContext{renderingAPI},
		_glfwWindow{},
		_size{size},
		_highDPIEnabled{enableHighDPI},
		_cursorCaptured{false} {
//		_framebufferSizes{},
//		_framebufferScales{} {


if (InitGLFW()) {
#ifdef OPENGL_DESKTOP
		// TODO: move these version numbers
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE); // needed for macOS
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_SAMPLES, static_cast<int>(antialiasingMode));
		// works in Win10 & macOS 14, but not Arch/XFCE/X11
		// see glfwGetMonitorContentScale() below
#ifndef LINUX
		glfwWindowHint(GLFW_SCALE_TO_MONITOR, (enableHighDPI ? GLFW_TRUE : GLFW_FALSE));
#endif
#else // OpenGL ES
		glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
#endif

//		auto viewportWidth = width;
//		auto viewportHeight = height;

		GLFWmonitor* monitor = glfwGetPrimaryMonitor();
		if (fullScreen) {
			const GLFWvidmode* vmode = glfwGetVideoMode(monitor);
			_glfwWindow = unique_ptr<GLFWwindow, DestroyGLFWWindow>(
					glfwCreateWindow(vmode->width,
									 vmode->height,
									 title.c_str(),
									 monitor,
									 nullptr));
//			viewportWidth = vmode->width;
//			viewportHeight = vmode->height;

//			scaleFactor = ScreenScaleFactor(monitor);
		}
		else {
			_glfwWindow = unique_ptr<GLFWwindow, DestroyGLFWWindow>(
					glfwCreateWindow((int)round(_size.x),
									 (int)round(_size.y),
									 title.c_str(),
									 nullptr,
									 nullptr));


//			scaleFactor = ScreenScaleFactor(glfwGetPrimaryMonitor());
		}

		// TODO: This is HACK. It looks like i_glfwWindow doesn't have a GLFWmonitor at this point
		// causing a segfault.  So we'll cheat and use the main monitor (probably the right one anyway)
		//auto monitor = glfwGetPrimaryMonitor();

		if (_glfwWindow) {
			glfwSetWindowUserPointer(_glfwWindow.get(), static_cast<void*>(this));

			glfwMakeContextCurrent(_glfwWindow.get());
			vSyncEnabled(false);

			if (InitGLAD()) {
				RenderContext::renderer()->initialize(*this);

//				calculateFramebufferSize();
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

		//calculateFramebufferSize();
		
		glfwSetWindowSizeCallback(_glfwWindow.get(), GLFWWindowSizeCallback);
		glfwSetWindowCloseCallback(_glfwWindow.get(), GLFWWindowCloseCallback);
		glfwSetFramebufferSizeCallback(_glfwWindow.get(), GLFWFramebufferSizeCallback);
		glfwSetWindowContentScaleCallback(_glfwWindow.get(), GLFWContentScaleCallback);

		cursorCaptured(cursorCaptured()); // needs to be set after windows is made current
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

	cursorCaptured(false);

	glfwSetWindowShouldClose(_glfwWindow.get(), true);
}

string Window::title() const {

	return glfwGetWindowTitle(_glfwWindow.get());
}

void Window::title(const string& title) {

	glfwSetWindowTitle(_glfwWindow.get(), title.c_str());
}

const uvec2& Window::size() const {
	return _size;
}

bool Window::highDPIEnabled() const {
	return _highDPIEnabled;
}

//unsigned Window::width() const {
////	return _width;
//	return 0;
//}
//
//unsigned Window::height() const {
////	return _height;
//	return 0;
//}

bool Window::cursorCaptured() const {
	return _cursorCaptured;
}

void Window::cursorCaptured(bool captured) {
	_cursorCaptured = captured;
	glfwSetInputMode(_glfwWindow.get(),
					 GLFW_CURSOR,
					 (captured ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL));
}

/*********************************************************************************************
	RenderContext Internal Members
 *********************************************************************************************/

void Window::swapBuffers() {
	glfwSwapBuffers(_glfwWindow.get());
}

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

glm::uvec2 Window::framebufferSize() const {
	ivec2 size;
	glfwGetFramebufferSize(_glfwWindow.get(), &size.x, &size.y);
	return {size.x, size.y};
}


glm::vec2 Window::framebufferScale() const {
	vec2 scale;
	glfwGetWindowContentScale(_glfwWindow.get(), &scale.x, &scale.y);
	//return scale;
	return {1.0, 1.0};
}

/*********************************************************************************************
	Internal Members
 *********************************************************************************************/

void Window::size(const uvec2& size) {
	_size = size;
	// calculateFramebufferSize() ?
}
//void Window::width(unsigned width) {
//	_width = width;
//	framebufferWidth((unsigned)round((float)_width * _framebufferScale.x));
//}
//
//void Window::height(unsigned height) {
//	_height = height;
//	framebufferHeight((unsigned)round((float)_height * _framebufferScale.y));
//}
//
//void Window::framebufferWidth(unsigned width) {
//	_framebufferWidth = width;
//}
//
//void Window::framebufferHeight(unsigned height) {
//	_framebufferHeight = height;
//}

//void Window::framebufferSizes(const vector<vec2>& sizes) {
//	_framebufferSizes = sizes;
//}
//
//void Window::framebufferScales(const vector<vec2>& scales) {
//	_framebufferScales = scales;
//}

//const glm::vec2& Window::framebufferSize() const {
//	return _framebufferSize;
//}
//
//void Window::framebufferSize(const glm::vec2& size) {
//	_framebufferSize = size;
//}
//
//const glm::vec2& Window::framebufferScale() const {
//	return _framebufferScale;
//}
//
//void Window::framebufferScale(const glm::vec2& scale) {
//	_framebufferScale = scale;
//}

void Window::highDPIEnabled(bool enabled) {
	_highDPIEnabled = enabled;
}

//void Window::calculateFramebufferSize() {
//
//
////	void glfwGetFramebufferSize 	( 	GLFWwindow *  	window,
////										int *  	width,
////										int *  	height)
//
////	void glfwGetWindowContentScale 	( 	GLFWwindow *  	window,
////										   float *  	xscale,
////										   float *  	yscale)
//
//
//
//
//
//	//A3D_LOG_D("monitor: {:p}", static_cast<void*>(&monitor));
//
//	//				_width = viewportWidth;
////				_height = viewportHeight;
//
//	// TODO: this is wrong on Arch/XFCE/X11
//
//	// scale factor ~0.9583 on XPS 13 9310
////#ifndef LINUX
//
//
//	//GLFWmonitor* monitor = glfwGetWindowMonitor(_glfwWindow.get());
//	// if the window hasn't been opened yet, its monitor will be null.
//	// (haven't actually exclicitly tested that opening -> now has a monitor...)
//	// set the default monitor to the primary monitor.
//	//glfwSetWindowMonitor(_glfwWindow.get(), monitor, 0, 0, 0, 0, 0);
//
//
//
//	// multi-monitor guide: https://www.glfw.org/docs/3.0/monitor.html
////	int numMons;
////	GLFWmonitor** monitors = glfwGetMonitors(&numMons);
////	for (int m=0; m<numMons; m++) {
////		GLFWmonitor* mon = monitors[m];
//
////		if (_highDPIEnabled) {
////			float scaleFactorX = 1.0;
////			float scaleFactorY = 1.0;
////			glfwGetMonitorContentScale(monitor, &scaleFactorX, &scaleFactorY);
////			_framebufferScale = {scaleFactorX, scaleFactorY};
////		}
////	}
////#endif
//
////	_framebufferSize = { round(_size.x * _framebufferScale.x),
////						 round(_size.y * _framebufferScale.y) };
////				_framebufferWidth = round(_size.x * _framebufferScale.x); // was floor()?
////				_framebufferHeight = round(_size.y * _framebufferScale.y); // was floor()?
//}

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
		A3D_LOG_C();
		
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
	A3D_LOG_C();

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

static void LogGLInfo()
{
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

//static float ScreenScaleFactor(GLFWmonitor* monitor) {
//#ifdef MACOS
//	//GLFWmonitor* monitor = glfwGetPrimaryMonitor();
//	//GLFWmonitor* monitor = glfwGetWindowMonitor(glfwWindow);
//	CGDirectDisplayID cgDisplayID = glfwGetCocoaMonitor(monitor);
//	CGDisplayModeRef currentModeRef = CGDisplayCopyDisplayMode(cgDisplayID);
//
//	Size width = CGDisplayModeGetWidth(currentModeRef);
//	Size pixelWidth = CGDisplayModeGetPixelWidth(currentModeRef);
//	return (float)pixelWidth / (float)width;
//#endif
//	return 1.0;
//}

void GLFWWindowSizeCallback(GLFWwindow* glfwWindow, int width, int height) {
	A3D_LOG_D("width: {}, height: {}", width, height);

	auto window = (Window*)glfwGetWindowUserPointer(glfwWindow);

//	window->width(width);
//	window->height(height);
	window->size({width, height});
}

void GLFWWindowCloseCallback(GLFWwindow* glfwWindow) {
	A3D_LOG_I("glfwWindow: {:p}", static_cast<void*>(glfwWindow));

	auto window = (Window*)glfwGetWindowUserPointer(glfwWindow);

	window->close();
}

void GLFWFramebufferSizeCallback(GLFWwindow* glfwWindow, int width, int height) {
	A3D_LOG_D("width: {}, height: {}", width, height);

//	auto* window = (Window*)glfwGetWindowUserPointer(glfwWindow);

//	auto framebufferScale = window->framebufferScale();
//	window->framebufferWidth((unsigned)round(float(window->width()) * framebufferScale.x));
//	window->framebufferHeight((unsigned)round(float(window->height()) * framebufferScale.y));

//	window->calculateFramebufferSize();
}

void GLFWContentScaleCallback(GLFWwindow* glfwWindow, float xScale, float yScale) {
	A3D_LOG_D("xScale: {}, yScale: {}", xScale, yScale);

//	auto* window = (Window*)glfwGetWindowUserPointer(glfwWindow);
//	window->calculateFramebufferSize();
}

void GLFWErrorCallback(int error, const char* description) {
	A3D_LOG_E("error: {}, description: {}", error, description);
}
