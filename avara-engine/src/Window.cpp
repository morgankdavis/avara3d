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
#include "gif.h"
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


using namespace std;
using namespace ae;
using namespace glm;
using namespace utils;


/***************************************************************************************
     MARK:   Internal Members
 **************************************************************************************/

Window* 		i_window;
GLFWwindow* 	i_glfwWindow;
GifWriter* 		i_gifWriter;

/***************************************************************************************
     MARK:   GLFW Callbacks
 **************************************************************************************/

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
     MARK:   Lifescycle
 **************************************************************************************/

Window::Window(bool fullScreen, unsigned width, unsigned height,
			   bool useHighDPI, ANTIALIASING_MODE antialiasingMode) {
//	m_scene(make_shared<Scene>()),
//	m_width(width),
//	m_height(height),
//	m_framebufferScale(framebufferScale),
//	m_framebufferWidth(m_width * m_framebufferScale),
//	m_framebufferHeight(m_height * m_framebufferScale),
//	m_antialiasingMode(AntialiasingMode_None),
//	m_backgroundColor(nullptr),
//	m_pointOfView(nullptr),
//	m_inputManager(nullptr),
//	m_maximumFramerate(60.0),
//	m_willUpdateCallback(nullptr),
//	m_didUpdateCallback(nullptr) {
	
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

	initFontstash();
	
	m_width = viewportWidth;
	m_height = viewportHeight;

	m_framebufferScale = (useHighDPI ? scaleFactor : 1.0);
	m_framebufferWidth = m_width * m_framebufferScale;
	m_framebufferHeight = m_height * m_framebufferScale;

	setupRenderBuffer(); // needs width and height!

	m_antialiasingMode = antialiasingMode;
	m_debugOptions = DEBUG_OPTIONS::NONE;
	m_backgroundColor = nullptr;
	m_pointOfView = nullptr;
	m_inputManager = nullptr;
	m_maximumFramerate = 60.0;
	m_updateCallback = nullptr;
	m_didSimulatePhysicsCallback = nullptr;
	m_willRenderCallback = nullptr;
	m_didRenderCallback = nullptr;
	m_recordingGIF = false;
	m_cursorCaptured = false;
}

Window::~Window() {
	glfwTerminate();
}

/***************************************************************************************
     MARK:   Public
 **************************************************************************************/

void Window::display() {
	AE_LOG->info("Window::display()");
	
	if (m_scene) {
		glfwMakeContextCurrent(i_glfwWindow);
		
		glfwSetWindowSizeCallback(i_glfwWindow, glfwWindowSizeCallback);
		glfwSetFramebufferSizeCallback(i_glfwWindow, glfwFramebufferSizeCallback);
		
		while (!glfwWindowShouldClose(i_glfwWindow)) {
			mainLoop();
		}
		
		stopGIFRecording();
	}
	else {
		throw Exception("Window has no scene.");
	}
}

shared_ptr<Scene> Window::scene() const {
	return m_scene;
}

void Window::scene(const shared_ptr<Scene> scene) {
	scene->attachedToWindow(shared_from_this());
	m_scene = scene;
	
}

bool Window::cursorCaptured() const {
	return m_cursorCaptured;
}

void Window::captureCursor(bool captured) {
	m_cursorCaptured = captured;
	glfwSetInputMode(i_glfwWindow, GLFW_CURSOR, (captured ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL));
}

bool Window::vSyncEnabled() const {
	return m_vSyncEnabled;
}

void Window::enableVSync(bool enabled) {
	m_vSyncEnabled = enabled;
	if (!enabled) glfwSwapInterval(0);
	else glfwSwapInterval(1);
}

float Window::maximumFramerate() const {
	return m_maximumFramerate;
}

void Window::maximumFramerate(float max) {
	m_maximumFramerate = max;
}

DEBUG_OPTIONS Window::debugOptions() const {
	return m_debugOptions;
}

