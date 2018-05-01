//
//  RenderContext.cpp
//	avara-engine
//
//  Created by Morgan Davis on 4/24/18.
//  Copyright © 2018 Morgan K Davis. All rights reserved.
//

#include "RenderContext.h"

#include <string>
#include <vector>

#include "gif.h"

#include "Camera.h"
#include "Image.h"
#include "Logger.h"
#include "Node.h"
#include "Renderer.h"
#include "Scene.h"


using namespace ae;
using namespace glm;
using namespace std;


/***************************************************************************************
     Lifescycle
 ***************************************************************************************/

RenderContext::RenderContext(shared_ptr<Renderer> renderer):
	m_renderer(renderer),
	m_scene(nullptr),
	m_vSyncEnabled(false),
	m_debugOptions(DEBUG_OPTIONS::NONE),
	m_antialiasingMode(ANTIALIASING_MODE::NONE),
	m_pointOfView(nullptr),
	m_updateCallback(nullptr),
	m_didSimulatePhysicsCallback(nullptr),
	m_willRenderCallback(nullptr),
	m_didRenderCallback(nullptr) {
	
}

RenderContext::~RenderContext() {
	
}

/**************************************************************************************
     Public
 **************************************************************************************/

shared_ptr<Renderer> RenderContext::renderer() const {
	return m_renderer;
}

void RenderContext::renderer(shared_ptr<Renderer> renderer) {
	m_renderer = renderer;
}

shared_ptr<Scene> RenderContext::scene() const {
	//return m_renderer->scene();
	return m_scene;
}

void RenderContext::scene(const shared_ptr<Scene> scene) {
	//m_renderer->scene(scene);
	m_scene = scene;
	scene->attachedToRenderContext(shared_from_this());
}

unsigned RenderContext::width() const {
	return m_width;
}

void RenderContext::width(unsigned width) {
	m_width = width;
	framebufferWidth(m_width * m_framebufferScale);
}

unsigned RenderContext::height() const {
	return m_height;
}

void RenderContext::height(unsigned height) {
	m_height = height;
	framebufferHeight(m_height * m_framebufferScale);
}

unsigned RenderContext::framebufferScale() const {
	return m_framebufferScale;
}

void RenderContext::framebufferScale(unsigned scale) {
	m_framebufferScale = scale;
}

unsigned RenderContext::framebufferWidth() const {
	return m_framebufferWidth;
}

void RenderContext::framebufferWidth(unsigned width) {
	m_framebufferWidth = width;
}

unsigned RenderContext::framebufferHeight() const {
	return m_framebufferHeight;
}

void RenderContext::framebufferHeight(unsigned height) {
	m_framebufferHeight = height;
}

bool RenderContext::vSyncEnabled() const {
	return m_vSyncEnabled;
}

void RenderContext::enableVSync(bool enabled) {
	m_vSyncEnabled = enabled;
}

DEBUG_OPTIONS RenderContext::debugOptions() const {
	return m_debugOptions;
}

void RenderContext::debugOptions(DEBUG_OPTIONS options) {
	m_debugOptions = options;
	
//	if (m_scene && m_scene->physicsWorld()) {
//		m_scene->physicsWorld()->debugOptions(m_debugOptions);
//	}
}

shared_ptr<Node> RenderContext::pointOfView() {
	
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

void RenderContext::pointOfView(const shared_ptr<Node> camera) {
	m_pointOfView = camera;
}

ANTIALIASING_MODE RenderContext::antialiasingMode() const {
	return m_antialiasingMode;
}

shared_ptr<InputManager> RenderContext::inputManager() {
	AE_LOG->critical("RenderContext::inputManager() should be overidden in derived class.");
	return nullptr;
}

float RenderContext::sceneTime() const {
	// should probably override in subclass to use library's time utilities (GLFW, for example)
	static auto startDate = chrono::high_resolution_clock::now();
	auto nowDate = chrono::high_resolution_clock::now();
	return (chrono::duration<float>(nowDate - startDate)).count();
}

shared_ptr<Image> RenderContext::snapshot() const {
	if (m_renderer) {
		return m_renderer->snapshot(*this);
	}
	return nullptr;
}

bool RenderContext::recordingGIF() const {
	return m_recordingGIF;
}

void RenderContext::startGIFRecording(const boost::filesystem::path& path,
							   unsigned maxHeight, unsigned maxFramerate) {
	
}

unsigned RenderContext::recordedGIFFrames() const {
	return 0; 
}

void RenderContext::stopGIFRecording() {

}

RenderContextUpdateFuction RenderContext::updateCallback() {
	return m_updateCallback;
}

void RenderContext::updateCallback(RenderContextUpdateFuction function) {
	m_updateCallback = function;
}

RenderContextDidSimulatePhysicsFuction RenderContext::didSimulatePhysicsCallback() {
	return m_didSimulatePhysicsCallback;
}

void RenderContext::didSimulatePhysicsCallback(RenderContextDidSimulatePhysicsFuction function) {
	m_didSimulatePhysicsCallback = function;
}

RenderContextWillRenderFuction RenderContext::willRenderCallback() {
	return m_willRenderCallback;
}

void RenderContext::willRenderCallback(RenderContextWillRenderFuction function) {
	m_willRenderCallback = function;
}

RenderContextDidRenderFuction RenderContext::didRenderCallback() {
	return m_didRenderCallback;
}

void RenderContext::didRenderCallback(RenderContextDidRenderFuction function) {
	m_didRenderCallback = function;
}

/**************************************************************************************
     Protected
 **************************************************************************************/

void RenderContext::saveGIFFrame(float deltaSeconds) {
	
}

shared_ptr<Node> RenderContext::defaultPointOfView() {
	
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
