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

#//include "fontstash.h"
#include "gl3fontstash.h"
#include <glm/gtc/matrix_transform.hpp>

#include "Camera.h"
#include "Color.h"
#include "Geometry.h"
#include "Global.h"
#include "Image.h"
#include "InputManager.h"
#include "Logger.h"
#include "Node.h"
#include "Scene.h"
#include "Utilities.h"


using namespace std;
using namespace ae;
using namespace glm;
using namespace utils;


/***************************************************************************************
     MARK:   Internal Members
 **************************************************************************************/

Window* i_window;
GLFWwindow* i_glfwWindow;

/***************************************************************************************
     MARK:   GLFW Callbacks
 **************************************************************************************/

void glfwWindowSizeCallback(GLFWwindow* glfwWindow, int aWidth, int aHeight) {
	cout << "glfwWindowSizeCallback()" << endl;
	
	i_window->width(aWidth);
	i_window->height(aHeight);
}

void glfwFramebufferSizeCallback(GLFWwindow* glfwWindow, int aWidth, int aHeight) {
	cout << "glfwFramebufferSizeCallback()" << endl;
	
	i_window->framebufferWidth(i_window->width() * i_window->framebufferScale());
	i_window->framebufferHeight(i_window->height() * i_window->framebufferScale());
}

/***************************************************************************************
     MARK:   Lifescycle
 **************************************************************************************/

Window::Window(bool fullScreen, unsigned width, unsigned height, bool useHighDPI) {
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
	
		if (initLog() != 0) {
			cout << "Error initializing log." << endl;
		}

		if (initGLFW() != 0) {
			AE_LOG.critical("Error initializing GLFW.");
		}
		
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_SAMPLES, 4); // TODO: Temporary
//		glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
	
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

		AE_LOG.info("scaleFactor: {}", scaleFactor);

		if (!i_glfwWindow) {
			//cout << "Error creating glfwWindow." << endl;
			AE_LOG.critical("Error creating glfwWindow: {}, {}", g_glfwLastErrorCode, g_glfwLastErrorDescription);
			glfwTerminate();
			//return -1;
		}
		
		glfwMakeContextCurrent(i_glfwWindow);
		vSyncEnabled(false);
	
		initGLEW();
	
		i_window = this;
	
		// setup Font Stash
	
		m_fonsContext = gl3fonsCreate(512, 512, FONS_ZERO_TOPLEFT);
		if (m_fonsContext == NULL) {
			AE_LOG.error("Error creating Font Stash context.");
		}
	
		string fontName = "SourceCodePro-Semibold";
		string fontType = "otf";
		string fontPath = FontPath(fontName, fontType);
	
		m_fonsFont = fonsAddFont(m_fonsContext, fontName.c_str(), fontPath.c_str());
		if (m_fonsFont == FONS_INVALID) {
			AE_LOG.error("Could not load font: {}", fontPath);
		}
			
		// moved from initializer list
		
		m_scene = make_shared<Scene>();
		m_width = viewportWidth;
		m_height = viewportHeight;
    
        m_framebufferScale = (useHighDPI ? scaleFactor : 1.0);
		m_framebufferWidth = m_width * m_framebufferScale;
		m_framebufferHeight = m_height * m_framebufferScale;
		m_antialiasingMode = AntialiasingMode_None;
		m_debugOptions = DebugOption_ShowStatsOveray;
		m_backgroundColor = nullptr;
		m_pointOfView = nullptr;
		m_inputManager = nullptr;
		m_maximumFramerate = 60.0;
		m_willUpdateCallback = nullptr;
		m_didUpdateCallback = nullptr;
}

Window::~Window() {
	glfwTerminate();
}

/***************************************************************************************
     MARK:   Public
 **************************************************************************************/

void Window::display() {
	AE_LOG.trace("Window::display()");

	glfwMakeContextCurrent(i_glfwWindow); // also done in Init
	
	glfwSetWindowSizeCallback(i_glfwWindow, glfwWindowSizeCallback);
	glfwSetFramebufferSizeCallback(i_glfwWindow, glfwFramebufferSizeCallback);
	
	while (!glfwWindowShouldClose(i_glfwWindow)) {
		static double previousSeconds = glfwGetTime();
		float totalSeconds = glfwGetTime();
		float deltaSeconds = totalSeconds - previousSeconds;
		
		//cout << "deltaSeconds: " << deltaSeconds << endl;
		
//		static const float frameInASecond = 1.0f/60.0f;
//		float diff = deltaSeconds - (m_maximumFramerate * frameInASecond);
//		if (diff >= 0) {
//		if (deltaSeconds >= (m_maximumFramerate * frameInASecond)) {
		
			if (m_willUpdateCallback) m_willUpdateCallback(*m_scene, deltaSeconds);
			
			mainLoop(deltaSeconds);
			
			if (!glfwWindowShouldClose(i_glfwWindow)) {
				if (m_didUpdateCallback) m_didUpdateCallback(*m_scene, deltaSeconds);
			}
//		}
//		else {
//			sleep(fabs(diff));
//		}
		
		previousSeconds = totalSeconds;
		
	}
}