void Window::debugOptions(DEBUG_OPTIONS options) {
	m_debugOptions = options;
	
	if (m_scene && m_scene->physicsWorld()) {
		m_scene->physicsWorld()->debugOptions(m_debugOptions);
	}
}

ANTIALIASING_MODE Window::antialiasingMode() const {
	return m_antialiasingMode;
}

shared_ptr<Node> Window::pointOfView() {
	
	if (m_pointOfView) {
		return m_pointOfView;
	}
	else {
		// try to assign one from the scene
		for (auto node: m_scene->rootNode()->childNodes(true)) {
			if (node->camera()) {
				m_pointOfView = node;
				return m_pointOfView;
			}
		}
	}
	if (!m_pointOfView) {
		// still no POV. add a default one.
		m_pointOfView = defaultPointOfView();
	}
	
	return m_pointOfView;
}

void Window::pointOfView(const shared_ptr<Node> camera) {
	m_pointOfView = camera;
}

shared_ptr<InputManager> Window::inputManager() {
	if (m_inputManager == nullptr) {
		m_inputManager = make_shared<InputManager>(this);
	}
	return m_inputManager;
}

shared_ptr<Image> Window::snapshot() const {
	
	unsigned char *buf = (unsigned char*)malloc(m_framebufferWidth * m_framebufferHeight * 4);
	glReadPixels(0, 0, m_framebufferWidth, m_framebufferHeight, GL_RGBA, GL_UNSIGNED_BYTE, buf);
	auto image = make_shared<Image>(buf, m_framebufferWidth, m_framebufferHeight);
	free(buf);
	return image;
}

bool Window::recordingGIF() const {
	return m_recordingGIF;
}

void Window::startGIFRecording(const boost::filesystem::path& path,
							   unsigned maxHeight, unsigned maxFramerate) {
	if (!m_recordingGIF) {
		AE_LOG->info("Starting GIF recording...");
		
		m_gifRecordingMaxFramerate = maxFramerate;
		
		m_gifRecordingHeight = m_framebufferHeight;
		m_gifRecordingWidth = m_framebufferWidth;
		if (m_gifRecordingHeight > maxHeight) {
			float scale = (float)maxHeight / (float)m_framebufferHeight;
			m_gifRecordingHeight = m_framebufferHeight * scale;
			m_gifRecordingWidth = m_framebufferWidth * scale;
		}
		
		unsigned frameTimeMS = 1000.0 /* (ms/sec) */ / m_gifRecordingMaxFramerate /* (frames/sec) */;
		// -> ms/frame
		unsigned frameTimeHS = frameTimeMS / 10.0; // 100th sec/frame
		
		i_gifWriter = (GifWriter *)malloc(sizeof(GifWriter));
		// gif-h frame time is in 100ths of a second
		GifBegin(i_gifWriter, path.string().c_str(), 
				 m_gifRecordingWidth, m_gifRecordingHeight, 
				 frameTimeHS);
		
		m_recordingGIF = true;
	}
}

void Window::stopGIFRecording() {
	if (m_recordingGIF) {
		m_recordingGIF = false;
		
		GifEnd(i_gifWriter);
		// crashing... but it doesn't look like GifEnd() frees everything,
		// just the main buffer.
		//free(i_gifWriter);
		
		AE_LOG->info("Stopped GIF recording.");
	}
}

