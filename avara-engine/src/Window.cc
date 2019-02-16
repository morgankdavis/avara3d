//
//  Window.cc
//	avara-engine
//
//  Created by Morgan Davis on 10/21/16.
//  Copyright © 2016 Morgan K Davis. All rights reserved.
//

#ifdef DESKTOP


#include "Window.h"

#include <iostream>

#define GLEW_STATIC // added for MinGW build... needed?
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#ifdef MACOS
#define GLFW_EXPOSE_NATIVE_COCOA
#endif
#include <GLFW/glfw3native.h>

#include "Camera.h"
#include "WindowInputManager.h"
#include "Global.h"
#include "Logger.h"
#include "Node.h"
#include "PhysicsWorld.h"
#include "Renderer.h"
#include "Scene.h"


using namespace std;
using namespace ae;


/*********************************************************************************************
     Static Prototypes
 *********************************************************************************************/

static bool InitializeGLFW();
static bool InitializeGLEW();
static float ScreenScaleFactor(GLFWmonitor* monitor);

/*********************************************************************************************
     Lifescycle
 *********************************************************************************************/

Window::Window(bool fullScreen,
			   unsigned width, unsigned height,
			   bool enableHighDPI,
			   ANTIALIASING_MODE antialiasingMode,
			   RENDER_API renderAPI):
	RenderContext(renderAPI),
	m_inputManager(nullptr),
	m_cursorCaptured(false) {

		if (!InitializeGLFW()) { AE_LOG_C("Failed to initializing GLFW."); }

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_SAMPLES, static_cast<underlying_type<ANTIALIASING_MODE>::type>(antialiasingMode));
        
		
#warning THIS
//        glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER, (enableHighDPI ? GLFW_TRUE : GLFW_FALSE));

        
		int viewportWidth = width;
		int viewportHeight = height;

		float scaleFactor = 1.0;
		
		if (fullScreen) {
			GLFWmonitor* monitor = glfwGetPrimaryMonitor();
			const GLFWvidmode* vmode = glfwGetVideoMode(monitor);
			m_glfwWindow = glfwCreateWindow(vmode->width, vmode->height, "avara-engine", monitor, NULL);
			viewportWidth = vmode->width;
			viewportHeight = vmode->height;
			
			scaleFactor = ScreenScaleFactor(monitor);
		}
		else {
			m_glfwWindow = glfwCreateWindow(width, height, "avara-engine", nullptr, nullptr);
			
			// TODO: This is HACK. It looks like i_glfwWindow doesn't have a GLFWmonitor at this point
			// causing a segfault.  So we'll cheat and use the main monitor (probably the right one anyway)
			scaleFactor = ScreenScaleFactor(glfwGetPrimaryMonitor());
		}

		if (!m_glfwWindow) {
			AE_LOG_C("Couldn't create GLFW Window.");
			glfwTerminate();
		}
		
		glfwSetWindowUserPointer(m_glfwWindow, (void*)this);
		
		glfwMakeContextCurrent(m_glfwWindow);
		enableVSync(false);

		if (!InitializeGLEW()) { AE_LOG_C("Failed to initialize GLEW."); }
		RenderContext::renderer()->initialize(*this);

		m_width = viewportWidth;
		m_height = viewportHeight;

		m_framebufferScale = (enableHighDPI ? scaleFactor : 1.0); //m_framebufferScale = 1;
		m_framebufferWidth = m_width * m_framebufferScale;
		m_framebufferHeight = m_height * m_framebufferScale;
}

Window::~Window() {
	AE_LOG_D("Destroying Window {:p}", (void*)this);
	
	glfwTerminate();
}

/*********************************************************************************************
     Public
 *********************************************************************************************/

void Window::display() {
	AE_LOG_I("Window::display()");
	
	if (m_scene) {
		glfwMakeContextCurrent(m_glfwWindow);
		
		glfwSetWindowSizeCallback(m_glfwWindow, Window::glfwWindowSizeCallback);
		glfwSetFramebufferSizeCallback(m_glfwWindow, Window::glfwFramebufferSizeCallback);
		
        captureCursor(cursorCaptured()); // needs to be set after windows is made current
        
		while (!glfwWindowShouldClose(m_glfwWindow)) {
			update();
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

/*********************************************************************************************
     Internal
 *********************************************************************************************/

GLFWwindow* Window::glfwWindow() const {
	return m_glfwWindow;
}

/*********************************************************************************************
     RenderContext
 *********************************************************************************************/

void Window::update() {
	RenderContext::update();

	AE_LOG_T("-------------------------------------------------------------------------------");

	if (updateCallback()) {
		(updateCallback())(*this, sceneTime());
	}
	
	m_renderer->beginFrame(*this);
	
	auto pov = pointOfView();
	float aspectRatio = (float)m_framebufferWidth/(float)m_framebufferHeight;
	pov->camera()->aspectRatio(aspectRatio);
	
	m_renderer->renderStats().cameraPosition = pov->position();

	if (willRenderCallback()) {
		(willRenderCallback())(*this, sceneTime());
	}
	
	m_scene->draw(*RenderContext::renderer(),
				  m_framebufferWidth, m_framebufferHeight,
				  *pov,
				  m_debugOptions, m_renderer->renderStats());
	
	m_renderer->endFrame(*this);
	
	glfwSwapBuffers(m_glfwWindow);
	
	if (m_recordingGIF) saveGIFFrame(sceneTime());
	
	if (didRenderCallback()) {
		(didRenderCallback())(*this, sceneTime());
	}
	
	glfwPollEvents();

	if (m_inputManager) {
		static_pointer_cast<WindowInputManager>(m_inputManager)->update();
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
//	if (m_scene && m_scene->physicsWorld()) {
//		m_scene->physicsWorld()->debugOptions(m_debugOptions);
//	}
}

shared_ptr<InputManager> Window::inputManager() {
	if (m_inputManager == nullptr) {
		shared_ptr<Window> window = static_pointer_cast<Window>(shared_from_this());
		auto inputManager = make_shared<WindowInputManager>(window);
		m_inputManager = static_pointer_cast<InputManager>(inputManager);
	}
	return m_inputManager;
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
		if (glewInit() != GLEW_OK) return false;
		
		const GLubyte *renderer = glGetString(GL_RENDERER);
		const GLubyte *version = glGetString(GL_VERSION);
		
		AE_LOG_I("Renderer: {}", renderer);
		AE_LOG_I("Version: {}", version);
		
		GLint numExtensions;
		glGetIntegerv(GL_NUM_EXTENSIONS, &numExtensions);
		AE_LOG_I("Extensions:\n");
		for (GLint e=0 ; e<numExtensions ; ++e) {
			AE_LOG_I("{}", glGetStringi(GL_EXTENSIONS, e));
		}
		
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
