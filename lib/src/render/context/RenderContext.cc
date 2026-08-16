//
//  RenderContext.cc
//  avara3d
//
//  Created by Morgan Davis on 4/24/18.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#include "a3d/render/context/RenderContext.h"

#include <stdexcept>

#include <gif-h/gif.h>
#include <stb/image_resize2.h>

#include "a3d/Buffer.h"
#include "a3d/Image.h"
#include "a3d/log/Log.h"
#include "a3d/render/Renderer.h"
#include "a3d/render/backend/opengl/OGLRenderer.h"
#include "a3d/scene/Node.h"
#include "a3d/visual/camera/PerspectiveCamera.h"

using namespace a3d;
using namespace a3d::math;
using namespace std;

/// Public Member Functions ///

//bool RenderContext::vSyncEnabled() const {
//	return _vSyncEnabled;
//}
//
//void RenderContext::vSyncEnabled(bool enabled) {
//	_vSyncEnabled = enabled;
//}

RenderContext::Antialiasing RenderContext::antialiasing() const {
    return _antialiasing;
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

void RenderContext::startGIFRecording(const filesystem::path& path, uvec2 fitInside, unsigned maxFramerate) {

    if (!_recordingGIF) {
        log::i()("Starting GIF recording...");

        _gifRecordingMaxFramerate = maxFramerate;
        _gifRecordingCurrentFrameTimeAccum = 0;
        _gifRecordedTime = 0;
        _gifRecordedFrames = 0;

        // nice! https://math.stackexchange.com/questions/1169409/formula-to-best-fit-a-rectangle-inside-another-by-scaling
        auto fbSize = framebufferSize();
        auto scale = math::min(float(fitInside.x) / float(fbSize.x), float(fitInside.y) / float(fbSize.y));
        _gifRecordingWidth = (unsigned) math::round(float(fbSize.x) * scale);
        _gifRecordingHeight = (unsigned) math::round(float(fbSize.y) * scale);

        unsigned frameTimeMS = 1000 /* (ms/sec) */ / _gifRecordingMaxFramerate /* (frames/sec) */;
        // -> ms/frame
        unsigned frameTimeHS = (unsigned) round((float) frameTimeMS / 10.0); // 100th sec/frame

        //_gifWriter = (GifWriter *)malloc(sizeof(GifWriter));
        _gifWriter = make_unique<GifWriter>();
        // gif-h frame time is in 100ths of a second
        GifBegin(_gifWriter.get(), path.string().c_str(), _gifRecordingWidth, _gifRecordingHeight, frameTimeHS);

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

        log::i()("Stopped GIF recording.");
    }
}

VisualWorld* RenderContext::visualWorld() const {
    return _visualWorld;
}

Renderer* RenderContext::renderer() const {
    return _renderer.get();
}

/// Internal Lifescycle ///

RenderContext::RenderContext(RenderingApi renderingApi):
    //		_vSyncEnabled{false},
    _antialiasing {Antialiasing::None},
    _gifWriter {},
    _recordingGIF {false},
    _gifRecordingWidth {0},
    _gifRecordingHeight {0},
    _gifRecordingMaxFramerate {0},
    _gifRecordingCurrentFrameTimeAccum {0},
    _gifRecordedTime {0},
    _gifRecordedFrames {0},
    _visualWorld {} /*_renderer{}*/ {

    switch (renderingApi) {
        case RenderingApi::OpenGL: {
            _renderer = make_unique<OGLRenderer>();
            break;
        }
        case RenderingApi::OpenGLES: {
            throw std::runtime_error("Unsupported rendering API: OpenGLES");
            break;
        }
        case RenderingApi::Vulkan: {
            throw std::runtime_error("Unsupported rendering API: Vulkan");
            break;
        }
    }
}

RenderContext::~RenderContext() {
    log::d()("Destroying RenderContext {:p}", static_cast<void*>(this));

    if (_recordingGIF) {
        stopGIFRecording();
    }
}

/// Internal Member Functions ///

void RenderContext::pollEvents() {}

void RenderContext::saveGIFFrame(double updateDeltaTime) {

    _gifRecordedTime += updateDeltaTime;
    _gifRecordingCurrentFrameTimeAccum += updateDeltaTime;

    float frameTimeMS = 1000.0f /* (ms/sec) */ / (float) _gifRecordingMaxFramerate /* (frames/sec) */;

    if (_gifRecordingCurrentFrameTimeAccum >= frameTimeMS / 1000.0) {

        auto frame = snapshot();

        auto resizedFrameData = (unsigned char*) malloc(_gifRecordingWidth * _gifRecordingHeight * 4);
        stbir_resize_uint8_linear(reinterpret_cast<const unsigned char*>(frame->buffer().data()),
                                  int(frame->width()), int(frame->height()), 0, resizedFrameData,
                                  (int) _gifRecordingWidth, (int) _gifRecordingHeight, 0, STBIR_RGBA);

        // gif-h frame time is in 100ths of a second
        GifWriteFrame(_gifWriter.get(), resizedFrameData, _gifRecordingWidth, _gifRecordingHeight,
                      (uint32_t) round((_gifRecordingCurrentFrameTimeAccum * 1000.0f) / 10.0f));

        ++_gifRecordedFrames;

        _gifRecordingCurrentFrameTimeAccum = 0;
    }
}

vec2 RenderContext::viewportScale() const {
    // see note in GLFWWindow::viewportLogicalSize().
    // on Windows and X11, DPI/glfwGetWindowContentScale() is mostly a "UI" scaling hint.
    uvec2 fbSize = framebufferSize();
    uvec2 vpLogicalSize = viewportLogicalSize();
    return vec2(float(fbSize.x) / float(vpLogicalSize.x), float(fbSize.y) / float(vpLogicalSize.y));
}

void RenderContext::attachedToVisualWorld(VisualWorld* world) {
    log::t()("world: {:p}", static_cast<void*>(world));

    _visualWorld = world;
}

void RenderContext::detachedFromVisualWorld(VisualWorld* world) {
    log::t()("world: {:p}", static_cast<void*>(world));

    _visualWorld = nullptr;
}