//void Window::getVRAMStats(unsigned& total, unsigned& used) {
//	
//#define GL_GPU_MEM_INFO_TOTAL_AVAILABLE_MEM_NVX 0x9048
//#define GL_GPU_MEM_INFO_CURRENT_AVAILABLE_MEM_NVX 0x9049
//
//	GLint total_mem_kb = 0;
//glGetIntegerv(GL_GPU_MEM_INFO_TOTAL_AVAILABLE_MEM_NVX, 
//			  &total_mem_kb);
//
//	GLint cur_avail_mem_kb = 0;
//glGetIntegerv(GL_GPU_MEM_INFO_CURRENT_AVAILABLE_MEM_NVX, 
//			  &cur_avail_mem_kb);
//	
//	
//	
//	cout << "GL_NVX_gpu_memory_info: " << GL_NVX_gpu_memory_info << endl;
//	cout << "GLEW_NVX_gpu_memory_info: " << GLEW_NVX_gpu_memory_info << endl;
//	
//	if (GL_NVX_gpu_memory_info) {
//		cout << "1" << endl;
//		
//		int totalmem = 0;
//		int freemem = 0;
//		glGetIntegerv(GL_GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX, &totalmem);
//		glGetIntegerv(GL_GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX, &freemem);
//		
//		cout << "totalmem: " << totalmem << endl;
//		cout << "freemem: " << freemem << endl;
//	}
//	else {
//		cout << "2" << endl;
//	}
//	
//	if (GLEW_NVX_gpu_memory_info) {
//		cout << "3" << endl;
//	}
//	else {
//		cout << "4" << endl;
//		
//		int totalmem = 0;
//		int freemem = 0;
//		glGetIntegerv(GL_GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX, &totalmem);
//		glGetIntegerv(GL_GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX, &freemem);
//		
//		cout << "totalmem: " << totalmem << endl;
//		cout << "freemem: " << freemem << endl;
//	}
//	
////	GPU_MEMORY_INFO_DEDICATED_VIDMEM_NVX          0x9047
////	GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX    0x9048
////	GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX  0x9049
////	GPU_MEMORY_INFO_EVICTION_COUNT_NVX            0x904A
////	GPU_MEMORY_INFO_EVICTED_MEMORY_NVX            0x904B
//
//	
//}

/***************************************************************************************
     MARK:   Internal
 **************************************************************************************/

unsigned Window::width() const {
	return m_width;
}

void Window::width(unsigned aWidth) {
	m_width = aWidth;
	framebufferWidth(m_width * m_framebufferScale);
}

unsigned Window::height() const {
	return m_height;
}

void Window::height(unsigned aHeight) {
	m_height = aHeight;
	framebufferHeight(m_height * m_framebufferScale);
}

unsigned Window::framebufferScale() const {
	return m_framebufferScale;
}

void Window::framebufferScale(unsigned aScale) {
	m_framebufferScale = aScale;
}

unsigned Window::framebufferWidth() const {
	return m_framebufferWidth;
}

void Window::framebufferWidth(unsigned aWidth) {
	m_framebufferWidth = aWidth;
}

unsigned Window::framebufferHeight() const {
	return m_framebufferHeight;
}

void Window::framebufferHeight(unsigned aHeight) {
	m_framebufferHeight = aHeight;
}

shared_ptr<Node> Window::defaultPointOfView() {
	
	auto cameraNode = make_shared<Node>();
	//m_scene->rootNode()->addChildNode(cameraNode); // done below
	auto camera = make_shared<Camera>();
	camera->name("default camera");
	cameraNode->camera(camera);

	auto boundingPoints = (*scene()->boundingPoints());

	float fovH = cameraNode->camera()->fov();
	float w = width();
	float h = height();
	float aspectRatio = w/h;
	float inverseAspectRatio = 1.0f/aspectRatio;
	float fovV = fovH * inverseAspectRatio;

	// tan(angle) = x/z
	// ztan(angle) = x
	// z = x/tan(angle)

	float maxZ = abs(boundingPoints["zMax"].z);

	float xH = abs(boundingPoints["xMin"].x) + abs(boundingPoints["xMax"].x) / 2.0f;
	float angleH = fovH / 2.0;
	float zH = xH / tan(angleH);

	float xV = abs(boundingPoints["yMin"].y) + abs(boundingPoints["yMax"].y) / 2.0f;
	float angleV = fovV / 2.0;
	float zV = xV / tan(angleV);

	zH += maxZ;
	zV += maxZ;

	float z = fmax(zH, zV);
	float midX = (boundingPoints["xMin"].x + boundingPoints["xMax"].x) / 2.0f;
	float midY = (boundingPoints["yMin"].y + boundingPoints["yMax"].y) / 2.0f;

	vec3 eye = vec3(midX, midY, z / 2.0f); // not sure why z is devided by 2.0, but it seems to work better...

	mat4 viewMat = translate(mat4(1.0f), eye);
	cameraNode->transform(viewMat);

	scene()->rootNode()->addChildNode(cameraNode);
	pointOfView(cameraNode);
	
	return cameraNode;
}