shared_ptr<Scene> Window::scene() const {
	return m_scene;
}

void Window::scene(const shared_ptr<Scene> scene) {
	m_scene = scene;

	//checkAddDefaultCamera();
}

void Window::enableCursor(bool enabled) {
	glfwSetInputMode(i_glfwWindow, GLFW_CURSOR, (enabled ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED));
}

bool Window::vSyncEnabled() const {
	return m_vSyncEnabled;
}

void Window::vSyncEnabled(bool enabled) {
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

DebugOption Window::debugOptions() const {
	return m_debugOptions;
}

void Window::debugOptions(DebugOption options) {
	m_debugOptions = options;
}

shared_ptr<Image> Window::snapshot() const {
	
	unsigned char *buf = (unsigned char*)malloc(m_framebufferWidth * m_framebufferHeight * 4);
	glReadPixels(0, 0, m_framebufferWidth, m_framebufferHeight, GL_RGBA, GL_UNSIGNED_BYTE, buf);
	auto image = make_shared<Image>(buf, m_framebufferWidth, m_framebufferHeight);
	free(buf);
	return image;
}

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

AntialiasingMode Window::antialiasingMode() const {
	return m_antialiasingMode;
}

void Window::antialiasingMode(AntialiasingMode mode) {
	m_antialiasingMode = mode;
	glfwWindowHint(GLFW_SAMPLES, mode);
}

//shared_ptr<Color> Window::backgroundColor() const {
//	return m_backgroundColor;
//}
//
//void Window::backgroundColor(const shared_ptr<Color> color) {
//	m_backgroundColor = color;
//}

shared_ptr<Node> Window::pointOfView() {
	
	if (m_pointOfView) {
		return m_pointOfView;
	}
	else {
		// try to assign one from the scene
		for (auto node: m_scene->rootNode()->allChildNodes()) {
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

//	float maxX = boundingPoints["xMax"].x;
//	float minX = boundingPoints["xMin"].x;
//	float maxY = boundingPoints["yMax"].y;
//	float minY = boundingPoints["yMin"].y;

	float maxZ = abs(boundingPoints["zMax"].z);

	float xH = abs(boundingPoints["xMin"].x) + abs(boundingPoints["xMax"].x) / 2.0f;
	float angleH = fovH / 2.0;
	float zH = xH / tan(angleH);

	float xV = abs(boundingPoints["yMin"].y) + abs(boundingPoints["yMax"].y) / 2.0f;
	float angleV = fovV / 2.0;
	float zV = xV / tan(angleV);

//	cout << "zH: " << zH << endl;
//	cout << "zV: " << zV << endl;

	zH += maxZ;
	zV += maxZ;

	float z = fmax(zH, zV);
	float midX = (boundingPoints["xMin"].x + boundingPoints["xMax"].x) / 2.0f;
	float midY = (boundingPoints["yMin"].y + boundingPoints["yMax"].y) / 2.0f;

//	cout << "maxX: " << maxX << endl;
//	cout << "minX: " << minX << endl;
//	cout << "maxY: " << maxY << endl;
//	cout << "minY: " << minY << endl;
//
//	cout << "maxZ: " << maxZ << endl;
//
//	cout << "midX: " << midX << endl;
//	cout << "midY: " << midY << endl;

	vec3 eye = vec3(midX, midY, z / 2.0f); // not sure why z is devided by 2.0, but it seems to work better...
	vec3 center = vec3(midX, midY, 0);

//	cout << "eye: " << eye << endl;
//	cout << "center: " << center << endl;

	mat4 viewMat = translate(mat4(1.0f), eye);
	cameraNode->transform(viewMat);

	scene()->rootNode()->addChildNode(cameraNode);
	pointOfView(cameraNode);
	
	return cameraNode;
}

GLFWwindow* Window::glfwWindow() const {
	return i_glfwWindow;
}

windowWillUpdateFuction Window::willUpdateCallback() {
	return m_willUpdateCallback;
}

void Window::willUpdateCallback(windowWillUpdateFuction function) {
	m_willUpdateCallback = function;
}

windowDidUpdateFuction Window::didUpdateCallback() {
	return m_didUpdateCallback;
}

void Window::didUpdateCallback(windowDidUpdateFuction function) {
	m_didUpdateCallback = function;
}

/***************************************************************************************
     MARK:   Private
 **************************************************************************************/

//void Window::updateFrametime(unsigned int numPolygons) {
//	const float GOAL_TIME = 16.6666667f;
//	static unsigned elapsedFrames = 0; ++elapsedFrames;
//	static float previousSeconds = glfwGetTime();
//	float currentSeconds = glfwGetTime();
//	float elapsedSeconds = currentSeconds - previousSeconds;
//	if (elapsedSeconds > 0.25) {
//		previousSeconds = currentSeconds;
//		float ms = ((elapsedSeconds*1000.0) / elapsedFrames);
//		float fps = elapsedFrames/elapsedSeconds;
//		float percentGoal = (ms / GOAL_TIME) * 100.0f;
//		char tmp[128];
//		sprintf(tmp, "%.1f ms | %.1f fps | %.1f %% | %u polys", ms, fps, percentGoal, numPolygons);
//		glfwSetWindowTitle(i_glfwWindow, tmp);
//		elapsedFrames = 0;
//	}
//}

void Window::mainLoop(float deltaSeconds) {
	
	AE_LOG.trace("-------------------------------------------------------------------------------");
	
	glViewport(0, 0, m_framebufferWidth, m_framebufferHeight);
	
	auto pov = pointOfView();
	float aspectRatio = (float)m_framebufferWidth/(float)m_framebufferHeight;
	pov->camera()->aspectRatio(aspectRatio);
	
	DrawStats stats = {};
	stats.cameraPosition = pov->position();
	
	m_scene->draw(pov, m_debugOptions, stats);
	
	glfwPollEvents();
	
	if (m_inputManager != nullptr) {
		m_inputManager->update(deltaSeconds);
	}
	
	if (m_debugOptions & DebugOption_ShowStatsOveray) {
		updateStatsOverlay(stats);
	}

	glfwSwapBuffers(i_glfwWindow);
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

	if (elapsedSeconds > 0.25) {
		
		ms = ((elapsedSeconds*1000.0) / elapsedFrames);
		fps = elapsedFrames/elapsedSeconds;
		percent = (ms / GOAL_TIME) * 100.0f;

		// reset stats
		previousSeconds = currentSeconds;
		elapsedFrames = 0;
	}
	
	int width;
	int height;
	glfwGetFramebufferSize(i_glfwWindow, &width, &height);
	gl3fonsProjectionSize(m_fonsContext, width, height);
	
	glDisable(GL_DEPTH_TEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	
	float dx = 12.0;
	float dy = 20.0;
	
	fonsClearState(m_fonsContext);
	
	fonsSetFont(m_fonsContext, m_fonsFont);

	static float textSize = 14.0;
	static float hPadding = 0.0;
	
	char tmpStr[256];
	
	
	//	framerate		322
	//	frametime		7.6
	//
	//	nodes			14
	//	geometries		7
	//	meshes			12
	//	polygons		576,300
	//	lights			2
	//
	//	camera pos		-23.4, 43.6, -66.3
	
	
	sprintf(tmpStr, "%-14s %.1f" ,"framerate", fps);
	drawTextLine(tmpStr, textSize, dx, dy);
	dy += (textSize + hPadding);
	
	sprintf(tmpStr, "%-14s %.1f" ,"frametime", ms);
	drawTextLine(tmpStr, textSize, dx, dy);
	dy += (textSize + hPadding);
	
	sprintf(tmpStr, "%-14s %.1f" ,"percent", percent);
	drawTextLine(tmpStr, textSize, dx, dy);
	dy += (textSize + hPadding);
	
	dy += textSize; // skip a line

	sprintf(tmpStr, "%-14s %d" ,"nodes", stats.nodes);
	drawTextLine(tmpStr, textSize, dx, dy);
	dy += (textSize + hPadding);
	
	sprintf(tmpStr, "%-14s %d" ,"geometries", stats.geometries);
	drawTextLine(tmpStr, textSize, dx, dy);
	dy += (textSize + hPadding);
	
	sprintf(tmpStr, "%-14s %d" ,"meshes", stats.meshes);
	drawTextLine(tmpStr, textSize, dx, dy);
	dy += (textSize + hPadding);
	
	sprintf(tmpStr, "%-14s %d" ,"polygons", stats.polygons);
	drawTextLine(tmpStr, textSize, dx, dy);
	dy += (textSize + hPadding);
	
	sprintf(tmpStr, "%-14s %d" ,"lights", stats.lights);
	drawTextLine(tmpStr, textSize, dx, dy);
	dy += (textSize + hPadding);
	
	dy += textSize; // skip a line
	
	sprintf(tmpStr, "%-14s %.1f, %.1f, %.1f" ,"camera pos",
			stats.cameraPosition.x, stats.cameraPosition.y, stats.cameraPosition.z);
	drawTextLine(tmpStr, textSize, dx, dy);
	dy += (textSize + hPadding);
}

//string createStatLine(string label, string value, unsigned )


float Window::drawTextLine(std::string line, float size, float dx, float dy) {
	// must setup fons GL state first
	
	static unsigned black = gl3fonsRGBA(0, 0, 0, 255);
	static unsigned white = gl3fonsRGBA(255, 255, 255, 255);
	
	fonsSetSize(m_fonsContext, size);
	
	fonsSetColor(m_fonsContext, black);
	fonsSetBlur(m_fonsContext, 1);
	fonsDrawText(m_fonsContext, dx, dy, line.c_str(), NULL);
	
	fonsSetColor(m_fonsContext, white);
	fonsSetBlur(m_fonsContext, 0);
	return fonsDrawText(m_fonsContext, dx, dy, line.c_str(), NULL);
}

