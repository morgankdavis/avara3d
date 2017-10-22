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

#define GLFW_DLL
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

#include "Camera.h"
#include "Color.h"
#include "Geometry.h"
#include "Globals.h"
#include "InputManager.h"
#include "Node.h"
#include "Scene.h"
#include "Utilities.h"

//#if defined(EXPERIMENTAL)
//#include <nanogui/nanogui.h>
//#include <glad/glad.h>
//#endif


using namespace std;
using namespace ae;
using namespace glm;
using namespace utils;


//#if defined(EXPERIMENTAL)
//using namespace nanogui;
//#endif


/***************************************************************************************
     MARK:   Globals
 **************************************************************************************/

Window *window;

/***************************************************************************************
     MARK:   GLFW Callbacks
 **************************************************************************************/

void glfwWindowSizeCallback(GLFWwindow* glfwWindow, int aWidth, int aHeight) {
	cout << "glfwWindowSizeCallback()" << endl;
	
	window->width(aWidth);
	window->height(aHeight);
}

void glfwFramebufferSizeCallback(GLFWwindow* glfwWindow, int aWidth, int aHeight) {
	cout << "glfwFramebufferSizeCallback()" << endl;
	
	window->framebufferWidth(window->width() * window->framebufferScale());
	window->framebufferHeight(window->height() * window->framebufferScale());
}

/***************************************************************************************
     MARK:   Lifescycle
 **************************************************************************************/

Window::Window(const unsigned width, const unsigned height, const float framebufferScale):
	m_scene(make_shared<Scene>()),
	m_width(width),
	m_height(height),
	m_framebufferScale(framebufferScale),
	m_framebufferWidth(m_width * m_framebufferScale),
	m_framebufferHeight(m_height * m_framebufferScale),
	m_antialiasingMode(AntialiasingMode_None),
	m_backgroundColor(nullptr),
	m_pointOfView(nullptr),
	m_inputManager(nullptr),
	m_willUpdateCallback(nullptr),
	m_didUpdateCallback(nullptr) {

	window = this;


//#if defined(EXPERIMENTAL)
//#if defined(NANOGUI_GLAD)
//	if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress))
//        throw std::runtime_error("Could not initialize GLAD!");
//    glGetError(); // pull and ignore unhandled errors like GL_INVALID_ENUM
//#endif
//#endif
//
//	m_screen = make_shared<Screen>();
//	m_screen->initialize(window, true);

}

Window::~Window() {
	glfwTerminate();
}

/***************************************************************************************
     MARK:   Public
 **************************************************************************************/

void Window::display() {
	cout << "Window::display()" << endl;

	glfwMakeContextCurrent(g_glfwWindow); // also done in Init
	
	glfwSetWindowSizeCallback(g_glfwWindow, glfwWindowSizeCallback);
	glfwSetFramebufferSizeCallback(g_glfwWindow, glfwFramebufferSizeCallback);
	
	while (!glfwWindowShouldClose(g_glfwWindow)) {
		static double previousSeconds = glfwGetTime();
		float totalSeconds = glfwGetTime();
		float deltaSeconds = totalSeconds - previousSeconds;
		previousSeconds = totalSeconds;
		
		if (m_willUpdateCallback) m_willUpdateCallback(*m_scene, deltaSeconds);
		
		mainLoop(deltaSeconds);
		
		if (!glfwWindowShouldClose(g_glfwWindow)) {
			if (m_didUpdateCallback) m_didUpdateCallback(*m_scene, deltaSeconds);
		}
	}
}

shared_ptr<Scene> Window::scene() const {
	return m_scene;
}

void Window::scene(const shared_ptr<Scene> scene) {
	m_scene = scene;

	addDefaultCamera();
}

void Window::enableCursor(bool enabled) {
	glfwSetInputMode(g_glfwWindow, GLFW_CURSOR, (enabled ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED));
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

shared_ptr<Color> Window::backgroundColor() const {
	return m_backgroundColor;
}

void Window::backgroundColor(const shared_ptr<Color> color) {
	m_backgroundColor = color;
}

shared_ptr<Camera> Window::pointOfView() const {
	return m_pointOfView;
}

void Window::pointOfView(const shared_ptr<Camera> camera) {
	m_pointOfView = camera;
}

shared_ptr<InputManager> Window::inputManager() {
	if (m_inputManager == nullptr) {
		m_inputManager = make_shared<InputManager>(this);
	}
	return m_inputManager;
}

void Window::addDefaultCamera() {
	auto cameraNode = make_shared<Node>();
	cameraNode->camera(make_shared<Camera>());

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

	mat4 viewMat = lookAt(eye,	// eye - location
						  center,	// center - look at
						  vec3(0, 1, 0));	// up

	cameraNode->transform(viewMat);

	scene()->rootNode()->addChildNode(cameraNode);
	pointOfView(cameraNode->camera());
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
		glfwSetWindowTitle(g_glfwWindow, tmp);
		elapsedFrames = 0;
	}
}

void Window::mainLoop(const float deltaSeconds) {
	//cout << "-------------------------------------------------------------------------------" << endl;
	
	unsigned int numPolygons = 0;
	
	glClearColor(109.0f/256.0f, 136.0f/256.0f, 164.0f/256.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, framebufferWidth(), framebufferHeight());

	// THIS IS BAD.
	// we want to be able to say pointOfView()->node->camera()
	// need to solve class forwarding/circular references.
	mat4 viewMat = mat4(1.0f);
	mat4 projectionMat = mat4(1.0f);
	bool foundCamera = false;
	for (auto node: scene()->rootNode()->allChildNodes()) {
		if (node->camera() == pointOfView()) {
			viewMat = node->worldTransform();
			projectionMat = node->camera()->projection();
			foundCamera = true;
			break;
		}
	}
	if (!foundCamera) {
		cout << "*** NO CAMERA FOUND IN SCENE! ***" << endl;
	}

	for (auto node: scene()->rootNode()->allChildNodes()) {
		if (!node->hidden()) {
			auto geometry = node->geometry();
			if (geometry != nullptr) {
				numPolygons += geometry->draw(viewMat, projectionMat);
			}
		}
	}
	
	if (m_inputManager != nullptr) {
		m_inputManager->update(deltaSeconds);
	}
	glfwPollEvents();

	// TODO: Move to client side
//	if (glfwGetKey(g_glfwWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
//		glfwSetWindowShouldClose(g_glfwWindow, 1);
//	}
//	else {
		glfwSwapBuffers(g_glfwWindow);
		updateFrametime(numPolygons);
//	}
}