GLFWwindow* Window::glfwWindow() const {
	return i_glfwWindow;
}

WindowUpdateFuction Window::updateCallback() {
	return m_updateCallback;
}

void Window::updateCallback(WindowUpdateFuction function) {
	m_updateCallback = function;
}

WindowDidSimulatePhysicsFuction Window::didSimulatePhysicsCallback() {
	return m_didSimulatePhysicsCallback;
}

void Window::didSimulatePhysicsCallback(WindowDidSimulatePhysicsFuction function) {
	m_didSimulatePhysicsCallback = function;
}

WindowWillRenderFuction Window::willRenderCallback() {
	return m_willRenderCallback;
}

void Window::willRenderCallback(WindowWillRenderFuction function) {
	m_willRenderCallback = function;
}

WindowDidRenderFuction Window::didRenderCallback() {
	return m_didRenderCallback;
}

void Window::didRenderCallback(WindowDidRenderFuction function) {
	m_didRenderCallback = function;
}

/***************************************************************************************
     MARK:   Private
 **************************************************************************************/

void Window::initFontstash() {

	m_fonsContext = gl3fonsCreate(512, 512, FONS_ZERO_TOPLEFT);
	if (m_fonsContext == NULL) {
		//AE_LOG->error("Error creating Font Stash context.");
		throw Exception("Error creating Font Stash context.");
	}
	
	string fontName = "SourceCodePro-Semibold";
	string fontType = "otf";
	auto fontPath = FontPath(fontName, fontType);
	
	if (fontPath) {
		m_fonsFont = fonsAddFont(m_fonsContext, fontName.c_str(), fontPath->string().c_str());
		if (m_fonsFont == FONS_INVALID) {
			char errStr[1024];
			sprintf(errStr, "Could not load font: %s\n", fontPath->string().c_str());
			throw Exception(errStr);
			//AE_LOG->error("Could not load font: {}", fontPath);
			
		}
	}
	else {
		char errStr[1024];
		sprintf(errStr, "Could not find font: %s\n", fontPath->string().c_str());
		throw Exception(errStr);
	}
}

