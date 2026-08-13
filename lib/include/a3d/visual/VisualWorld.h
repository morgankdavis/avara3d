//
//  VisualWorld.h
//  avara3d
//
//  Created by Morgan Davis on 11/25/23.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
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
#include "a3d/visual/AtmosphericHaze.h"
#include "a3d/visual/Background.h"
#include "a3d/visual/Fog.h"
#include "a3d/visual/InfiniteGround.h"
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

    class VisualWorld {

    public:
        /// Public Types ///

        struct Capabilities {
            bool wireframeRendering {false};
        };

        struct RenderInfo {

            // zero-based successful-render-frame index
            std::uint64_t frameIndex {0};

            // Runner update responsible for this render attempt
            std::uint64_t updateIndex {0};

            // monotonic Runner elapsed time for the containing update
            double        updateTime {0.0};

            // monotonic delta for the containing Runner update
            double        updateDeltaTime {0.0};

            // time reached by the most recently completed simulation step
            double        simulationTime {0.0};

            // total number of completed simulation steps
            std::uint64_t simulationStepCount {0};
        };

        struct HitTestOptions {
            HitTestSearchMode searchMode {HitTestSearchMode::Closest};
        };

        /// Public Lifecycle Functions ///

        VisualWorld() = delete;
        explicit VisualWorld(RenderContext& context);

        VisualWorld(const VisualWorld&)            = delete;
        VisualWorld& operator=(const VisualWorld&) = delete;

        VisualWorld(VisualWorld&&)            = delete;
        VisualWorld& operator=(VisualWorld&&) = delete;

        virtual ~VisualWorld();

        /// Public Member Functions ///

        Capabilities                   capabilities() const;

        std::optional<Background>&     background();
        void                           background(const std::optional<Background>& background);

        const std::optional<AtmosphericHaze>& atmosphericHaze() const;
        void                                  atmosphericHaze(const std::optional<AtmosphericHaze>& haze);

        const std::optional<Fog>&      fog() const;
        void                           fog(const std::optional<Fog>& fog);

        std::optional<InfiniteGround>& infiniteGround();
        void                           infiniteGround(const std::optional<InfiniteGround>& ground);

        std::weak_ptr<Node>&           pointOfView();
        void                           pointOfView(const std::weak_ptr<Node>& cameraNode);

        math::vec3                     projectPoint(const math::vec3& point) const;
        math::vec3                     unprojectPoint(const math::vec3& point) const;

        std::vector<HitTestResult>     hitTest(const math::vec2& point, const HitTestOptions& options) const;
        std::vector<HitTestResult>     hitTest(const math::vec2& point) const;

        bool                           defaultLightingEnabled() const;
        void                           defaultLightingEnabled(bool enabled);

        RenderContext*                 renderContext() const;

        Scene*                         scene() const;

        /// Internal Types ///

        using DidBeginFrameCallback = std::function<void(VisualWorld& visualWorld, const RenderInfo& info)>;

        /// Internal Member Functions ///

        void                      attachedToScene(Scene& scene);
        void                      detachedFromScene(Scene& scene);

        DidBeginFrameCallback     didBeginFrameCallback() const;
        void                      didBeginFrameCallback(DidBeginFrameCallback function);

        bool                      draw(const Scene&             scene,
                                       const PhysicsWorld*      physicsWorld,
                                       const RenderInfo&        info,
                                       Scene::DebugOptions      debugOptions,
                                       FrameStats&              stats,
                                       Profiler&                profiler,
                                       const FrameStatsHistory& statsHistory);

        std::shared_ptr<Material> backgroundMaterial();

    private:
        /// Private Member Functions ///

        void                          firstDraw();
        std::shared_ptr<Node>         defaultPOV();

        /// Private Member Variables ///

        std::optional<Background>     _background;
        std::shared_ptr<Material>     _backgroundMaterial;
        std::optional<Fog>            _fog;
        std::optional<InfiniteGround> _infiniteGround;
        bool                          _defaultLightingEnabled;
        std::weak_ptr<Node>           _pointOfView;
        RenderContext*                _renderContext;
        Scene*                        _scene;
        DidBeginFrameCallback         _didBeginFrameCallback;
    };

}

#endif //AVARA3D_VISUAL_VISUALWORLD_H
