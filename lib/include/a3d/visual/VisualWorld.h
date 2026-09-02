//
//  VisualWorld.h
//  avara3d
//
//  Created by Morgan Davis on 11/25/23.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_VISUAL_VISUALWORLD_H
#define AVARA3D_VISUAL_VISUALWORLD_H

#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <vector>

#include "a3d/scene/Scene.h"
#include "a3d/scene/HitTestResult.h"
#include "a3d/visual/Atmosphere.h"
#include "a3d/visual/Background.h"
#include "a3d/visual/Fog.h"
#include "a3d/visual/Ground.h"
#include "a3d/visual/Surface.h"
#include "a3d/visual/material/Material.h"

namespace a3d {

struct FrameStats;

class Color;
class FrameStatsHistory;
class Material;
class Mesh;
class Node;
class PhysicsWorld;
class Profiler;
class Renderer;
class RenderContext;
class Scene;

/**
 * @brief Manages the visual presentation, camera, and visual queries for a Scene.
 *
 * A VisualWorld renders through the RenderContext supplied at construction and
 * stores a non-owning reference to that context. The RenderContext must outlive
 * the VisualWorld.
 *
 * If no point of view is set before the first draw, VisualWorld uses the first
 * camera node it finds in the Scene. If the Scene contains no camera, a default
 * perspective camera node is created and added to the Scene root.
 */
class VisualWorld {

public:
    // [Public Types]

    /** @brief Rendering capabilities available through this VisualWorld. */
    struct Capabilities {
        bool wireframeRendering {false}; ///< Whether wireframe rendering is supported.
    };

    /** @brief Timing and progression information associated with a rendered frame. */
    struct RenderInfo {

        // zero-based successful-render-frame index
        std::uint64_t frameIndex {0}; ///< Zero-based index of the successful rendered frame.

        // Runner update responsible for this render attempt
        std::uint64_t updateIndex {0}; ///< Runner update responsible for this render attempt.

        // monotonic Runner elapsed time for the containing update
        double        updateTime {0.0}; ///< Runner elapsed time for the containing update, in seconds.

        // monotonic delta for the containing Runner update
        double        updateDeltaTime {0.0}; ///< Delta time of the containing Runner update, in seconds.

        // time reached by the most recently completed simulation step
        double        simulationTime {
            0.0}; ///< Time reached by the most recently completed simulation step, in seconds.

        // total number of completed simulation steps
        std::uint64_t simulationStepCount {0}; ///< Total number of completed simulation steps.
    };

    /** @brief Options controlling visual hit testing. */
    struct HitTestOptions {
        HitTestSearchMode searchMode {HitTestSearchMode::Closest}; ///< Which matching hits to return.
        bool elementBoundsOnly {false}; ///< Test MeshElement bounds instead of individual triangles.
        std::vector<const Node*> ignoredNodes {}; ///< Nodes excluded from hit testing.
    };

    /** @brief Callback invoked after renderer frame setup and before Scene rendering begins. */
    using DidBeginFrameCallback = std::function<void(VisualWorld& visualWorld, const RenderInfo& info)>;

    // [Public Lifecycle Functions]

    VisualWorld() = delete;

    /**
     * @brief Creates a VisualWorld that renders through @p context.
     *
     * @p context is retained non-owningly and must outlive this VisualWorld.
     */
    explicit VisualWorld(RenderContext& context);

    VisualWorld(const VisualWorld&)            = delete;
    VisualWorld& operator=(const VisualWorld&) = delete;

    VisualWorld(VisualWorld&&)            = delete;
    VisualWorld& operator=(VisualWorld&&) = delete;

    virtual ~VisualWorld();

    // [Public Member Functions]

    /** @brief Returns the rendering capabilities currently available to the VisualWorld. */
    Capabilities                       capabilities() const;

    /** @brief Returns the optional Scene background. */
    const std::optional<Background>&   background() const;

    /**
     * @brief Sets or disables the Scene background.
     *
     * Background contents must be a Color or a Texture containing a CubeImage.
     * A cubemap Texture must have a non-null Sampler. Its wrapping is set to
     * ClampToEdge on all three axes.
     *
     * @throws std::invalid_argument if the supplied background contents are invalid.
     */
    void                               background(const std::optional<Background>& background);

    /** @brief Returns the optional distance fog configuration. */
    const std::optional<Fog>&          fog() const;

    /**
     * @brief Sets or disables distance fog.
     *
     * @throws std::invalid_argument if distances or transition exponent are invalid.
     */
    void                               fog(const std::optional<Fog>& fog);

    /** @brief Returns the optional atmosphere configuration. */
    const std::optional<Atmosphere>&   atmosphere() const;

    /**
     * @brief Sets or disables atmospheric effects.
     *
     * An enabled Atmosphere requires a Surface; limb glow additionally requires
     * a SphereSurface.
     *
     * @throws std::logic_error if the required Surface configuration is absent.
     * @throws std::invalid_argument if an atmospheric numeric parameter is invalid.
     */
    void                               atmosphere(const std::optional<Atmosphere>& atmosphere);

