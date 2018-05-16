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
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb_image_resize.h"

#include "BulletPhysicsSimulator.h"
#include "Camera.h"
#include "Exception.h"
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
	m_physicsSimulator(make_shared<BulletPhysicsSimulator>()),
	m_scene(nullptr),
	m_width(0),
	m_height(0),
	m_framebufferScale(1),
	m_framebufferWidth(0),
	m_framebufferHeight(0),
	m_vSyncEnabled(false),
	m_antialiasingMode(ANTIALIASING_MODE::NONE),
	m_debugOptions(DEBUG_OPTIONS::NONE),
	m_pointOfView(nullptr),
	m_gifWriter(nullptr),
	m_recordingGIF(false),
	m_gifRecordingWidth(0),
	m_gifRecordingHeight(0),
	m_gifRecordingMaxFramerate(0),
	m_gifRecordedFrames(0),
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

unsigned RenderContext::height() const {
	return m_height;
}

unsigned RenderContext::framebufferScale() const {
	return m_framebufferScale;
}

unsigned RenderContext::framebufferWidth() const {
	return m_framebufferWidth;
}

unsigned RenderContext::framebufferHeight() const {
	return m_framebufferHeight;
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
	
#ifdef ANDROID
	if (DEBUG_OPTIONS_CONTAINS(options, DEBUG_OPTIONS::SHOW_WIREFRAMES)) {
		throw Exception("DEBUG_OPTIONS::SHOW_WIREFRAMES not supported on this platform.");
	}
	if (DEBUG_OPTIONS_CONTAINS(options, DEBUG_OPTIONS::SHOW_BOUNDING_BOXES)) {
		throw Exception("DEBUG_OPTIONS::SHOW_BOUNDING_BOXES not supported on this platform.");
	}
#endif
	
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
		for (auto node: m_scene->rootNode()->children(true)) {
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
	
	if (!m_recordingGIF) {
		AE_LOG->info("Starting GIF recording...");
		
		m_gifRecordingMaxFramerate = maxFramerate;
		m_gifRecordedFrames = 0;
		
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
		
		//m_gifWriter = (GifWriter *)malloc(sizeof(GifWriter));
		m_gifWriter = make_shared<GifWriter>();
		// gif-h frame time is in 100ths of a second
		GifBegin(m_gifWriter.get(), path.string().c_str(), 
				 m_gifRecordingWidth, m_gifRecordingHeight, 
				 frameTimeHS);
		
		m_recordingGIF = true;
	}
}

unsigned RenderContext::recordedGIFFrames() const {
	return m_gifRecordedFrames; 
}

void RenderContext::stopGIFRecording() {
	if (m_recordingGIF) {
		m_recordingGIF = false;
		
		GifEnd(m_gifWriter.get());
		// crashing... but it doesn't look like GifEnd() frees everything,
		// just the main buffer.
		//free(m_gifWriter.get());
		m_gifWriter = nullptr;
		
		AE_LOG->info("Stopped GIF recording.");
	}
}

RenderContext::UpdateFunction RenderContext::updateCallback() const {
	return m_updateCallback;
}

void RenderContext::updateCallback(RenderContext::UpdateFunction function) {
	m_updateCallback = function;
}

RenderContext::DidSimulatePhysicsFunction RenderContext::didSimulatePhysicsCallback() const {
	return m_didSimulatePhysicsCallback;
}

void RenderContext::didSimulatePhysicsCallback(RenderContext::DidSimulatePhysicsFunction function) {
	m_didSimulatePhysicsCallback = function;
}

RenderContext::WillRenderFunction RenderContext::willRenderCallback() const {
	return m_willRenderCallback;
}

void RenderContext::willRenderCallback(RenderContext::WillRenderFunction function) {
	m_willRenderCallback = function;
}

RenderContext::DidRenderFunction RenderContext::didRenderCallback() const {
	return m_didRenderCallback;
}

void RenderContext::didRenderCallback(RenderContext::DidRenderFunction function) {
	m_didRenderCallback = function;
}

/**************************************************************************************
     Internal
 **************************************************************************************/

void RenderContext::update() {

}

shared_ptr<PhysicsSimulator> RenderContext::physicsSimulator() const {
	return m_physicsSimulator;
}

void RenderContext::physicsSimulator(shared_ptr<PhysicsSimulator> physicsSimulator) {
	m_physicsSimulator = physicsSimulator;
}

void RenderContext::width(unsigned width) {
	m_width = width;
	framebufferWidth(m_width * m_framebufferScale);
}

void RenderContext::height(unsigned height) {
	m_height = height;
	framebufferHeight(m_height * m_framebufferScale);
}

void RenderContext::framebufferScale(unsigned scale) {
	m_framebufferScale = scale;
}

void RenderContext::framebufferWidth(unsigned width) {
	m_framebufferWidth = width;
}

void RenderContext::framebufferHeight(unsigned height) {
	m_framebufferHeight = height;
}

/**************************************************************************************
     Protected
 **************************************************************************************/

void RenderContext::saveGIFFrame(float deltaSeconds) {
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
		GifWriteFrame(m_gifWriter.get(), resizedFrameData,
					  m_gifRecordingWidth, m_gifRecordingHeight,
					  (secondsAccum*1000.0)/10.0);
		
		++m_gifRecordedFrames;
		
		//secondsAccum = secondsAccum - frameTimeMS/1000.0;
		secondsAccum = 0;
	}
}

shared_ptr<Node> RenderContext::defaultPointOfView() {
	
	auto cameraNode = make_shared<Node>();
	//m_scene->rootNode()->addChild(cameraNode); // done below
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
	
	scene()->rootNode()->addChild(cameraNode);
	pointOfView(cameraNode);
	
	return cameraNode;
}
