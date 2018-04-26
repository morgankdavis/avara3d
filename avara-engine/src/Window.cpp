//
//  Window.cpp
//	avara-engine
//
//  Created by Morgan Davis on 10/21/16.
//  Copyright © 2016 Morgan K Davis. All rights reserved.
//

#include "Window.h"

#include <algorithm>
#include <iostream>


#include <GL/glew.h> // include before anything that might include GL/gl.h...
#include "fontstash.h"
//#include "gif.h"
#include "gl3fontstash.h"
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#define STB_IMAGE_RESIZE_IMPLEMENTATION
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
#include "Scene.h"
#include "Utilities.h"


//#include <chrono>


using namespace std;
using namespace ae;
using namespace glm;
using namespace utils;


/***************************************************************************************
     Internal Members
 ***************************************************************************************/

Window* 		i_window;
GLFWwindow* 	i_glfwWindow;
//GifWriter* 		i_gifWriter;

/***************************************************************************************
     GLFW Callbacks
 ***************************************************************************************/

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

/***************************************************************************************
     Lifescycle
 ***************************************************************************************/

Window::Window(bool fullScreen, unsigned width, unsigned height,
			   bool useHighDPI, ANTIALIASING_MODE antialiasingMode):
	RenderContext() {

	if (initLog() != 0) { cout << "Error initializing log." << endl; }
	if (initGLFW() != 0) { AE_LOG->critical("Error initializing GLFW."); }
	
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
		i_glfwWindow = glfwCreateWindow(vmode->width, vmode->height, "avara-engine", monitor, NULL);
		viewportWidth = vmode->width;
		viewportHeight = vmode->height;
		
		scaleFactor = GetScreenScaleFactor(monitor);
	}
	else {
		i_glfwWindow = glfwCreateWindow(width, height, "avara-engine", NULL, NULL);
		
		// TODO: This is HACK. It looks like i_glfwWindow doesn't have a GLFWmonitor at this point
		// causing a segfault.  So we'll cheat and use the main monitor (probably the right one anyway)
		//scaleFactor = GetScreenScaleFactor(glfwGetWindowMonitor(i_glfwWindow));
		scaleFactor = GetScreenScaleFactor(glfwGetPrimaryMonitor());
	}

	AE_LOG->info("scaleFactor: {}", scaleFactor);

	if (!i_glfwWindow) {
		AE_LOG->critical("Error creating glfwWindow: {}, {}", g_glfwLastErrorCode, g_glfwLastErrorDescription);
		glfwTerminate();
	}
	
	glfwMakeContextCurrent(i_glfwWindow);
	enableVSync(false);

	initGLEW();

	i_window = this;

	//initFontstash();
	
	m_width = viewportWidth;
	m_height = viewportHeight;

	m_framebufferScale = (useHighDPI ? scaleFactor : 1.0);
	m_framebufferWidth = m_width * m_framebufferScale;
	m_framebufferHeight = m_height * m_framebufferScale;

	m_antialiasingMode = antialiasingMode;
	m_backgroundColor = nullptr;
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
		glfwMakeContextCurrent(i_glfwWindow);
		
		glfwSetWindowSizeCallback(i_glfwWindow, glfwWindowSizeCallback);
		glfwSetFramebufferSizeCallback(i_glfwWindow, glfwFramebufferSizeCallback);
		
		while (!glfwWindowShouldClose(i_glfwWindow)) {
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
	glfwSetInputMode(i_glfwWindow, GLFW_CURSOR, (captured ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL));
}

shared_ptr<InputManager> Window::inputManager() {
#warning Refactor this (DesktopInputManager?)
	
	if (m_inputManager == nullptr) {
		m_inputManager = make_shared<InputManager>(this);
	}
	return m_inputManager;
}

/***************************************************************************************
     Internal
 ***************************************************************************************/

