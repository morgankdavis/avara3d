//
//  RenderContext.cc
//  avara3d
//
//  Created by Morgan Davis on 4/24/18.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#include "a3d/rendering/context/RenderContext.h"

#include "gif.h"
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb_image_resize2.h"

#include "a3d/Buffer.h"
#include "a3d/Image.h"
#include "a3d/diagnostic/exception/Exception.h"
#include "a3d/diagnostic/logging/Logger.h"
#include "a3d/rendering/opengl/OpenGLRenderer.h"
#include "a3d/rendering/camera/PerspectiveCamera.h"
#include "a3d/rendering/Renderer.h"
#include "a3d/scene/Node.h"
#include "a3d/scene/Scene.h"


using namespace a3d;
using namespace glm;
using namespace std;


/*********************************************************************************************
	Lifescycle
 *********************************************************************************************/

RenderContext::RenderContext(RenderingApi renderingApi):
		_width{0},
		_height{0},
		_framebufferWidth{0},
		_framebufferHeight{0},
		_framebufferScale{1.0, 1.0},
		_vSyncEnabled{false},
		_antialiasingMode{AntialiasingMode::None},
		_gifWriter{},
		_recordingGIF{false},
		_gifRecordingWidth{0},
		_gifRecordingHeight{0},
		_gifRecordingMaxFramerate{0},
		_gifRecordedFrames{0},
		_visualWorld{},
		_renderer{} {

	switch (renderingApi) {
		case RenderingApi::OpenGL: {
			_renderer = make_unique<OpenGLRenderer>();
			break; }
		case RenderingApi::OpenGLES: {
			throw Exception("Unsupported rendering API: OpenGLES");
			break; }
		case RenderingApi::Vulkan: {
			throw Exception("Unsupported rendering API: Vulkan");
			break; }
	}
}

RenderContext::~RenderContext() {
	A3D_LOG_D("Destroying RenderContext {:p}", static_cast<void*>(this));
	
	if (_recordingGIF) {
		stopGIFRecording();
	}
}

/*********************************************************************************************
	Public
 *********************************************************************************************/

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

const vec2& RenderContext::framebufferScale() const {
	return _framebufferScale;
}

bool RenderContext::vSyncEnabled() const {
	return _vSyncEnabled;
}

void RenderContext::vSyncEnabled(bool enabled) {
	_vSyncEnabled = enabled;
}

AntialiasingMode RenderContext::antialiasingMode() const {
	return _antialiasingMode;
}

unique_ptr<Image> RenderContext::snapshot() const {
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
		A3D_LOG_I("Starting GIF recording...");
		
		_gifRecordingMaxFramerate = maxFramerate;
		_gifRecordedFrames = 0;
		
		_gifRecordingHeight = _framebufferHeight;
		_gifRecordingWidth = _framebufferWidth;
		if (_gifRecordingHeight > maxHeight) {
			float scale = (float)maxHeight / (float)_framebufferHeight;
			_gifRecordingHeight = (unsigned)round((float)_framebufferHeight * scale);
			_gifRecordingWidth = (unsigned)round((float)_framebufferWidth * scale);
		}

		unsigned frameTimeMS = 1000 /* (ms/sec) */ / _gifRecordingMaxFramerate /* (frames/sec) */;
		// -> ms/frame
		unsigned frameTimeHS = (unsigned)round((float)frameTimeMS / 10.0); // 100th sec/frame
		
		//_gifWriter = (GifWriter *)malloc(sizeof(GifWriter));
		_gifWriter = make_unique<GifWriter>();
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
		
		A3D_LOG_I("Stopped GIF recording.");
	}
}

VisualWorld* RenderContext::visualWorld() const {
	return _visualWorld;
}

Renderer* RenderContext::renderer() const {
	return _renderer.get();
}

/*********************************************************************************************
	Internal
 *********************************************************************************************/

void RenderContext::width(unsigned width) {
	_width = width;
	framebufferWidth((unsigned)round((float)_width * _framebufferScale.x));
}

void RenderContext::height(unsigned height) {
	_height = height;
	framebufferHeight((unsigned)round((float)_height * _framebufferScale.y));
}

void RenderContext::framebufferWidth(unsigned width) {
	_framebufferWidth = width;
}

void RenderContext::framebufferHeight(unsigned height) {
	_framebufferHeight = height;
}

void RenderContext::framebufferScale(const vec2& scale) {
	_framebufferScale = scale;
}

void RenderContext::saveGIFFrame(float deltaRunT) {

	static float secondsAccum = 0; // TODO: this won't work correctly after first call
	secondsAccum += deltaRunT;

	float frameTimeMS = 1000.0f /* (ms/sec) */ / (float)_gifRecordingMaxFramerate /* (frames/sec) */;
	// -> ms/frame
	//unsigned frameTimeHS = frameTimeMS / 10.0; // 100th sec/frame

	//unsigned frameTime = 1000.0/_gifRecordingMaxFramerate; // ms/frame

	if (secondsAccum >= frameTimeMS/1000.0) {

		auto frame = snapshot();

		auto resizedFrameData = (unsigned char*)malloc(_gifRecordingWidth * _gifRecordingHeight * 4);
		stbir_resize_uint8_linear(reinterpret_cast<const unsigned char*>(frame->buffer().data()),
								  int(frame->width()), int(frame->height()), 0,
								  resizedFrameData, (int)_gifRecordingWidth, (int)_gifRecordingHeight, 0,
								  STBIR_RGBA);

		// gif-h frame time is in 100ths of a second
		GifWriteFrame(_gifWriter.get(), resizedFrameData,
					  _gifRecordingWidth, _gifRecordingHeight,
					  (uint32_t)round((secondsAccum*1000.0f)/10.0f));

		++_gifRecordedFrames;

		//secondsAccum = secondsAccum - frameTimeMS/1000.0;
		secondsAccum = 0;
	}
}

void RenderContext::attachedToVisualWorld(VisualWorld* world) {
	A3D_LOG_T("world: {:p}", static_cast<void*>(world));

	_visualWorld = world;
}

void RenderContext::detachedFromVisualWorld(VisualWorld* world) {
	A3D_LOG_T("world: {:p}", static_cast<void*>(world));

	_visualWorld = nullptr;
}