    /** @brief Returns the optional visual ground configuration. */
    const std::optional<Ground>&       ground() const;

    /**
     * @brief Sets or disables visual ground rendering.
     *
     * An enabled Ground requires a Surface.
     *
     * @throws std::logic_error if no Surface is configured.
     * @throws std::invalid_argument if a ground, fade, or haze parameter is invalid.
     */
    void                               ground(const std::optional<Ground>& ground);

    /** @brief Returns the optional reference Surface used by ground and atmospheric effects. */
    const std::optional<a3d::Surface>& surface() const;

    /**
     * @brief Sets or removes the reference Surface used by ground and atmospheric effects.
     *
     * The Surface cannot be removed while Ground or Atmosphere is enabled, and
     * an Atmosphere with limb glow requires a SphereSurface.
     *
     * @throws std::logic_error if the requested Surface conflicts with enabled effects.
     * @throws std::invalid_argument if the Surface contains invalid geometry.
     */
    void                               surface(const std::optional<a3d::Surface>& surface);

    /** @brief Returns the weak reference to the camera Node used as the point of view. */
    std::weak_ptr<Node>&               pointOfView();

    /**
     * @brief Sets the camera Node used as the point of view.
     *
     * Passing an empty weak pointer clears the explicit point of view. If no point
     * of view is set before the first draw, first-draw initialization selects or
     * creates a Scene camera automatically.
     */
    void                               pointOfView(const std::weak_ptr<Node>& cameraNode);

    /**
     * @brief Projects a world-space point into logical viewport coordinates.
     *
     * The returned X and Y use a top-left viewport origin. Z is normalized so
     * 0 corresponds to the near clip plane and 1 to the far clip plane.
     *
     * @throws std::runtime_error if no valid point of view or viewport is available.
     */
    math::vec3                         projectPoint(const math::vec3& point) const;

    /**
     * @brief Unprojects a logical viewport point into world coordinates.
     *
     * Input X and Y use a top-left viewport origin. Input Z uses normalized
     * depth, with 0 at the near clip plane and 1 at the far clip plane.
     *
     * @throws std::runtime_error if no valid point of view or viewport is available.
     */
    math::vec3                         unprojectPoint(const math::vec3& point) const;

    /**
     * @brief Hit-tests visible Scene mesh geometry beneath a logical viewport point.
     *
     * With elementBoundsOnly enabled, MeshElement bounds are tested instead of
     * triangles and returned hits do not contain a face index. Nodes listed in
     * HitTestOptions::ignoredNodes are excluded from testing; their descendants
     * remain eligible for hits.
     *
     * @throws std::runtime_error if the Scene, point of view, or viewport cannot
     * produce a valid hit-test ray.
     */
    std::vector<HitTestResult>         hitTest(const math::vec2& point, const HitTestOptions& options) const;

    /** @brief Hit-tests the closest visible Scene mesh geometry beneath a logical viewport point. */
    std::vector<HitTestResult>         hitTest(const math::vec2& point) const;

    /** @brief Returns whether renderer-provided default lighting is enabled. */
    bool                               defaultLightingEnabled() const;

    /**
     * @brief Enables or disables renderer-provided default lighting.
     *
     * When enabled, material base content is rendered without evaluating the
     * Scene's ambient, directional, point, or spot lights.
     */
    void                               defaultLightingEnabled(bool enabled);

    /** @brief Returns the callback invoked immediately before Scene rendering begins. */
    DidBeginFrameCallback              didBeginFrameCallback() const;

    /** @brief Sets the begin-frame callback; an empty callback disables it. */
    void                               didBeginFrameCallback(DidBeginFrameCallback function);

    /** @brief Returns the non-owning RenderContext associated with this VisualWorld. */
    RenderContext*                     renderContext() const;

    /** @brief Returns the Scene this VisualWorld is attached to, or nullptr if unattached. */
    Scene*                             scene() const;

    // [Internal Member Functions]

    void                               attachedToScene(Scene& scene);
    void                               detachedFromScene(Scene& scene);

    bool                               draw(const Scene&             scene,
                                            const PhysicsWorld*      physicsWorld,
                                            const RenderInfo&        info,
                                            Scene::DebugOptions      debugOptions,
                                            FrameStats&              stats,
                                            Profiler&                profiler,
                                            const FrameStatsHistory& statsHistory);

    std::shared_ptr<Material>          backgroundMaterial();

private:
    // [Private Member Functions]

    void                        firstDraw();
    std::shared_ptr<Node>       defaultPOV();

    // [Private Member Variables]

    std::optional<Background>   _background;
    std::shared_ptr<Material>   _backgroundMaterial;
    std::optional<Fog>          _fog;
    std::optional<Atmosphere>   _atmosphere;
    std::optional<Ground>       _ground;
    std::optional<a3d::Surface> _surface;
    bool                        _defaultLightingEnabled;
    std::weak_ptr<Node>         _pointOfView;
    RenderContext*              _renderContext;
    Scene*                      _scene;
    DidBeginFrameCallback       _didBeginFrameCallback;
};

}

#endif // AVARA3D_VISUAL_VISUALWORLD_H
