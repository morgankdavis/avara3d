//
//  Renderer.cpp
//	avara-engine
//
//  Created by Morgan Davis on 4/22/18.
//  Copyright © 2018 Morgan K Davis. All rights reserved.
//

#include "Renderer.h"


#include "Camera.h"
#include "Image.h"
#include "Logger.h"
#include "Node.h"
#include "PhysicsWorld.h"
#include "Scene.h"


using namespace std;
using namespace ae;


/***************************************************************************************
     Lifescycle
 ***************************************************************************************/

Renderer::Renderer():
	m_debugOptions(DEBUG_OPTIONS::NONE),
	m_pointOfView(nullptr),
	m_maximumFramerate(60.0),
	m_updateCallback(nullptr),
	m_didSimulatePhysicsCallback(nullptr),
	m_willRenderCallback(nullptr),
	m_didRenderCallback(nullptr) {
		
}

Renderer::~Renderer() {
	
}

/**************************************************************************************
     Public
 **************************************************************************************/

shared_ptr<Scene> Renderer::scene() const {
	return m_scene;
}

void Renderer::scene(const shared_ptr<Scene> scene) {
	scene->attachedToRenderer(shared_from_this());
	m_scene = scene;
	
}

bool Renderer::vSyncEnabled() const {
	return m_vSyncEnabled;
}

void Renderer::enableVSync(bool enabled) {
	m_vSyncEnabled = enabled;
	//AE_LOG->critical("Renderer::enableVSync() must be overidden in derived class.");
}

float Renderer::maximumFramerate() const {
	return m_maximumFramerate;
}

void Renderer::maximumFramerate(float max) {
	m_maximumFramerate = max;
}

DEBUG_OPTIONS Renderer::debugOptions() const {
	return m_debugOptions;
}

void Renderer::debugOptions(DEBUG_OPTIONS options) {
	m_debugOptions = options;
	
	if (m_scene && m_scene->physicsWorld()) {
		m_scene->physicsWorld()->debugOptions(m_debugOptions);
	}
}

shared_ptr<Node> Renderer::pointOfView() {
	
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

void Renderer::pointOfView(const shared_ptr<Node> camera) {
	m_pointOfView = camera;
}

shared_ptr<Image> Renderer::snapshot() const {
	AE_LOG->critical("Renderer::snapshot() must be overidden in derived class.");
	return nullptr;
}

bool Renderer::recordingGIF() const {
	AE_LOG->critical("Renderer::recordingGIF() must be overidden in derived class.");
	return false;
}

void Renderer::startGIFRecording(const boost::filesystem::path& path,
							   unsigned maxHeight, unsigned maxFramerate) {
	AE_LOG->critical("Renderer::startGIFRecording() must be overidden in derived class.");
}

void Renderer::stopGIFRecording() {
	AE_LOG->critical("Renderer::recordingGIF() must be overidden in derived class.");
}

RendererUpdateFuction Renderer::updateCallback() {
	return m_updateCallback;
}

void Renderer::updateCallback(RendererUpdateFuction function) {
	m_updateCallback = function;
}

RendererDidSimulatePhysicsFuction Renderer::didSimulatePhysicsCallback() {
	return m_didSimulatePhysicsCallback;
}

void Renderer::didSimulatePhysicsCallback(RendererDidSimulatePhysicsFuction function) {
	m_didSimulatePhysicsCallback = function;
}

RendererWillRenderFuction Renderer::willRenderCallback() {
	return m_willRenderCallback;
}

void Renderer::willRenderCallback(RendererWillRenderFuction function) {
	m_willRenderCallback = function;
}

RendererDidRenderFuction Renderer::didRenderCallback() {
	return m_didRenderCallback;
}

void Renderer::didRenderCallback(RendererDidRenderFuction function) {
	m_didRenderCallback = function;
}

/**************************************************************************************
     Internal
 ***************************************************************************************/

shared_ptr<Node> Renderer::defaultPointOfView() {
	AE_LOG->critical("Renderer::defaultPointOfView() must be overidden in derived class.");
	return nullptr;
}

void Renderer::saveGIFFrame(float deltaSeconds) {
	AE_LOG->critical("Renderer::saveGIFFrame() must be overidden in derived class.");
}