void Window::setupRenderBuffer() {
	
	return;
	
	// http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-14-render-to-texture/

	// render framebuffer
	
	glGenFramebuffers(1, &m_renderFramebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, m_renderFramebuffer);
	
	// render texture
	
	GLuint renderTexture;
	glGenTextures(1, &renderTexture);
	
	glBindTexture(GL_TEXTURE_2D, renderTexture);
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	
	// new from antoin {
//	glBindFramebuffer(GL_FRAMEBUFFER, m_renderFramebuffer);
//	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderTexture, 0);
	// }
	
	// render depth buffer
	
	// ORIGINAL
	GLuint depthRenderBuffer;
	glGenRenderbuffers(1, &depthRenderBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, depthRenderBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, m_width, m_height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderBuffer);

	
	
	// setup draw buffer
	
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, renderTexture, 0);
	GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, drawBuffers);
	
	GLenum fbStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (fbStatus == GL_FRAMEBUFFER_COMPLETE) {
		AE_LOG->info("Render framebuffer created.");
	}
	else {
		//char errMsg[1024];
		//sprintf(errMsg, "Error creating render framebuffer: %s\n", fbStatus);
		AE_LOG->critical("Error creating render framebuffer: {}", fbStatus);
		//throw Exception(errMsg);
	}
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Window::mainLoop() {
	
	AE_LOG->trace("-------------------------------------------------------------------------------");

	float time = glfwGetTime();
	static double previousSeconds = time;
	float deltaSeconds = time - previousSeconds;
	previousSeconds = time;

	if (m_updateCallback) m_updateCallback(*m_scene, glfwGetTime());

	auto pov = pointOfView();
	float aspectRatio = (float)m_framebufferWidth/(float)m_framebufferHeight;
	pov->camera()->aspectRatio(aspectRatio);
	
	DrawStats stats = {};
	//stats.cameraPosition = pov->worldPosition();
	stats.cameraPosition = pov->position();
	
	// simulate physics
	auto physicsWorld = m_scene->physicsWorld();
	if (physicsWorld) {
		physicsWorld->step();
		
		if (m_didSimulatePhysicsCallback) {
			m_didSimulatePhysicsCallback(*m_scene, glfwGetTime());
		}
	}
	
	CheckGLError();
	
	//glBindFramebuffer(GL_FRAMEBUFFER, m_renderFramebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//glReadBuffer(GL_COLOR_ATTACHMENT0);
	
	glViewport(0, 0, m_framebufferWidth, m_framebufferHeight);
//	glViewport(0, 0, m_width, m_height);
//	glClearColor(1.0, 0, 0, 1.0);
//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	if (m_willRenderCallback) m_willRenderCallback(*m_scene, glfwGetTime());
	
	m_scene->draw(pov, m_debugOptions, stats);
	
	if ((unsigned)m_debugOptions & (unsigned)DEBUG_OPTIONS::SHOW_STATS_OVERLAY) updateStatsOverlay(stats);

//	glBindFramebuffer(GL_READ_FRAMEBUFFER, m_renderFramebuffer);
//	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
//
//	glBlitFramebuffer(0, 0, m_width, m_height,
//					  0, 0, m_framebufferWidth, m_framebufferHeight,
//					  GL_COLOR_BUFFER_BIT, GL_NEAREST); // must be GL_NEAREST for integer format data
//
//	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
	glfwSwapBuffers(i_glfwWindow);

	if (m_recordingGIF) saveGIFFrame(deltaSeconds);
	
	if (m_didRenderCallback) m_didRenderCallback(*m_scene, glfwGetTime());
	
	glfwPollEvents();
	if (m_inputManager) m_inputManager->update();
}

