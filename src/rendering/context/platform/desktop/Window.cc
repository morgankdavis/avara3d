//
//  Window.cc
//	avara-engine
//
//  Created by Morgan Davis on 10/21/16.
//  Copyright © 2016 Morgan K Davis. All rights reserved.
//

#ifdef DESKTOP


#include "ae/rendering/context/platform/desktop/Window.h"

#include <iostream>
#include <sstream>

//#define GLEW_STATIC // added for MinGW build... needed?
#include "GL/glew.h"
#include "GLFW/glfw3.h"

#include "ae/diagnostic/exceptions/Exception.h"
#include "ae/diagnostic/logging/Logger.h"
#include "ae/input/platform/desktop/WindowInputManager.h"
#include "ae/physics/PhysicalWorld.h"
#include "ae/rendering/Renderer.h"
#include "ae/rendering/VisualWorld.h"
#include "ae/rendering/camera/Camera.h"
#include "ae/scene/Node.h"
#include "ae/scene/Scene.h"


using namespace std;
using namespace ae;


/*********************************************************************************************
	Static Prototypes
 *********************************************************************************************/

static bool 	InitGLFW();
static bool 	InitGLEW();
static void 	LogGLInfo();
//static float 	ScreenScaleFactor(GLFWmonitor* monitor);
static void 	GLFWWindowSizeCallback(GLFWwindow* glfwWindow,
									  int width,
									  int height);
static void		GLFWWindowCloseCallback(GLFWwindow* glfwWindow);
static void		GLFWFramebufferSizeCallback(GLFWwindow* glfwWindow,
											   int width,
											   int height);
static void 	GLFWErrorCallback(int error,
								 const char* description);

/*********************************************************************************************
	Lifescycle
 *********************************************************************************************/

Window::Window(RenderApi renderAPI,
			   bool fullScreen,
			   unsigned width,
			   unsigned height,
			   bool enableHighDPI,
			   AntialiasingMode antialiasingMode):
		RenderContext(renderAPI),
		_glfwWindow(nullptr),
		_cursorCaptured(false) {

	if (InitGLFW()) {
#ifdef OPENGL_DESKTOP
		// TODO: move these version numbers
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_SAMPLES, static_cast<underlying_type<AntialiasingMode>::type>(antialiasingMode));
#else // OpenGL ES
		glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
#endif

		// glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER, (enableHighDPI ? GLFW_TRUE : GLFW_FALSE));

		auto viewportWidth = width;
		auto viewportHeight = height;

		if (fullScreen) {
			auto monitor = glfwGetPrimaryMonitor();
			const GLFWvidmode* vmode = glfwGetVideoMode(monitor);
			_glfwWindow = unique_ptr<GLFWwindow, DestroyGLFWWindow>(glfwCreateWindow(vmode->width,
																					 vmode->height,
																					 "avara-engine",
																					 monitor,
																					 nullptr));
			viewportWidth = vmode->width;
			viewportHeight = vmode->height;

//			scaleFactor = ScreenScaleFactor(monitor);
		}
		else {
			_glfwWindow = unique_ptr<GLFWwindow, DestroyGLFWWindow>(glfwCreateWindow(width,
																					 height,
																					 "avara-engine",
																					 nullptr,
																					 nullptr));


//			scaleFactor = ScreenScaleFactor(glfwGetPrimaryMonitor());
		}

		// TODO: This is HACK. It looks like i_glfwWindow doesn't have a GLFWmonitor at this point
		// causing a segfault.  So we'll cheat and use the main monitor (probably the right one anyway)
		auto monitor = glfwGetPrimaryMonitor();

		if (_glfwWindow) {
			glfwSetWindowUserPointer(_glfwWindow.get(), static_cast<void*>(this));

			glfwMakeContextCurrent(_glfwWindow.get());
			vSyncEnabled(false);

			if (InitGLEW()) {
				RenderContext::renderer()->initialize(*this);

				_width = viewportWidth;
				_height = viewportHeight;

				if (enableHighDPI) {
					float scaleFactorX = 1.0;
					float scaleFactorY = 1.0;
					glfwGetMonitorContentScale(monitor, &scaleFactorX, &scaleFactorY);
					_framebufferScale = {scaleFactorX, scaleFactorY};
				}

				_framebufferWidth = _width * _framebufferScale.x;
				_framebufferHeight = _height * _framebufferScale.y;
			}
			else {
				// TODO: exception
				AE_LOG_C("Failed to initialize GLEW.");
				glfwTerminate();
				// exception
			}
		}
		else {
			// TODO: exception
			AE_LOG_C("Couldn't create GLFW Window.");
			glfwTerminate();
			// exception
		}
	}
	else {
		AE_LOG_C("Failed to initializing GLFW.");
		// exception
	}
}

