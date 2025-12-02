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
#include "a3d/diagnostic/log/Log.h"
#include "a3d/rendering/camera/PerspectiveCamera.h"
#include "a3d/rendering/renderer/Renderer.h"
#include "a3d/rendering/renderer/opengl/OpenGlRenderer.h"
#include "a3d/scene/Node.h"
#include "a3d/scene/Scene.h"


using namespace a3d;
using namespace glm;
using namespace std;


/*********************************************************************************************
	Public Member Functions
 *********************************************************************************************/

//bool RenderContext::vSyncEnabled() const {
//	return _vSyncEnabled;
//}
//
//void RenderContext::vSyncEnabled(bool enabled) {
//	_vSyncEnabled = enabled;
//}

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
									  uvec2 fitInside,
									  unsigned maxFramerate) {
	
	if (!_recordingGIF) {
		A3D_LOG_I("Starting GIF recording...");
		
		_gifRecordingMaxFramerate = maxFramerate;
		_gifRecordingCurrentFrameTimeAccum = 0;
		_gifRecordedTime = 0;
		_gifRecordedFrames = 0;

		// nice! https://math.stackexchange.com/questions/1169409/formula-to-best-fit-a-rectangle-inside-another-by-scaling
		auto fbSize = framebufferSize();
		auto scale = std::min(float(fitInside.x)/float(fbSize.x),
							  float(fitInside.y)/float(fbSize.y));
		_gifRecordingWidth = (unsigned)round(float(fbSize.x) * scale);
		_gifRecordingHeight = (unsigned)round(float(fbSize.y) * scale);

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

double RenderContext::recordedGIFTime() const {
	return _gifRecordedTime;
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
	Internal Lifescycle
 *********************************************************************************************/

RenderContext::RenderContext(RenderingApi renderingApi):
//		_vSyncEnabled{false},
		_antialiasingMode{AntialiasingMode::None},
		_gifWriter{},
		_recordingGIF{false},
		_gifRecordingWidth{0},
		_gifRecordingHeight{0},
		_gifRecordingMaxFramerate{0},
		_gifRecordingCurrentFrameTimeAccum{0},
		_gifRecordedTime{0},
		_gifRecordedFrames{0},
		_visualWorld{}
		/*_renderer{}*/ {

	switch (renderingApi) {
		case RenderingApi::OpenGL: {
			_renderer = make_unique<OpenGlRenderer>();
			break;
		}
		case RenderingApi::OpenGLES: {
			throw Exception("Unsupported rendering API: OpenGLES");
			break;
		}
		case RenderingApi::Vulkan: {
			throw Exception("Unsupported rendering API: Vulkan");
			break;
		}
	}
}

RenderContext::~RenderContext() {
	A3D_LOG_D("Destroying RenderContext {:p}", static_cast<void*>(this));

	if (_recordingGIF) {
		stopGIFRecording();
	}
}

/*********************************************************************************************
	Internal Member Functions
 *********************************************************************************************/

void RenderContext::saveGIFFrame(double deltaRunT) {

	_gifRecordedTime += deltaRunT;
	_gifRecordingCurrentFrameTimeAccum += deltaRunT;

	float frameTimeMS = 1000.0f /* (ms/sec) */ / (float)_gifRecordingMaxFramerate /* (frames/sec) */;
	// -> ms/frame
	//unsigned frameTimeHS = frameTimeMS / 10.0; // 100th sec/frame

	//unsigned frameTime = 1000.0/_gifRecordingMaxFramerate; // ms/frame

	if (_gifRecordingCurrentFrameTimeAccum >= frameTimeMS/1000.0) {

		auto frame = snapshot();

		auto resizedFrameData = (unsigned char*)malloc(_gifRecordingWidth * _gifRecordingHeight * 4);
		stbir_resize_uint8_linear(reinterpret_cast<const unsigned char*>(frame->buffer().data()),
								  int(frame->width()), int(frame->height()), 0,
								  resizedFrameData, (int)_gifRecordingWidth, (int)_gifRecordingHeight, 0,
								  STBIR_RGBA);

		// gif-h frame time is in 100ths of a second
		GifWriteFrame(_gifWriter.get(), resizedFrameData,
					  _gifRecordingWidth, _gifRecordingHeight,
					  (uint32_t)round((_gifRecordingCurrentFrameTimeAccum*1000.0f)/10.0f));

		++_gifRecordedFrames;

		//secondsAccum = secondsAccum - frameTimeMS/1000.0;
		_gifRecordingCurrentFrameTimeAccum = 0;
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
