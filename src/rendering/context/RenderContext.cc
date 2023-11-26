//
//  RenderContext.cc
//	avara-engine
//
//  Created by Morgan Davis on 4/24/18.
//  Copyright © 2018 Morgan K Davis. All rights reserved.
//

#include "rendering/context/RenderContext.h"

//#include <chrono>

#include "gif.h"
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb_image_resize.h"

#include "diagnostic/Exception.h"
#include "diagnostic/logging/Logger.h"
#include "physics/bullet/BulletPhysicsSimulator.h"
#include "rendering/opengl/OpenGLRenderer.h"
#include "rendering/camera/PerspectiveCamera.h"
#include "rendering/Renderer.h"
#include "scene/Node.h"
#include "scene/Scene.h"
#include "utilities/Buffer.h"
#include "utilities/Image.h"


using namespace ae;
using namespace glm;
using namespace std;


/*********************************************************************************************
	Lifescycle
 *********************************************************************************************/

RenderContext::RenderContext(RENDER_API renderAPI):
	_renderAPI(renderAPI),
	_renderer(nullptr),
//	_physicsSimulator(make_shared<BulletPhysicsSimulator>()),
//	_scene(nullptr),
	_width(0),
	_height(0),
	_framebufferScale(1),
	_framebufferWidth(0),
	_framebufferHeight(0),
	_vSyncEnabled(false),
	_antialiasingMode(ANTIALIASING_MODE::NONE),
	_debugOptions(DEBUG_OPTIONS::NONE),
//	_pointOfView(nullptr),
	_gifWriter(nullptr),
	_recordingGIF(false),
	_gifRecordingWidth(0),
	_gifRecordingHeight(0),
	_gifRecordingMaxFramerate(0),
	_gifRecordedFrames(0) {
//	_update(nullptr),
//	_didSimulate(nullptr),
//	_willRender(nullptr),
//	_didRender(nullptr) {

		switch (_renderAPI) {
			case RENDER_API::OPENGL: {
				auto renderer = make_shared<OpenGLRenderer>();
				_renderer = static_pointer_cast<Renderer>(renderer);
				break; }
			case RENDER_API::OPENGL_ES: {
				throw Exception("Unsupported render API: OPENGL_ES");
				break; }
			case RENDER_API::VULKAN: {
				throw Exception("Unsupported render API: VULKAN");
				break; }
		}		
}

RenderContext::~RenderContext() {
	AE_LOG_D("Destroying RenderContext {:p}", (void*)this);
	
	if (_recordingGIF) {
		stopGIFRecording();
	}
}

/*********************************************************************************************
	Public
 *********************************************************************************************/

RENDER_API RenderContext::renderAPI() const {
	return _renderAPI;
}

//shared_ptr<Scene> RenderContext::scene() const {
//	//return _renderer->scene();
//	return _scene;
//}
//
//void RenderContext::scene(const shared_ptr<Scene> scene) {
//	//_renderer->scene(scene);
//	_scene = scene;
////	scene->attachedToRenderContext(shared_from_this());
//}

unsigned RenderContext::width() const {
	return _width;
}

unsigned RenderContext::height() const {
	return _height;
}

float RenderContext::framebufferScale() const {
	return _framebufferScale;
}

unsigned RenderContext::framebufferWidth() const {
	return _framebufferWidth;
}

unsigned RenderContext::framebufferHeight() const {
	return _framebufferHeight;
}

bool RenderContext::vSyncEnabled() const {
	return _vSyncEnabled;
}

void RenderContext::enableVSync(bool enabled) {
	_vSyncEnabled = enabled;
}

DEBUG_OPTIONS RenderContext::debugOptions() const {
	return _debugOptions;
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
	
	_debugOptions = options;
	
//	if (_scene && _scene->physicalWorld()) {
//		_scene->physicalWorld()->debugOptions(_debugOptions);
//	}
}

