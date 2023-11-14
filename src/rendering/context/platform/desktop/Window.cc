//
//  Window.cc
//	avara-engine
//
//  Created by Morgan Davis on 10/21/16.
//  Copyright © 2016 Morgan K Davis. All rights reserved.
//

#ifdef DESKTOP


#include "rendering/context/platform/desktop/Window.h"

#include <iostream>
#include <sstream>

#define GLEW_STATIC // added for MinGW build... needed?
#include "GL/glew.h"
#include "GLFW/glfw3.h"
#ifdef MACOS
#define GLFW_EXPOSE_NATIVE_COCOA
#endif
#include "GLFW/glfw3native.h"

#include "Global.h"
#include "diagnostic/logging/Logger.h"
#include "input/platform/desktop/WindowInputManager.h"
#include "physics/PhysicsWorld.h"
#include "rendering/Renderer.h"
#include "rendering/camera/Camera.h"
#include "scene/Node.h"
#include "scene/Scene.h"


using namespace std;
using namespace ae;


/*********************************************************************************************
	Static Prototypes
 *********************************************************************************************/

static bool 	InitializeGLFW();
static bool 	InitializeGLEW();
static void 	LogGLInfo();
static float 	ScreenScaleFactor(GLFWmonitor* monitor);

/*********************************************************************************************
	Lifescycle
 *********************************************************************************************/

