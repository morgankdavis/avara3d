//
//  RenderContext.h
//  avara3d
//
//  Created by Morgan Davis on 4/24/18.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_RENDER_CONTEXT_RENDERCONTEXT_H
#define AVARA3D_RENDER_CONTEXT_RENDERCONTEXT_H

#include <memory>

#include "a3d/Math.h"

namespace a3d {
class Camera;
class Image;
class Material;
class Node;
class Renderer;
class RenderContext;
class Scene;
class VisualWorld;

/**
 * @brief Abstract rendering destination used by VisualWorld.
 *
 * A RenderContext owns the renderer resources associated with a rendering target.
 * A VisualWorld constructed with the context registers itself with the context;
 * visualWorld() exposes the currently registered attachment non-owningly.
 */
class RenderContext {

public:
    // [Public Types]

    /** @brief Selects multisample antialiasing requested for the rendering target. */
    enum class Antialiasing : uint8_t {
        None    = 0,  ///< No multisample antialiasing.
        Msaa2X  = 2,  ///< Two samples per pixel.
        Msaa4X  = 4,  ///< Four samples per pixel.
        Msaa8X  = 8,  ///< Eight samples per pixel.
        Msaa16X = 16  ///< Sixteen samples per pixel.
    };

    // [Public Member Functions]

    /** @brief Returns whether presentation synchronization is enabled for this context. */
    virtual bool           vSyncEnabled() const = 0;

    /** @brief Enables or disables presentation synchronization when supported by the concrete context. */
    virtual void           vSyncEnabled(bool enabled) = 0;

    /** @brief Returns the antialiasing mode selected when the rendering target was created. */
    Antialiasing           antialiasing() const;

    /**
     * @brief Captures the current framebuffer into a new Image at framebuffer resolution.
     *
     * @return The captured Image, or nullptr if no renderer is available.
     */
    std::unique_ptr<Image> snapshot() const;

    /** @brief Returns the attached VisualWorld, or nullptr if none is attached. */
    VisualWorld*           visualWorld() const;

    // [Internal Lifecycle Functions]

    RenderContext();

    RenderContext(const RenderContext&)            = delete;
    RenderContext& operator=(const RenderContext&) = delete;

    RenderContext(RenderContext&&)            = delete;
    RenderContext& operator=(RenderContext&&) = delete;

    virtual ~RenderContext();

    // [Internal Member Functions]

    virtual void        pollEvents();

    virtual void        beginFrame(const Scene& scene) = 0;
    virtual void        endFrame(const Scene& scene)   = 0;

    virtual void        swapBuffers() = 0;

    virtual math::uvec2 viewportLogicalSize() const = 0; // DIPs
    virtual math::uvec2 framebufferSize() const     = 0;
    math::vec2          viewportScale() const;

    void                attachedToVisualWorld(VisualWorld* world);
    void                detachedFromVisualWorld(VisualWorld* world);

    virtual unsigned    defaultFramebuffer() const = 0;

    Renderer*           renderer() const;

protected:
    // [Protected Member Variables]

    Antialiasing              _antialiasing;
    VisualWorld*              _visualWorld;
    std::unique_ptr<Renderer> _renderer;
};
} // namespace a3d

#endif // AVARA3D_RENDER_CONTEXT_RENDERCONTEXT_H