//shared_ptr<Node> RenderContext::pointOfView() {
//
//	if (_pointOfView) {
//		return _pointOfView;
//	}
//	else {
//		// try to assign one from the scene
//		for (auto node: _scene->rootNode()->children(true)) {
//			if (node->camera()) {
//				_pointOfView = node;
//				return _pointOfView;
//			}
//		}
//	}
//	if (!_pointOfView) {
//		// still no POV. add a default one.
//		_pointOfView = defaultPointOfView();
//	}
//
//	return _pointOfView;
//}
//
//void RenderContext::pointOfView(const shared_ptr<Node> camera) {
//	_pointOfView = camera;
//}

ANTIALIASING_MODE RenderContext::antialiasingMode() const {
	return _antialiasingMode;
}

//shared_ptr<InputManager> RenderContext::inputManager() { // pure virtual
//	AE_LOG_C("RenderContext::inputManager() should be overidden in derived class.");
//	return nullptr;
//}

//float RenderContext::sceneTime() const {
//	// should probably override in subclass to use library's time utilities (GLFW, for example)
//	static auto startDate = chrono::high_resolution_clock::now();
//	auto nowDate = chrono::high_resolution_clock::now();
//	return (chrono::duration<float>(nowDate - startDate)).count();
//}

shared_ptr<Image> RenderContext::snapshot() const {
	if (_renderer) {
		return _renderer->snapshot(*this);
	}
	return nullptr;
}

bool RenderContext::recordingGIF() const {
	return _recordingGIF;
}

void RenderContext::startGIFRecording(const filesystem::path& path,
									  unsigned maxHeight, unsigned maxFramerate) {
	
	if (!_recordingGIF) {
		AE_LOG_I("Starting GIF recording...");
		
		_gifRecordingMaxFramerate = maxFramerate;
		_gifRecordedFrames = 0;
		
		_gifRecordingHeight = _framebufferHeight;
		_gifRecordingWidth = _framebufferWidth;
		if (_gifRecordingHeight > maxHeight) {
			float scale = (float)maxHeight / (float)_framebufferHeight;
			_gifRecordingHeight = _framebufferHeight * scale;
			_gifRecordingWidth = _framebufferWidth * scale;
		}
		
		unsigned frameTimeMS = 1000.0 /* (ms/sec) */ / _gifRecordingMaxFramerate /* (frames/sec) */;
		// -> ms/frame
		unsigned frameTimeHS = frameTimeMS / 10.0; // 100th sec/frame
		
		//_gifWriter = (GifWriter *)malloc(sizeof(GifWriter));
		_gifWriter = make_shared<GifWriter>();
		// gif-h frame time is in 100ths of a second
		GifBegin(_gifWriter.get(), path.string().c_str(),
				 _gifRecordingWidth, _gifRecordingHeight,
				 frameTimeHS);
		
		_recordingGIF = true;
	}
}

unsigned RenderContext::recordedGIFFrames() const {
	return _gifRecordedFrames;
}

void RenderContext::stopGIFRecording() {
	if (_recordingGIF) {
		_recordingGIF = false;
		
		GifEnd(_gifWriter.get());
		// crashing... but it doesn't look like GifEnd() frees everything,
		// just the main buffer.
		//free(_gifWriter.get());
		_gifWriter = nullptr;
		
		AE_LOG_I("Stopped GIF recording.");
	}
}

//RenderContext::UpdateCallback RenderContext::update() const {
//	return _update;
//}
//
//void RenderContext::update(RenderContext::UpdateCallback function) {
//	_update = function;
//}

//RenderContext::DidSimulateCallback RenderContext::didSimulate() const {
//	return _didSimulate;
//}
//
//void RenderContext::didSimulate(RenderContext::DidSimulateCallback function) {
//	_didSimulate = function;
//}

//RenderContext::WillRenderCallback RenderContext::willRender() const {
//	return _willRender;
//}
//
//void RenderContext::willRender(RenderContext::WillRenderCallback function) {
//	_willRender = function;
//}
//
//RenderContext::DidRenderCallback RenderContext::didRender() const {
//	return _didRender;
//}
//
//void RenderContext::didRender(RenderContext::DidRenderCallback function) {
//	_didRender = function;
//}

weak_ptr<Scene> RenderContext::scene() const {
	return _scene;
}