GLFWwindow* Window::glfwWindow() const {
	return i_glfwWindow;
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

/***************************************************************************************
     Private
 ***************************************************************************************/

//void Window::initFontstash() {
//
//	m_fonsContext = gl3fonsCreate(512, 512, FONS_ZERO_TOPLEFT);
//	if (m_fonsContext == NULL) {
//		//AE_LOG->error("Error creating Font Stash context.");
//		throw Exception("Error creating Font Stash context.");
//	}
//	
//	string fontName = "SourceCodePro-Semibold";
//	string fontType = "otf";
//	auto fontPath = FontPath(fontName, fontType);
//	
//	if (fontPath) {
//		m_fonsFont = fonsAddFont(m_fonsContext, fontName.c_str(), fontPath->string().c_str());
//		if (m_fonsFont == FONS_INVALID) {
//			char errStr[1024];
//			sprintf(errStr, "Could not load font: %s\n", fontPath->string().c_str());
//			throw Exception(errStr);
//			//AE_LOG->error("Could not load font: {}", fontPath);
//			
//		}
//	}
//	else {
//		char errStr[1024];
//		sprintf(errStr, "Could not find font: %s\n", fontPath->string().c_str());
//		throw Exception(errStr);
//	}
//}

void Window::drawLoop() {
	
	AE_LOG->trace("-------------------------------------------------------------------------------");
	
	float time = glfwGetTime();
	static double previousSeconds = time;
	float deltaSeconds = time - previousSeconds;
	previousSeconds = time;

	if (RenderContext::updateCallback()) RenderContext::updateCallback()(*this, glfwGetTime());

	auto pov = pointOfView();
	float aspectRatio = (float)m_framebufferWidth/(float)m_framebufferHeight;
	pov->camera()->aspectRatio(aspectRatio);
	
	RenderStats stats = {};
	//stats.cameraPosition = pov->worldPosition();
	stats.cameraPosition = pov->position();
	
	// simulate physics
	auto physicsWorld = m_scene->physicsWorld();
	if (physicsWorld) {
		physicsWorld->step();
		
		if (didSimulatePhysicsCallback()) {
			didSimulatePhysicsCallback()(*this, glfwGetTime());
		}
	}
	
	CheckGLError();
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
	glViewport(0, 0, m_framebufferWidth, m_framebufferHeight);
	
	if (RenderContext::willRenderCallback()) RenderContext::willRenderCallback()(*this, glfwGetTime());
	
	m_scene->draw(pov, m_debugOptions, stats);
	
	//if (DEBUG_OPTIONS_CONTAINS(debugOptions(), DEBUG_OPTIONS::SHOW_STATS_OVERLAY)) updateStatsOverlay(stats);
	//if ((unsigned)m_debugOptions & (unsigned)DEBUG_OPTIONS::SHOW_STATS_OVERLAY) updateStatsOverlay(stats);
	
	glfwSwapBuffers(i_glfwWindow);

	if (m_recordingGIF) saveGIFFrame(deltaSeconds);
	
	if (RenderContext::didRenderCallback()) RenderContext::didRenderCallback()(*this, glfwGetTime());
	
	glfwPollEvents();
	if (inputManager()) inputManager()->update();
}

//void Window::updateStatsOverlay(RenderStats& stats) {
//	
//	static float fps = 0.0;
//	static float ms = 0.0;
//	static float percent = 0.0;
//
//	const float GOAL_TIME = 16.6666667f;
//
//	static unsigned elapsedFrames = 0; ++elapsedFrames;
//	static float previousSeconds = glfwGetTime();
//	float currentSeconds = glfwGetTime();
//	float elapsedSeconds = currentSeconds - previousSeconds;
//
//	if (elapsedSeconds > 0.5) {
//		// only update the framerate stats every so often so they're readable
//		
//		ms = ((elapsedSeconds*1000.0) / elapsedFrames);
//		fps = elapsedFrames/elapsedSeconds;
//		percent = (ms / GOAL_TIME) * 100.0f;
//
//		// reset framerate stats
//		previousSeconds = currentSeconds;
//		elapsedFrames = 0;
//	}
//
//	gl3fonsProjectionSize(m_fonsContext, m_framebufferWidth, m_framebufferHeight);
//	
//	glDisable(GL_DEPTH_TEST);
//	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
//	glEnable(GL_BLEND);
//	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//
//	
//	float dx = 12.0 * m_framebufferScale;
//	float dy = 20.0 * m_framebufferScale;
//	
//	fonsClearState(m_fonsContext);
//	
//	fonsSetFont(m_fonsContext, m_fonsFont);
//
//	static float textSize = 14.0 * m_framebufferScale;
//	static float hPadding = 0.0 * m_framebufferScale;
//	
//	char tmpStr[256];
//	
//	sprintf(tmpStr, "%-14s %.1f%s", "framerate", fps, (m_vSyncEnabled ? " [vsync]" : ""));
//	drawText(tmpStr, textSize, dx, dy);
//	dy += (textSize + hPadding);
//	
//	sprintf(tmpStr, "%-14s %.1f", "frametime", ms);
//	drawText(tmpStr, textSize, dx, dy);
//	dy += (textSize + hPadding);
//	
//	sprintf(tmpStr, "%-14s %.1f", "percent", percent);
//	drawText(tmpStr, textSize, dx, dy);
//	dy += (textSize + hPadding);
//	
//	dy += textSize; // skip a line
//
//	sprintf(tmpStr, "%-14s %d", "nodes", stats.nodes);
//	drawText(tmpStr, textSize, dx, dy);
//	dy += (textSize + hPadding);
//	
//	sprintf(tmpStr, "%-14s %d", "geometries", stats.geometries);
//	drawText(tmpStr, textSize, dx, dy);
//	dy += (textSize + hPadding);
//	
//	sprintf(tmpStr, "%-14s %d", "meshes", stats.meshes);
//	drawText(tmpStr, textSize, dx, dy);
//	dy += (textSize + hPadding);
//	
//	sprintf(tmpStr, "%-14s %d", "polygons", stats.polygons);
//	drawText(tmpStr, textSize, dx, dy);
//	dy += (textSize + hPadding);
//	
//	sprintf(tmpStr, "%-14s %d", "lights", stats.lights);
//	drawText(tmpStr, textSize, dx, dy);
//	dy += (textSize + hPadding);
//	
//	dy += textSize; // skip a line
//	
//	sprintf(tmpStr, "%-14s %.1f, %.1f, %.1f", "camera pos",
//			stats.cameraPosition.x, stats.cameraPosition.y, stats.cameraPosition.z);
//	drawText(tmpStr, textSize, dx, dy);
//	dy += (textSize + hPadding);
//	
//	if (m_recordingGIF) {
//		dy += textSize; // skip a line
//		
//		sprintf(tmpStr, "%-14s %d" , "RECORDING", m_gifRecordedFrames);
//		drawText(tmpStr, textSize, dx, dy);
//		dy += (textSize + hPadding);
//	}
//}
//
//float Window::drawText(string text, float size, float dx, float dy) {
//	// must setup fons GL state first
//	
//	static unsigned black = gl3fonsRGBA(0, 0, 0, 255);
//	static unsigned white = gl3fonsRGBA(255, 255, 255, 255);
//	
//	fonsSetSize(m_fonsContext, size);
//	
//	fonsSetColor(m_fonsContext, black);
//	fonsSetBlur(m_fonsContext, 1);
//	fonsDrawText(m_fonsContext, dx, dy, text.c_str(), NULL);
//	
//	fonsSetColor(m_fonsContext, white);
//	fonsSetBlur(m_fonsContext, 0);
//	return fonsDrawText(m_fonsContext, dx, dy, text.c_str(), NULL);
//}
