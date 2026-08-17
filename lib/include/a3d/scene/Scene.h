//
//  Scene.h
//  avara3d
//
//  Created by Morgan Davis on 10/21/16.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_SCENE_SCENE_H
#define AVARA3D_SCENE_SCENE_H

#include <cstdint>
#include <filesystem>
#include <functional>
#include <memory>
#include <optional>
#include <string>

#include "a3d/Math.h"
#include "a3d/input/InputContext.h"
#include "a3d/util/Bitmask.h"

namespace a3d {

    struct AABB;

    class Color;
    class Mesh;
    class Node;
    class PhysicsWorld;
    class Profiler;
    class Renderer;
    class RenderContext;
    class VisualWorld;

    class Scene {

    public:
        /// Public Types ///

        enum class ImportOptions : uint16_t {
            None            = 0,
            ImportMeshes    = 1 << 0,
            ImportMaterials = 1 << 1,
            ImportLights    = 1 << 2,
            ImportCameras   = 1 << 3,
            ImportAll       = UINT16_MAX
        };

        // TODO: move to VisualWorld?
        enum class DebugOptions : uint32_t {
            None = 0,

            ShowStatsOverlay = 1 << 0,

            ShowMeshBounds     = 1 << 1,
            ShowMeshFrames     = 1 << 2,
            ShowMeshWireframes = 1 << 3,

            ShowCameras      = 1 << 4, // not implemented
            ShowLights       = 1 << 5, // not implemented
            ShowLightExtents = 1 << 6, // not implemented

            ShowPhysicsBounds           = 1 << 7,
            ShowPhysicsFrames           = 1 << 8,
            ShowPhysicsWireframes       = 1 << 9,
            ShowPhysicsContactPoints    = 1 << 10,
            ShowPhysicsNormals          = 1 << 11,
            ShowPhysicsConstraints      = 1 << 12, // not implemented
            ShowPhysicsConstraintLimits = 1 << 13, // not implemented
        };

        struct StepInfo {

            // zero-based simulation-step index
            std::uint64_t stepIndex {0};

            // simulation time before this step
            double        startTime {0.0};

            // simulation time after this step completes
            double        endTime {0.0};

            // amount of simulation time advanced by this step
            double        deltaTime {0.0};
        };

        using WillStepCallback = std::function<void(Scene& scene, const StepInfo& info)>;
        using DidStepCallback  = std::function<void(Scene& scene, const StepInfo& info)>;

        /// Public Static Member Functions ///

        static std::unique_ptr<Scene> FromFile(const std::filesystem::path& path,
                                               ImportOptions                options = ImportOptions::ImportAll);

        /// Public Lifecycle Functions ///

        Scene();
        explicit Scene(const std::string& name);
        Scene(std::unique_ptr<VisualWorld>  visualWorld,
              std::unique_ptr<PhysicsWorld> physicsWorld,
              std::unique_ptr<InputContext> inputContext);
        Scene(const std::string&            name,
              std::unique_ptr<VisualWorld>  visualWorld,
              std::unique_ptr<PhysicsWorld> physicsWorld,
              std::unique_ptr<InputContext> inputContext);

        Scene(const Scene&)            = delete;
        Scene& operator=(const Scene&) = delete;

        Scene(Scene&&)            = delete;
        Scene& operator=(Scene&&) = delete;

        ~Scene();

        /// Public Member Functions ///

        const std::optional<std::string>& name() const;
        void                              name(const std::string& name);

        const std::shared_ptr<Node>&      rootNode() const;
        void                              rootNode(const std::shared_ptr<Node>& node);

        VisualWorld*                      visualWorld() const;
        void                              visualWorld(std::unique_ptr<VisualWorld> world);

        PhysicsWorld*                     physicsWorld() const;
        void                              physicsWorld(std::unique_ptr<PhysicsWorld> world);

        InputContext*                     inputContext() const;
        void                              inputContext(std::unique_ptr<InputContext> context);

        AABB                              aabb(bool vertfit = false) const;
        math::vec3                        extent(bool vertfit = false) const;

        DebugOptions                      debugOptions() const;
        void                              debugOptions(DebugOptions options);

        WillStepCallback                  willStepCallback() const;
        void                              willStepCallback(WillStepCallback callback);

        DidStepCallback                   didStepCallback() const;
        void                              didStepCallback(DidStepCallback callback);

        /// Internal Member Functions ///

        void                              pollEvents(Profiler& profiler);
        void                              updateInput(const InputContext::UpdateInfo& info, Profiler& profiler);
        void                              stepSimulation(const StepInfo& info, Profiler& profiler);

    private:
        /// Private Member Variables ///

        std::optional<std::string>    _name;
        std::shared_ptr<Node>         _rootNode;
        std::unique_ptr<VisualWorld>  _visualWorld;
        std::unique_ptr<PhysicsWorld> _physicsWorld;
        std::unique_ptr<InputContext> _inputContext;
        DebugOptions                  _debugOptions;
        WillStepCallback              _willStepCallback;
        DidStepCallback               _didStepCallback;
    };

    namespace util::bitmask {

        template<>
        struct enable_ops<Scene::ImportOptions> : std::true_type {};

        template<>
        struct enable_ops<Scene::DebugOptions> : std::true_type {};

    }
}

#endif /* AVARA3D_SCENE_SCENE_H */