void RenderContext::scene(weak_ptr<Scene> scene) {
	_scene = scene;
}

weak_ptr<VisualWorld> RenderContext::visualWorld() const {
	return _visualWorld;
}

void RenderContext::visualWorld(weak_ptr<VisualWorld> world) {
	_visualWorld = world;
}

/*********************************************************************************************
	Internal
 *********************************************************************************************/

shared_ptr<Renderer> RenderContext::renderer() const {
	return _renderer;
}

//void RenderContext::renderer(shared_ptr<Renderer> renderer) {
//	_renderer = renderer;
//}

void RenderContext::update() { // pure virtual
//	//AE_LOG_T("-------------------------------------------------------------------------------");
//
//	if (updateCallback()) {
//		(updateCallback())(*this, sceneTime());
//	}
//
//	//_renderStats = (FrameStats){};
//
//	_renderer->beginFrame(*this);
//
//	auto pov = pointOfView();
//	float aspectRatio = (float)_framebufferWidth/(float)_framebufferHeight;
//	static_pointer_cast<PerspectiveCamera>(pov->camera())->aspectRatio(aspectRatio);
//
//	_renderer->frameStats().cameraPosition = pov->position();
//
//	if (willRenderCallback()) {
//		(willRenderCallback())(*this, sceneTime());
//	}
//
//	_scene->update(*RenderContext::renderer(),
//				   _framebufferWidth, _framebufferHeight,
//				   *pov,
//				   _debugOptions,
//				   _renderer->frameStats());
//
//	_renderer->endFrame(*this);
//
//	swapBuffers();
//
//	if (_recordingGIF) {
//		saveGIFFrame(sceneTime());
//	}
//
//	if (didRenderCallback()) {
//		(didRenderCallback())(*this, sceneTime());
//	}
//
//	pollInput();
}
//
//shared_ptr<PhysicsSimulator> RenderContext::physicsSimulator() const {
//	return _physicsSimulator;
//}
//
//void RenderContext::physicsSimulator(shared_ptr<PhysicsSimulator> physicsSimulator) {
//	_physicsSimulator = physicsSimulator;
//}

void RenderContext::width(unsigned width) {
	_width = width;
	framebufferWidth(_width * _framebufferScale);
}

void RenderContext::height(unsigned height) {
	_height = height;
	framebufferHeight(_height * _framebufferScale);
}

void RenderContext::framebufferScale(float scale) {
	_framebufferScale = scale;
}

void RenderContext::framebufferWidth(unsigned width) {
	_framebufferWidth = width;
}

void RenderContext::framebufferHeight(unsigned height) {
	_framebufferHeight = height;
}

/*********************************************************************************************
	Protected
 *********************************************************************************************/

void RenderContext::saveGIFFrame(float time) {

//	float time = sceneTime();
	static float previousSeconds = time;
	float deltaSeconds = time - previousSeconds;
	previousSeconds = time;
	
	static float secondsAccum = 0;
	secondsAccum += deltaSeconds;
	
	unsigned frameTimeMS = 1000.0 /* (ms/sec) */ / _gifRecordingMaxFramerate /* (frames/sec) */;
	// -> ms/frame
	//unsigned frameTimeHS = frameTimeMS / 10.0; // 100th sec/frame
	
	//unsigned frameTime = 1000.0/_gifRecordingMaxFramerate; // ms/frame
	
	if (secondsAccum >= frameTimeMS/1000.0) {
		
		auto frame = snapshot();
		
		unsigned char* resizedFrameData = (unsigned char*)malloc(_gifRecordingWidth * _gifRecordingHeight * 4);
		stbir_resize_uint8(frame->data()->pointer(), frame->width(), frame->height(), 0,
						   resizedFrameData, _gifRecordingWidth, _gifRecordingHeight, 0, 4);
		
		// gif-h frame time is in 100ths of a second
		GifWriteFrame(_gifWriter.get(), resizedFrameData,
					  _gifRecordingWidth, _gifRecordingHeight,
					  (secondsAccum*1000.0)/10.0);
		
		++_gifRecordedFrames;
		
		//secondsAccum = secondsAccum - frameTimeMS/1000.0;
		secondsAccum = 0;
	}
}

