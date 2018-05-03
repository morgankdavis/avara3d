//
//  Window.cpp
//	avara-engine
//
//  Created by Morgan Davis on 10/21/16.
//  Copyright © 2016 Morgan K Davis. All rights reserved.
//

#ifdef DESKTOP


#include "Window.h"

#include <algorithm>
#include <iostream>

#include <GLFW/glfw3.h>
#ifdef MACOS
#define GLFW_EXPOSE_NATIVE_COCOA
#endif
#include <GLFW/glfw3native.h>
#include <glm/gtc/matrix_transform.hpp>
#include "stb_image_resize.h"

#include "Camera.h"
#include "Color.h"
#include "Exception.h"
#include "Geometry.h"
#include "Global.h"
#include "Image.h"
#include "InputManager.h"
#include "Logger.h"
#include "Node.h"
#include "PhysicsWorld.h"
#include "Renderer.h"
#include "Scene.h"
#include "Utilities.h"


using namespace std;
using namespace ae;
using namespace glm;
using namespace utils;


/***************************************************************************************
     Internal (Global!) Members
 ***************************************************************************************/

Window* 		i_window;
// remove this!
// https://embeddedartistry.com/blog/2017/7/10/using-a-c-objects-member-function-with-c-style-callbacks

/***************************************************************************************
     GLFW Callbacks
 ***************************************************************************************/

void glfwErrorCallback(int error, const char* description) {
	AE_LOG->error("glfwErrorCallback(): error: {}, description: {}", error, description);
	
	g_glfwLastErrorCode = error;
	if (g_glfwLastErrorDescription) {
		free(g_glfwLastErrorDescription);
	}
	g_glfwLastErrorDescription = (char *)malloc(strlen(description));
	strcpy(g_glfwLastErrorDescription, description);
}

void glfwWindowSizeCallback(GLFWwindow* glfwWindow, int aWidth, int aHeight) {
	AE_LOG->trace("glfwWindowSizeCallback()");
	
	i_window->width(aWidth);
	i_window->height(aHeight);
}

void glfwFramebufferSizeCallback(GLFWwindow* glfwWindow, int aWidth, int aHeight) {
	AE_LOG->trace("glfwFramebufferSizeCallback()");
	
	i_window->framebufferWidth(i_window->width() * i_window->framebufferScale());
	i_window->framebufferHeight(i_window->height() * i_window->framebufferScale());
}

/**************************************************************************************
     Static Prototypes
 **************************************************************************************/

static bool InitializeGLFW();
static float ScreenScaleFactor(GLFWmonitor* monitor);

/***************************************************************************************
     Lifescycle
 ***************************************************************************************/

Window::Window(shared_ptr<Renderer> renderer,
			   bool fullScreen,
			   unsigned width, unsigned height,
			   bool useHighDPI, ANTIALIASING_MODE antialiasingMode):
	RenderContext(renderer) {

	if (initLog() != 0) { cout << "Error initializing log." << endl; }
	if (!InitializeGLFW()) { AE_LOG->critical("Error initializing GLFW."); }
	
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_SAMPLES, antialiasingMode);
	glfwWindowHint(GLFW_SAMPLES, (unsigned)antialiasingMode);
				   //static_cast<underlying_type<ANTIALIASING_MODE>::type>(ANTIALIASING_MODE::NONE));
	
//		glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
//		glfwWindowHint(GLFW_VISIBLE, GL_FALSE);

	int viewportWidth = width;
	int viewportHeight = height;

	float scaleFactor = 1.0;
	
	if (fullScreen) {
		GLFWmonitor* monitor = glfwGetPrimaryMonitor();
		const GLFWvidmode* vmode = glfwGetVideoMode(monitor);
		m_glfwWindow = glfwCreateWindow(vmode->width, vmode->height, "avara-engine", monitor, NULL);
		//i_glfwWindow = glfwCreateWindow(vmode->width, vmode->height, "avara-engine", monitor, NULL);
		viewportWidth = vmode->width;
		viewportHeight = vmode->height;
		
		scaleFactor = ScreenScaleFactor(monitor);
	}
	else {
		m_glfwWindow = glfwCreateWindow(width, height, "avara-engine", NULL, NULL);
		
		// TODO: This is HACK. It looks like i_glfwWindow doesn't have a GLFWmonitor at this point
		// causing a segfault.  So we'll cheat and use the main monitor (probably the right one anyway)
		//scaleFactor = GetScreenScaleFactor(glfwGetWindowMonitor(i_glfwWindow));
		scaleFactor = ScreenScaleFactor(glfwGetPrimaryMonitor());
	}

	AE_LOG->info("scaleFactor: {}", scaleFactor);

	if (!m_glfwWindow) {
		AE_LOG->critical("Error creating glfwWindow: {}, {}", g_glfwLastErrorCode, g_glfwLastErrorDescription);
		glfwTerminate();
	}
	
	glfwMakeContextCurrent(m_glfwWindow);
	enableVSync(false);

	RenderContext::renderer()->initialize();

	i_window = this;

	m_width = viewportWidth;
	m_height = viewportHeight;

	m_framebufferScale = (useHighDPI ? scaleFactor : 1.0);
	m_framebufferWidth = m_width * m_framebufferScale;
	m_framebufferHeight = m_height * m_framebufferScale;

	m_antialiasingMode = antialiasingMode;
	m_inputManager = nullptr;

	m_recordingGIF = false;
	m_cursorCaptured = false;
	m_recordingGIF = false;
	m_gifRecordingWidth = 0;
	m_gifRecordingHeight = 0;
	m_gifRecordingMaxFramerate = 0;
	m_gifRecordedFrames = 0;
}