void Window::updateStatsOverlay(DrawStats& stats) {
	
	static float fps = 0.0;
	static float ms = 0.0;
	static float percent = 0.0;

	const float GOAL_TIME = 16.6666667f;

	static unsigned elapsedFrames = 0; ++elapsedFrames;
	static float previousSeconds = glfwGetTime();
	float currentSeconds = glfwGetTime();
	float elapsedSeconds = currentSeconds - previousSeconds;

	if (elapsedSeconds > 0.5) {
		// only update the framerate stats every so often so they're readable
		
		ms = ((elapsedSeconds*1000.0) / elapsedFrames);
		fps = elapsedFrames/elapsedSeconds;
		percent = (ms / GOAL_TIME) * 100.0f;

		// reset framerate stats
		previousSeconds = currentSeconds;
		elapsedFrames = 0;
	}

	gl3fonsProjectionSize(m_fonsContext, m_framebufferWidth, m_framebufferHeight);
	
	glDisable(GL_DEPTH_TEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	
	float dx = 12.0 * m_framebufferScale;
	float dy = 20.0 * m_framebufferScale;
	
	fonsClearState(m_fonsContext);
	
	fonsSetFont(m_fonsContext, m_fonsFont);

	static float textSize = 14.0 * m_framebufferScale;
	static float hPadding = 0.0 * m_framebufferScale;
	
	char tmpStr[256];
	
	sprintf(tmpStr, "%-14s %.1f%s" ,"framerate", fps, (m_vSyncEnabled ? " [vsync]" : ""));
	drawText(tmpStr, textSize, dx, dy);
	dy += (textSize + hPadding);
	
	sprintf(tmpStr, "%-14s %.1f" ,"frametime", ms);
	drawText(tmpStr, textSize, dx, dy);
	dy += (textSize + hPadding);
	
	sprintf(tmpStr, "%-14s %.1f" ,"percent", percent);
	drawText(tmpStr, textSize, dx, dy);
	dy += (textSize + hPadding);
	
	dy += textSize; // skip a line

	sprintf(tmpStr, "%-14s %d" ,"nodes", stats.nodes);
	drawText(tmpStr, textSize, dx, dy);
	dy += (textSize + hPadding);
	
	sprintf(tmpStr, "%-14s %d" ,"geometries", stats.geometries);
	drawText(tmpStr, textSize, dx, dy);
	dy += (textSize + hPadding);
	
	sprintf(tmpStr, "%-14s %d" ,"meshes", stats.meshes);
	drawText(tmpStr, textSize, dx, dy);
	dy += (textSize + hPadding);
	
	sprintf(tmpStr, "%-14s %d" ,"polygons", stats.polygons);
	drawText(tmpStr, textSize, dx, dy);
	dy += (textSize + hPadding);
	
	sprintf(tmpStr, "%-14s %d" ,"lights", stats.lights);
	drawText(tmpStr, textSize, dx, dy);
	dy += (textSize + hPadding);
	
	dy += textSize; // skip a line
	
	sprintf(tmpStr, "%-14s %.1f, %.1f, %.1f" ,"camera pos",
			stats.cameraPosition.x, stats.cameraPosition.y, stats.cameraPosition.z);
	drawText(tmpStr, textSize, dx, dy);
	dy += (textSize + hPadding);
}

float Window::drawText(string text, float size, float dx, float dy) {
	// must setup fons GL state first
	
	static unsigned black = gl3fonsRGBA(0, 0, 0, 255);
	static unsigned white = gl3fonsRGBA(255, 255, 255, 255);
	
	fonsSetSize(m_fonsContext, size);
	
	fonsSetColor(m_fonsContext, black);
	fonsSetBlur(m_fonsContext, 1);
	fonsDrawText(m_fonsContext, dx, dy, text.c_str(), NULL);
	
	fonsSetColor(m_fonsContext, white);
	fonsSetBlur(m_fonsContext, 0);
	return fonsDrawText(m_fonsContext, dx, dy, text.c_str(), NULL);
}

void Window::saveGIFFrame(float deltaSeconds) {

	static float secondsAccum = 0;
	secondsAccum += deltaSeconds;
	
	unsigned frameTimeMS = 1000.0 /* (ms/sec) */ / m_gifRecordingMaxFramerate /* (frames/sec) */;
	// -> ms/frame
	//unsigned frameTimeHS = frameTimeMS / 10.0; // 100th sec/frame
	
	//unsigned frameTime = 1000.0/m_gifRecordingMaxFramerate; // ms/frame
	
	if (secondsAccum >= frameTimeMS/1000.0) {
		
		auto frame = snapshot();
		
		unsigned char* resizedFrameData = (unsigned char*)malloc(m_gifRecordingWidth * m_gifRecordingHeight * 4);
		stbir_resize_uint8(frame->data(), frame->width(), frame->height(), 0,
						   resizedFrameData, m_gifRecordingWidth, m_gifRecordingHeight, 0, 4);
		
		// gif-h frame time is in 100ths of a second
		GifWriteFrame(i_gifWriter, resizedFrameData,
					  m_gifRecordingWidth, m_gifRecordingHeight,
					  (secondsAccum*1000.0)/10.0);
		
		//secondsAccum = secondsAccum - frameTimeMS/1000.0;
		secondsAccum = 0;
	}
}