Window::~Window() {
	AE_LOG_D("Destroying Window {:p}", static_cast<void*>(this));

	close(); // meh?

	// TODO: must move to support multiple windows
	glfwSetErrorCallback(NULL);
	glfwTerminate();
}

/*********************************************************************************************
	Public
 *********************************************************************************************/

void Window::open() {
	AE_LOG_T("");

	if (_visualWorld && _visualWorld->scene()) {
		glfwMakeContextCurrent(_glfwWindow.get());
		
		glfwSetWindowSizeCallback(_glfwWindow.get(), GLFWWindowSizeCallback);
		glfwSetWindowCloseCallback(_glfwWindow.get(), GLFWWindowCloseCallback);
		glfwSetFramebufferSizeCallback(_glfwWindow.get(), GLFWFramebufferSizeCallback);

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

bool Window::cursorCaptured() const {
	return _cursorCaptured;
}

void Window::cursorCaptured(bool captured) {
	_cursorCaptured = captured;
	glfwSetInputMode(_glfwWindow.get(), GLFW_CURSOR, (captured ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL));
}

/*********************************************************************************************
	RenderContext
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

/*********************************************************************************************
	Internal
 *********************************************************************************************/

void Window::pollInput() {
	glfwPollEvents();
}

GLFWwindow* Window::glfwWindow() const {
	return _glfwWindow.get();
}

/*********************************************************************************************
	Internal Static
 *********************************************************************************************/

void Window::Destroy(GLFWwindow* window) {
	glfwDestroyWindow(window);
}

/*********************************************************************************************
	Static
 *********************************************************************************************/

static bool InitGLFW() {
	
	static bool initialized = false;
	if (!initialized) {
		AE_LOG_T("");
		
		int glfwMajVers, glfwMinVers, glfwRev;
		glfwGetVersion(&glfwMajVers, &glfwMinVers, &glfwRev);
		AE_LOG_I("Starting GLFW version {}.{}.{}...", glfwMajVers, glfwMinVers, glfwRev);

		// TODO: must move to support multiple windows
		glfwSetErrorCallback(GLFWErrorCallback);
		
		if (glfwInit()) {
			AE_LOG_I("GLFW Initialized.");
		}
		else {
			AE_LOG_C("Error initializing GLFW.");
			return false;
		}
		
		srand(time(NULL)); // where else can we put this?
		
		initialized = true;
	}
	return true;
}

static bool InitGLEW() {
	AE_LOG_T("");

	// NOTE: OpenGL context must be setup first
	
	static bool initialized = false;
	if (!initialized) {
		glewExperimental = GL_TRUE;

		auto initStatus = glewInit();
		if (initStatus == GLEW_OK) {

			LogGLInfo();
			initialized = true;
		}
		else {
			AE_LOG_C("Failed to initialize GLEW: {}", initStatus);
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
	AE_LOG_I("Renderer: {}", reinterpret_cast<const char*>(renderer));
	AE_LOG_I("Version: {}", reinterpret_cast<const char*>(version));

	// extensions

	GLint numExtensions;
	glGetIntegerv(GL_NUM_EXTENSIONS, &numExtensions);
	ostringstream extensionsStream;
	extensionsStream << "Extensions:" << endl;
	for (GLint e=0; e < numExtensions; ++e) {
		extensionsStream << "\t" << glGetStringi(GL_EXTENSIONS, e);
		if (e < numExtensions-1) extensionsStream << endl;
	}
	AE_LOG_I("{}", extensionsStream.str());

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

	AE_LOG_I("{}", contextParamsStream.str());
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
	AE_LOG_T("width: {}, height: {}", width, height);

	Window* window = (Window*)glfwGetWindowUserPointer(glfwWindow);

	window->width(width);
	window->height(height);
}

void GLFWWindowCloseCallback(GLFWwindow* glfwWindow) {
	AE_LOG_I("glfwWindow: {:p}", static_cast<void*>(glfwWindow));

	Window* window = (Window*)glfwGetWindowUserPointer(glfwWindow);

	window->close();
}

void GLFWFramebufferSizeCallback(GLFWwindow* glfwWindow, int width, int height) {
	AE_LOG_T("width: {}, height: {}", width, height);

	Window* window = (Window*)glfwGetWindowUserPointer(glfwWindow);

	auto framebufferScale = window->framebufferScale();
	window->framebufferWidth(window->width() * framebufferScale.x);
	window->framebufferHeight(window->height() * framebufferScale.y);
}

void GLFWErrorCallback(int error, const char* description) {
	AE_LOG_E("error: {}, description: {}", error, description);
}


#endif // DESKTOP