//shared_ptr<Node> RenderContext::defaultPointOfView() {
//
//	auto cameraNode = make_shared<Node>();
//	//_scene->rootNode()->addChild(cameraNode); // done below
//	auto camera = make_shared<PerspectiveCamera>();
//	camera->name("default camera");
//	cameraNode->camera(camera);
//
//	auto boundingPoints = (*scene()->aabb());
//
//	float fovH = static_pointer_cast<PerspectiveCamera>(cameraNode->camera())->fov();
//	float w = width();
//	float h = height();
//	float aspectRatio = w/h;
//	float inverseAspectRatio = 1.0f/aspectRatio;
//	float fovV = fovH * inverseAspectRatio;
//
//	// tan(angle) = x/z
//	// ztan(angle) = x
//	// z = x/tan(angle)
//
//	float maxZ = abs(boundingPoints["zMax"].z);
//
//	float xH = abs(boundingPoints["xMin"].x) + abs(boundingPoints["xMax"].x) / 2.0f;
//	float angleH = fovH / 2.0;
//	float zH = xH / tan(angleH);
//
//	float xV = abs(boundingPoints["yMin"].y) + abs(boundingPoints["yMax"].y) / 2.0f;
//	float angleV = fovV / 2.0;
//	float zV = xV / tan(angleV);
//
//	zH += maxZ;
//	zV += maxZ;
//
//	float z = fmax(zH, zV);
//	float midX = (boundingPoints["xMin"].x + boundingPoints["xMax"].x) / 2.0f;
//	float midY = (boundingPoints["yMin"].y + boundingPoints["yMax"].y) / 2.0f;
//
//	vec3 eye = vec3(midX, midY, z / 2.0f); // not sure why z is devided by 2.0, but it seems to work better...
//	//vec3 eye = vec3(midX, midY, z);
//
//	mat4 viewMat = translate(mat4(1.0f), eye);
//	cameraNode->transform(viewMat);
//
//	scene()->rootNode()->addChild(cameraNode);
//	pointOfView(cameraNode);
//
//	return cameraNode;
//}

//shared_ptr<Node> RenderContext::defaultPointOfView() {
//
//	auto cameraNode = make_shared<Node>();
//	//_scene->rootNode()->addChild(cameraNode); // done below
//	auto camera = make_shared<PerspectiveCamera>();
//	camera->name("default camera");
//	cameraNode->camera(camera);
//
//	auto aabb = scene()->rootNode()->aabb();
//
//	float fovH = static_pointer_cast<PerspectiveCamera>(cameraNode->camera())->fov();
//	float w = width();
//	float h = height();
//	float aspectRatio = w/h;
//	float inverseAspectRatio = 1.0f/aspectRatio;
//	float fovV = fovH * inverseAspectRatio;
//
//	// tan(angle) = x/z
//	// ztan(angle) = x
//	// z = x/tan(angle)
//
//	float maxZ = abs(aabb.max.z);
//
//	float xH = abs(aabb.min.x) + abs(aabb.max.x) / 2.0f;
//	float angleH = fovH / 2.0;
//	float zH = xH / tan(angleH);
//
//	float xV = abs(aabb.min.y) + abs(aabb.max.y) / 2.0f;
//	float angleV = fovV / 2.0;
//	float zV = xV / tan(angleV);
//
//	zH += maxZ;
//	zV += maxZ;
//
//	float z = fmax(zH, zV);
//	float midX = (aabb.min.x + aabb.max.x) / 2.0f;
//	float midY = (aabb.min.y + aabb.max.y) / 2.0f;
//
//	vec3 eye = vec3(midX, midY, z / 2.0f); // not sure why z is devided by 2.0, but it seems to work better...
//	//vec3 eye = vec3(midX, midY, z);
//
//	mat4 viewMat = translate(mat4(1.0f), eye);
//	cameraNode->transform(viewMat);
//
//	scene()->rootNode()->addChild(cameraNode);
//	pointOfView(cameraNode);
//
//	return cameraNode;
//}
