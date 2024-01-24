//
//  RenderContext.cc
//	avara-engine
//
//  Created by Morgan Davis on 4/24/18.
//  Copyright © 2018 Morgan K Davis. All rights reserved.
//

#include "ae/rendering/context/RenderContext.h"

#include "gif.h"
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb_image_resize2.h"

#include "ae/Buffer.h"
#include "ae/Image.h"
#include "ae/diagnostic/Exception.h"
#include "ae/diagnostic/logging/Logger.h"
#include "ae/rendering/opengl/OpenGLRenderer.h"
#include "ae/rendering/camera/PerspectiveCamera.h"
#include "ae/rendering/Renderer.h"
#include "ae/scene/Node.h"
#include "ae/scene/Scene.h"


using namespace ae;
using namespace glm;
using namespace std;


/*********************************************************************************************
	Lifescycle
 *********************************************************************************************/

RenderContext::RenderContext(RENDER_API renderAPI):
		_renderAPI(renderAPI),
		_renderer(nullptr),
		_width(0),
		_height(0),
		_framebufferWidth(0),
		_framebufferHeight(0),
		_framebufferScale{1.0, 1.0},
		_vSyncEnabled(false),
		_antialiasingMode(ANTIALIASING_MODE::NONE),
		_gifWriter(nullptr),
		_recordingGIF(false),
		_gifRecordingWidth(0),
		_gifRecordingHeight(0),
		_gifRecordingMaxFramerate(0),
		_gifRecordedFrames(0),
		_visualWorld(nullptr) {

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
	AE_LOG_D("Destroying RenderContext {:p}", static_cast<void*>(this));
	
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

unsigned RenderContext::width() const {
	return _width;
}

unsigned RenderContext::height() const {
	return _height;
}

unsigned RenderContext::framebufferWidth() const {
	return _framebufferWidth;
}

unsigned RenderContext::framebufferHeight() const {
	return _framebufferHeight;
}

vec2 RenderContext::framebufferScale() const {
	return _framebufferScale;
}

bool RenderContext::vSyncEnabled() const {
	return _vSyncEnabled;
}

void RenderContext::vSyncEnabled(bool enabled) {
	_vSyncEnabled = enabled;
}

ANTIALIASING_MODE RenderContext::antialiasingMode() const {
	return _antialiasingMode;
}

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

VisualWorld* RenderContext::visualWorld() const {
	return _visualWorld;
}

/*********************************************************************************************
	Internal
 *********************************************************************************************/

shared_ptr<Renderer> RenderContext::renderer() const {
	return _renderer;
}

void RenderContext::width(unsigned width) {
	_width = width;
	framebufferWidth(_width * _framebufferScale.x);
}

void RenderContext::height(unsigned height) {
	_height = height;
	framebufferHeight(_height * _framebufferScale.y);
}

void RenderContext::framebufferWidth(unsigned width) {
	_framebufferWidth = width;
}

void RenderContext::framebufferHeight(unsigned height) {
	_framebufferHeight = height;
}

void RenderContext::framebufferScale(vec2 scale) {
	_framebufferScale = scale;
}

void RenderContext::saveGIFFrame(float deltaRunT) {

	static float secondsAccum = 0; // TODO: this won't work correctly after first call
	secondsAccum += deltaRunT;

	unsigned frameTimeMS = 1000.0 /* (ms/sec) */ / _gifRecordingMaxFramerate /* (frames/sec) */;
	// -> ms/frame
	//unsigned frameTimeHS = frameTimeMS / 10.0; // 100th sec/frame

	//unsigned frameTime = 1000.0/_gifRecordingMaxFramerate; // ms/frame

	if (secondsAccum >= frameTimeMS/1000.0) {

		auto frame = snapshot();

		auto resizedFrameData = (unsigned char*)malloc(_gifRecordingWidth * _gifRecordingHeight * 4);
		stbir_resize_uint8_linear(frame->buffer()->data(), frame->width(), frame->height(), 0,
								  resizedFrameData, _gifRecordingWidth, _gifRecordingHeight, 0,
								  STBIR_RGBA);

		// gif-h frame time is in 100ths of a second
		GifWriteFrame(_gifWriter.get(), resizedFrameData,
					  _gifRecordingWidth, _gifRecordingHeight,
					  (secondsAccum*1000.0)/10.0);

		++_gifRecordedFrames;

		//secondsAccum = secondsAccum - frameTimeMS/1000.0;
		secondsAccum = 0;
	}
}

void RenderContext::attachedToVisualWorld(VisualWorld* world) {
	AE_LOG_T("world: {:p}", static_cast<void*>(world));

	_visualWorld = world;
}

void RenderContext::detachedFromVisualWorld(VisualWorld* world) {
	AE_LOG_T("world: {:p}", static_cast<void*>(world));

	_visualWorld = nullptr;
}
