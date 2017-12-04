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
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

#include "Camera.h"
#include "Color.h"
#include "Geometry.h"
#include "Globals.h"
#include "InputManager.h"
//#include "Material.h" // temporary
//#include "MaterialProperty.h" // temporary
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

	//checkAddDefaultCamera();
}

DebugOption& Window::debugOptions() {
	return m_debugOptions;
}

void Window::debugOptions(const DebugOption& options) {
	m_debugOptions = options;
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

shared_ptr<Node> Window::pointOfView() const {
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

shared_ptr<Node> Window::addDefaultPointOfView() {
	
//	for (auto node : m_scene->rootNode()->allChildNodes()) {
//		if (node->camera()) {
//			return;
//		}
//	}
	
	auto cameraNode = make_shared<Node>();
	//m_scene->rootNode()->addChildNode(cameraNode); // done below
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

//	mat4 viewMat = lookAt(eye,	// eye - location
//						  center,	// center - look at
//						  vec3(0, 1, 0));	// up
	
	// TODO: WHY does look at appear to invert its output?? see also Geometry::draw()
	mat4 viewMat = translate(mat4(1.0f), eye);

	cameraNode->transform(viewMat);
	
	//cout << "default viewMat: " << viewMat << endl;

	scene()->rootNode()->addChildNode(cameraNode);
	pointOfView(cameraNode);
	
	return cameraNode;
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
	cout << "\n-------------------------------------------------------------------------------" << endl;
	
	unsigned int numPolygons = 0;
	
	glClearColor(109.0f/256.0f, 136.0f/256.0f, 164.0f/256.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, framebufferWidth(), framebufferHeight());
	
	shared_ptr<Camera> camera = nullptr;
	
	if (pointOfView()) {
		camera = pointOfView()->camera();
	}
	else {
		// try to find one
		for (auto node: scene()->rootNode()->allChildNodes()) {
			if (node->camera()) {
				camera = node->camera();
				pointOfView(node);
				break;
			}
		}
	}
	if (!camera) {
		// still no camera. add a default one.
		pointOfView(addDefaultPointOfView());
		camera = pointOfView()->camera();
	}
	
	
//	for (auto mat : m_scene->materials()) {
//		cout << "Color: ("
//		<< mat->diffuse()->color()->r << ", "
//		<< mat->diffuse()->color()->g<< ", "
//		<< mat->diffuse()->color()->b << ")"<< endl;
//	}
	
	
	
	auto viewMat = pointOfView()->worldTransform();
	auto projectionMat = pointOfView()->camera()->projection();

	for (auto node: scene()->rootNode()->allChildNodes()) {
		if (!node->hidden()) {
			auto geometry = node->geometry();
			if (geometry != nullptr) {
				cout << "Node: " << node->name() << endl;
				auto modelMat = node->worldTransform();
				numPolygons += geometry->draw(modelMat, viewMat, projectionMat);
			}
		}
	}
	
	if (m_inputManager != nullptr) {
		m_inputManager->update(deltaSeconds);
	}
	glfwPollEvents();

	glfwSwapBuffers(g_glfwWindow);
	updateFrametime(numPolygons);
}
