//
//  RenderContext.h
//  avara3d
//
//  Created by Morgan Davis on 4/24/18.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_RENDER_CONTEXT_RENDERCONTEXT_H
#define AVARA3D_RENDER_CONTEXT_RENDERCONTEXT_H

#include <filesystem>
#include <memory>

#include "a3d/Math.h"

struct GifWriter;

namespace a3d {

    class Camera;
    class Image;
    class Material;
    class Node;
    class Renderer;
    class RenderContext;
    class Scene;
    class VisualWorld;

    class RenderContext {

    public:
        /// Public Types ///

        enum class AntialiasingMode : uint8_t {
            None    = 0,
            Msaa2X  = 2,
            Msaa4X  = 4,
            Msaa8X  = 8,
            Msaa16X = 16
        };

        /// Public Member Functions ///

        virtual bool           vSyncEnabled() const       = 0;
        virtual void           vSyncEnabled(bool enabled) = 0;

        AntialiasingMode       antialiasingMode() const;

        std::unique_ptr<Image> snapshot() const;

        virtual bool           recordingGIF() const;
        virtual void           startGIFRecording(const std::filesystem::path& path,
                                                 math::uvec2                  fitInside,
                                                 unsigned                     maxFramerate);
        virtual double         recordedGIFTime() const;
        virtual unsigned       recordedGIFFrames() const;
        virtual void           stopGIFRecording();

        VisualWorld*           visualWorld() const;

        Renderer*              renderer() const;

        /// Internal Types ///

        enum class RenderingApi : uint8_t {
            OpenGL,
            OpenGLES,
            Vulkan
        };

        /// Internal Lifecycle Functions ///

        explicit RenderContext(RenderingApi renderingApi);

        RenderContext(const RenderContext&)            = delete;
        RenderContext& operator=(const RenderContext&) = delete;

        RenderContext(RenderContext&&)            = delete;
        RenderContext& operator=(RenderContext&&) = delete;

        virtual ~RenderContext();

        /// Internal Member Functions ///

        virtual void        pollEvents();

        virtual void        beginFrame(const Scene& scene) = 0;
        virtual void        endFrame(const Scene& scene)   = 0;

        virtual void        swapBuffers() = 0;

        virtual math::uvec2 viewportLogicalSize() const = 0; // DIPs
        virtual math::uvec2 framebufferSize() const     = 0;
        math::vec2          viewportScale() const;

        virtual void        saveGIFFrame(double updateDeltaTime);

        void                attachedToVisualWorld(VisualWorld* world);
        void                detachedFromVisualWorld(VisualWorld* world);

        virtual unsigned    defaultFramebuffer() const = 0;

    protected:
        /// Protected Member Variables ///

        AntialiasingMode           _antialiasingMode;
        std::unique_ptr<GifWriter> _gifWriter;
        bool                       _recordingGIF;
        unsigned                   _gifRecordingWidth;
        unsigned                   _gifRecordingHeight;
        unsigned                   _gifRecordingMaxFramerate;
        double                     _gifRecordingCurrentFrameTimeAccum;
        double                     _gifRecordedTime;
        unsigned                   _gifRecordedFrames;
        VisualWorld*               _visualWorld;
        std::unique_ptr<Renderer>  _renderer;
    };

}

#endif //AVARA3D_RENDER_CONTEXT_RENDERCONTEXT_H