Window::~Window() {
	glfwTerminate();
}

/***************************************************************************************
     Public
 ***************************************************************************************/

void Window::display() {
	AE_LOG->info("Window::display()");
	
	if (m_scene) {
		glfwMakeContextCurrent(m_glfwWindow);
		
		glfwSetWindowSizeCallback(m_glfwWindow, glfwWindowSizeCallback);
		glfwSetFramebufferSizeCallback(m_glfwWindow, glfwFramebufferSizeCallback);
		
		while (!glfwWindowShouldClose(m_glfwWindow)) {
			drawLoop();
		}
		
		stopGIFRecording();
	}
	else {
		throw Exception("Window has no scene.");
	}
}

bool Window::cursorCaptured() const {
	return m_cursorCaptured;
}

void Window::captureCursor(bool captured) {
	m_cursorCaptured = captured;
	glfwSetInputMode(m_glfwWindow, GLFW_CURSOR, (captured ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL));
}

void Window::setShouldClose() {
	glfwSetWindowShouldClose(m_glfwWindow, true);
}

/***************************************************************************************
     Internal
 ***************************************************************************************/

GLFWwindow* Window::glfwWindow() const {
	return m_glfwWindow;
}

/**************************************************************************************
     Renderer
 ***************************************************************************************/

void Window::enableVSync(bool enabled) {
	RenderContext::enableVSync(enabled);
	
	if (!enabled) glfwSwapInterval(0);
	else glfwSwapInterval(1);
}

void Window::debugOptions(DEBUG_OPTIONS options) {
	RenderContext::debugOptions(options);
	
#warning Refactor this
	if (m_scene && m_scene->physicsWorld()) {
		m_scene->physicsWorld()->debugOptions(m_debugOptions);
	}
}

shared_ptr<InputManager> Window::inputManager() {
#warning Refactor this (DesktopInputManager?)
	
	if (m_inputManager == nullptr) {
		m_inputManager = make_shared<InputManager>(this);
	}
	return m_inputManager;
}

float Window::sceneTime() const {
	return glfwGetTime();
}

/***************************************************************************************
     Private
 ***************************************************************************************/

void Window::drawLoop() {
	
	AE_LOG->trace("-------------------------------------------------------------------------------");\
	
	m_renderer->beginFrame(*this);
	
#warning move to saveGIFFrame()
	float time = sceneTime();
	static double previousSeconds = time;
	float deltaSeconds = time - previousSeconds;
	previousSeconds = time;

	if (RenderContext::updateCallback()) RenderContext::updateCallback()(*this, sceneTime());

	auto pov = pointOfView();
	float aspectRatio = (float)m_framebufferWidth/(float)m_framebufferHeight;
	pov->camera()->aspectRatio(aspectRatio);
	
	m_renderer->renderStats().cameraPosition = pov->position();
	
	// simulate physics
	auto physicsWorld = m_scene->physicsWorld();
	if (physicsWorld) {
		physicsWorld->step();
		
		if (didSimulatePhysicsCallback()) {
			didSimulatePhysicsCallback()(*this, sceneTime());
		}
	}

	if (RenderContext::willRenderCallback()) RenderContext::willRenderCallback()(*this, sceneTime());
	
	m_scene->draw(*RenderContext::renderer(),
				  m_framebufferWidth, m_framebufferHeight,
				  *pov,
				  m_debugOptions, m_renderer->renderStats());

	m_renderer->endFrame(*this);
	
	glfwSwapBuffers(m_glfwWindow);

	if (m_recordingGIF) saveGIFFrame(deltaSeconds);
	
	if (RenderContext::didRenderCallback()) RenderContext::didRenderCallback()(*this, sceneTime());
	
	glfwPollEvents();
	if (inputManager()) inputManager()->update();
}

/**************************************************************************************
     Static
 **************************************************************************************/

static bool InitializeGLFW() {
	static bool initialized = false;
	
	if (!initialized) {
		AE_LOG->trace("InitializeGLFW()");
		
		int glfwMajVers, glfwMinVers, glfwRev;
		glfwGetVersion(&glfwMajVers, &glfwMinVers, &glfwRev);
		AE_LOG->info("Starting GLFW version {}.{}.{}", glfwMajVers, glfwMinVers, glfwRev);
		
		glfwSetErrorCallback(glfwErrorCallback);
		
		if (glfwInit()) {
			AE_LOG->info("GLFW Initialized.");
		}
		else {
			AE_LOG->critical("Error initializing GLFW.");
			return false;
		}
		
		srand(time(NULL)); // where else can we put this?
		
		initialized = true;
	}
	
	return true;
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
