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

//#define GLFW_DLL
//#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

#include "Camera.h"
#include "Color.h"
#include "Geometry.h"
#include "Globals.h"
#include "Init.h"
#include "InputManager.h"
#include "Node.h"
#include "Scene.h"
#include "Utilities.h"


using namespace std;
using namespace ae;
using namespace glm;
using namespace utils;


/***************************************************************************************
     MARK:   Globals
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

		if (initGLFW() != 0) {
			cout << "Init error!" << endl;// return -1;
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
            // causing a segfault.  So we'll cheat and use the main monitor (probably the one)
            // it's going to use anyway...
            //scaleFactor = GetScreenScaleFactor(glfwGetWindowMonitor(i_glfwWindow));
            scaleFactor = GetScreenScaleFactor(glfwGetPrimaryMonitor());
		}
    
        cout << "scaleFactor: " << scaleFactor << endl;
		
		if (!i_glfwWindow) {
			cout << "Error creating glfwWindow." << endl;
			glfwTerminate();
			//return -1;
		}
		
		glfwMakeContextCurrent(i_glfwWindow);
		vSyncEnabled(false);
	
		initGLEW();
	
		i_window = this;
			
		// moved from initializer list
		
		m_scene = make_shared<Scene>();
		m_width = viewportWidth;
		m_height = viewportHeight;
    
    
        m_framebufferScale = (useHighDPI ? scaleFactor : 1.0);
		m_framebufferWidth = m_width * m_framebufferScale;
		m_framebufferHeight = m_height * m_framebufferScale;
		m_antialiasingMode = AntialiasingMode_None;
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
	cout << "Window::display()" << endl;

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

void Window::debugOptions(const DebugOption options) {
	m_debugOptions = options;
}

shared_ptr<Image> Window::snapshot() const {
	return nullptr;
}

/***************************************************************************************
     MARK:   Internal
 **************************************************************************************/

unsigned Window::width() const {
	return m_width;
}

void Window::width(const unsigned aWidth) {
	m_width = aWidth;
	framebufferWidth(m_width * m_framebufferScale);
}

unsigned Window::height() const {
	return m_height;
}

void Window::height(const unsigned aHeight) {
	m_height = aHeight;
	framebufferHeight(m_height * m_framebufferScale);
}

unsigned Window::framebufferScale() const {
	return m_framebufferScale;
}

void Window::framebufferScale(const unsigned aScale) {
	m_framebufferScale = aScale;
}

unsigned Window::framebufferWidth() const {
	return m_framebufferWidth;
}

void Window::framebufferWidth(const unsigned aWidth) {
	m_framebufferWidth = aWidth;
}

unsigned Window::framebufferHeight() const {
	return m_framebufferHeight;
}

void Window::framebufferHeight(const unsigned aHeight) {
	m_framebufferHeight = aHeight;
}

AntialiasingMode Window::antialiasingMode() const {
	return m_antialiasingMode;
}

void Window::antialiasingMode(const AntialiasingMode mode) {
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

	float maxX = boundingPoints["xMax"].x;
	float minX = boundingPoints["xMin"].x;
	float maxY = boundingPoints["yMax"].y;
	float minY = boundingPoints["yMin"].y;

	float maxZ = abs(boundingPoints["zMax"].z);

	float xH = abs(boundingPoints["xMin"].x) + abs(boundingPoints["xMax"].x) / 2.0f;
	float angleH = fovH / 2.0;
	float zH = xH / tan(angleH);

	float xV = abs(boundingPoints["yMin"].y) + abs(boundingPoints["yMax"].y) / 2.0f;
	float angleV = fovV / 2.0;
	float zV = xV / tan(angleV);

	cout << "zH: " << zH << endl;
	cout << "zV: " << zV << endl;

	zH += maxZ;
	zV += maxZ;

	float z = fmax(zH, zV);
	float midX = (boundingPoints["xMin"].x + boundingPoints["xMax"].x) / 2.0f;
	float midY = (boundingPoints["yMin"].y + boundingPoints["yMax"].y) / 2.0f;

	cout << "maxX: " << maxX << endl;
	cout << "minX: " << minX << endl;
	cout << "maxY: " << maxY << endl;
	cout << "minY: " << minY << endl;

	cout << "maxZ: " << maxZ << endl;

	cout << "midX: " << midX << endl;
	cout << "midY: " << midY << endl;

	vec3 eye = vec3(midX, midY, z / 2.0f); // not sure why z is devided by 2.0, but it seems to work better...
	vec3 center = vec3(midX, midY, 0);

	cout << "eye: " << eye << endl;
	cout << "center: " << center << endl;

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

void Window::updateFrametime(unsigned int numPolygons) {
	const float GOAL_TIME = 16.6666667f;
	static unsigned elapsedFrames = 0; ++elapsedFrames;
	static float previousSeconds = glfwGetTime();
	float currentSeconds = glfwGetTime();
	float elapsedSeconds = currentSeconds - previousSeconds;
	if (elapsedSeconds > 0.25) {
		previousSeconds = currentSeconds;
		float ms = ((elapsedSeconds*1000.0) / elapsedFrames);
		float fps = elapsedFrames/elapsedSeconds;
		float percentGoal = (ms / GOAL_TIME) * 100.0f;
		char tmp[128];
		sprintf(tmp, "%.1f ms | %.1f fps | %.1f %% | %u polys", ms, fps, percentGoal, numPolygons);
		glfwSetWindowTitle(i_glfwWindow, tmp);
		elapsedFrames = 0;
	}
}

void Window::mainLoop(const float deltaSeconds) {
	
	//cout << "\n-------------------------------------------------------------------------------" << endl;
	
	unsigned numPolygons = 0;
	
	glViewport(0, 0, m_framebufferWidth, m_framebufferHeight);

	auto pov = pointOfView();
	float aspectRatio = (float)m_framebufferWidth/(float)m_framebufferHeight;
	pov->camera()->aspectRatio(aspectRatio);
	numPolygons += m_scene->draw(pov);

	if (m_inputManager != nullptr) {
		m_inputManager->update(deltaSeconds);
	}
	glfwPollEvents();

	glfwSwapBuffers(i_glfwWindow);
	updateFrametime(numPolygons);
}