Window::Window(bool fullScreen,
			   unsigned width, unsigned height,
			   bool enableHighDPI,
			   ANTIALIASING_MODE antialiasingMode,
			   RENDER_API renderAPI):
	RenderContext(renderAPI),
	_inputManager(nullptr),
	_cursorCaptured(false) {

	if (InitializeGLFW()) {
#ifdef GL_FULL
		// TODO: move these version numbers
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_SAMPLES, static_cast<underlying_type<ANTIALIASING_MODE>::type>(antialiasingMode));
#else // OpenGL ES
		glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
#endif


		#warning THIS
			//        glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER, (enableHighDPI ? GLFW_TRUE : GLFW_FALSE));


		int viewportWidth = width;
		int viewportHeight = height;

		float scaleFactor = 1.0;

		if (fullScreen) {
			GLFWmonitor* monitor = glfwGetPrimaryMonitor();
			const GLFWvidmode* vmode = glfwGetVideoMode(monitor);
			_glfwWindow = glfwCreateWindow(vmode->width, vmode->height, "avara-engine", monitor, NULL);
			viewportWidth = vmode->width;
			viewportHeight = vmode->height;

			scaleFactor = ScreenScaleFactor(monitor);
		}
		else {
			_glfwWindow = glfwCreateWindow(width, height, "avara-engine", nullptr, nullptr);

			// TODO: This is HACK. It looks like i_glfwWindow doesn't have a GLFWmonitor at this point
			// causing a segfault.  So we'll cheat and use the main monitor (probably the right one anyway)
			scaleFactor = ScreenScaleFactor(glfwGetPrimaryMonitor());
		}

		if (_glfwWindow) {
			glfwSetWindowUserPointer(_glfwWindow, (void*)this);

			glfwMakeContextCurrent(_glfwWindow);
			enableVSync(false);

			if (InitializeGLEW()) {
				RenderContext::renderer()->initialize(*this);

				_width = viewportWidth;
				_height = viewportHeight;

				_framebufferScale = (enableHighDPI ? scaleFactor : 1.0); //_framebufferScale = 1;
				_framebufferWidth = _width * _framebufferScale;
				_framebufferHeight = _height * _framebufferScale;
			}
			else {
				AE_LOG_C("Failed to initialize GLEW.");
				glfwTerminate();
				// exception
			}
		}
		else {
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
	AE_LOG_D("Destroying Window {:p}", (void*)this);

	// TODO: must move to support multiple windows
	glfwSetErrorCallback(NULL);
	glfwTerminate();
}

/*********************************************************************************************
	Public
 *********************************************************************************************/

void Window::display() {
	AE_LOG_D("display()");
	
	if (_scene) {
		glfwMakeContextCurrent(_glfwWindow);
		
		glfwSetWindowSizeCallback(_glfwWindow, Window::glfwWindowSizeCallback);
		glfwSetFramebufferSizeCallback(_glfwWindow, Window::glfwFramebufferSizeCallback);
		
        captureCursor(cursorCaptured()); // needs to be set after windows is made current
        
		while (!glfwWindowShouldClose(_glfwWindow)) {
			update();
		}
		
		stopGIFRecording();
	}
	else {
		throw Exception("Window has no scene.");
	}
}

bool Window::cursorCaptured() const {
	return _cursorCaptured;
}

void Window::captureCursor(bool captured) {
	_cursorCaptured = captured;
	glfwSetInputMode(_glfwWindow, GLFW_CURSOR, (captured ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL));
}

void Window::setShouldClose() {
	glfwSetWindowShouldClose(_glfwWindow, true);
}

/*********************************************************************************************
	Internal
 *********************************************************************************************/

GLFWwindow* Window::glfwWindow() const {
	return _glfwWindow;
}

/*********************************************************************************************
	RenderContext
 *********************************************************************************************/

//void Window::update() {
//	//RenderContext::update();
//
//	AE_LOG_T("-------------------------------------------------------------------------------");
//
//	if (updateCallback()) {
//		(updateCallback())(*this, sceneTime());
//	}
//
//	_renderer->beginFrame(*this);
//
//	auto pov = pointOfView();
//	float aspectRatio = (float)_framebufferWidth/(float)_framebufferHeight;
//	pov->camera()->aspectRatio(aspectRatio);
//
//	_renderer->renderStats().cameraPosition = pov->position();
//
//	if (willRenderCallback()) {
//		(willRenderCallback())(*this, sceneTime());
//	}
//
//	_scene->update(*RenderContext::renderer(),
//				  _framebufferWidth, _framebufferHeight,
//				  *pov,
//				  _debugOptions, _renderer->renderStats());
//
//	_renderer->endFrame(*this);
//
//	glfwSwapBuffers(_glfwWindow);
//
//	if (_recordingGIF) saveGIFFrame(sceneTime());
//
//	if (didRenderCallback()) {
//		(didRenderCallback())(*this, sceneTime());
//	}
//
//	glfwPollEvents();
//
//	if (_inputManager) {
//		static_pointer_cast<WindowInputManager>(_inputManager)->update();
//	}
//}

void Window::swapBuffers() {
	glfwSwapBuffers(_glfwWindow);
}

void Window::pollInput() {
	glfwPollEvents();

	if (_inputManager) {
		static_pointer_cast<WindowInputManager>(_inputManager)->update();
	}
}

void Window::enableVSync(bool enabled) {
	RenderContext::enableVSync(enabled);
	
	if (!enabled) glfwSwapInterval(0);
	else glfwSwapInterval(1);
}

void Window::debugOptions(DEBUG_OPTIONS options) {
	RenderContext::debugOptions(options);
	
//#warning Refactor this
//	if (_scene && _scene->physicsWorld()) {
//		_scene->physicsWorld()->debugOptions(_debugOptions);
//	}
}

shared_ptr<InputManager> Window::inputManager() {
	if (_inputManager == nullptr) {
		shared_ptr<Window> window = static_pointer_cast<Window>(shared_from_this());
		auto inputManager = make_shared<WindowInputManager>(window);
		_inputManager = static_pointer_cast<InputManager>(inputManager);
	}
	return _inputManager;
}

float Window::sceneTime() const {
	return glfwGetTime();
}

/*********************************************************************************************
	GLFW Callbacks
 *********************************************************************************************/

void Window::glfwWindowSizeCallback(GLFWwindow* glfwWindow, int aWidth, int aHeight) {
	AE_LOG_T("glfwWindowSizeCallback()");
	
	Window* window = (Window*)glfwGetWindowUserPointer(glfwWindow);
	
	window->width(aWidth);
	window->height(aHeight);
}

void Window::glfwFramebufferSizeCallback(GLFWwindow* glfwWindow, int aWidth, int aHeight) {
	AE_LOG_T("glfwFramebufferSizeCallback()");
	
	Window* window = (Window*)glfwGetWindowUserPointer(glfwWindow);
	
	window->framebufferWidth(window->width() * window->framebufferScale());
	window->framebufferHeight(window->height() * window->framebufferScale());
}

void Window::glfwErrorCallback(int error, const char* description) {
	AE_LOG_E("glfwErrorCallback(): error: {}, description: {}", error, description);
}

/*********************************************************************************************
	Static
 *********************************************************************************************/

static bool InitializeGLFW() {
	
	static bool initialized = false;
	if (!initialized) {
		AE_LOG_T("InitializeGLFW()");
		
		int glfwMajVers, glfwMinVers, glfwRev;
		glfwGetVersion(&glfwMajVers, &glfwMinVers, &glfwRev);
		AE_LOG_I("Starting GLFW version {}.{}.{}", glfwMajVers, glfwMinVers, glfwRev);

		// TODO: must move to support multiple windows
		glfwSetErrorCallback(Window::glfwErrorCallback);
		
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

static bool InitializeGLEW() {

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

	AE_LOG_I("Renderer: {}", renderer);
	AE_LOG_I("Version: {}", version);

	// extensions

	GLint numExtensions;
	glGetIntegerv(GL_NUM_EXTENSIONS, &numExtensions);
	ostringstream extensionsStream;
	extensionsStream << "Extensions:" << endl;
	for (GLint e=0; e < numExtensions; ++e) {
		extensionsStream << "\t" << glGetStringi(GL_EXTENSIONS, e);
		if (e < numExtensions-1) extensionsStream << endl;
	}
	AE_LOG_I(extensionsStream.str());

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

	AE_LOG_I(contextParamsStream.str());
}

static float ScreenScaleFactor(GLFWmonitor* monitor) {
#ifdef MACOS
	//GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	//GLFWmonitor* monitor = glfwGetWindowMonitor(glfwWindow);
	CGDirectDisplayID cgDisplayID = glfwGetCocoaMonitor(monitor);
	CGDisplayModeRef currentModeRef = CGDisplayCopyDisplayMode(cgDisplayID);
	
	Size width = CGDisplayModeGetWidth(currentModeRef);
	Size pixelWidth = CGDisplayModeGetPixelWidth(currentModeRef);
	return (float)pixelWidth / (float)width;
#endif
	return 1.0;
}

#endif // DESKTOP
