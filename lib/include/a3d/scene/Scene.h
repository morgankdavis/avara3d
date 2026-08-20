//
//  Scene.h
//  avara3d
//
//  Created by Morgan Davis on 10/21/16.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
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

    /**
     * @brief Owns a scene graph and the worlds used to simulate and render it.
     *
     * A Scene always has a root Node. VisualWorld, PhysicsWorld, and InputContext
     * are optional and may be installed or replaced independently. Nodes attached
     * beneath the root become part of the Scene hierarchy and can discover the
     * Scene and its currently installed worlds.
     */
    class Scene {

    public:
        // [Public Types]

        /** @brief Selects which resource categories are imported by FromFile(). */
        enum class ImportOptions : uint16_t {
            None            = 0,          ///< Import no optional resource categories.
            ImportMeshes    = 1 << 0,     ///< Import meshes.
            ImportMaterials = 1 << 1,     ///< Import materials.
            ImportLights    = 1 << 2,     ///< Import lights.
            ImportCameras   = 1 << 3,     ///< Import cameras.
            ImportAll       = UINT16_MAX  ///< Import all supported resource categories.
        };

        // TODO: move to VisualWorld?
        /** @brief Scene-wide debug visualization options. */
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

        /** @brief Timing information for one simulation step. */
        struct StepInfo {

            /** Zero-based simulation-step index. */
            std::uint64_t stepIndex {0};

            /** Simulation time in seconds before this step. */
            double        startTime {0.0};

            /** Simulation time in seconds after this step completes. */
            double        endTime {0.0};

            /** Amount of simulation time in seconds advanced by this step. */
            double        deltaTime {0.0};
        };

        /** @brief Callback invoked immediately before a Scene simulation step. */
        using WillStepCallback = std::function<void(Scene& scene, const StepInfo& info)>;

        /** @brief Callback invoked immediately after a Scene simulation step. */
        using DidStepCallback  = std::function<void(Scene& scene, const StepInfo& info)>;

        // [Public Static Member Functions]

        /**
         * @brief Imports a Scene from @p path.
         *
         * @param path glTF scene file to import.
         * @param options resource categories to import.
         * @return The imported Scene.
         */
        static std::unique_ptr<Scene> FromFile(const std::filesystem::path& path,
                                               ImportOptions                options = ImportOptions::ImportAll);

        // [Public Lifecycle Functions]

        /** @brief Creates a Scene with an empty root node and no worlds or input context. */
        Scene();

        /** @brief Creates a named Scene with an empty root node and no worlds or input context. */
        explicit Scene(const std::string& name);

        /**
         * @brief Creates a Scene and takes ownership of its optional worlds and input context.
         *
         * Any argument may be nullptr.
         */
        Scene(std::unique_ptr<VisualWorld>  visualWorld,
              std::unique_ptr<PhysicsWorld> physicsWorld,
              std::unique_ptr<InputContext> inputContext);

        /**
         * @brief Creates a named Scene and takes ownership of its optional worlds and input context.
         *
         * Any world or input-context argument may be nullptr.
         */
        Scene(const std::string&            name,
              std::unique_ptr<VisualWorld>  visualWorld,
              std::unique_ptr<PhysicsWorld> physicsWorld,
              std::unique_ptr<InputContext> inputContext);

        Scene(const Scene&)            = delete;
        Scene& operator=(const Scene&) = delete;

        Scene(Scene&&)            = delete;
        Scene& operator=(Scene&&) = delete;

        ~Scene();

        // [Public Member Functions]

        /** @brief Returns the optional Scene name. */
        const std::optional<std::string>& name() const;

        /** @brief Sets the Scene name. */
        void                              name(const std::string& name);

        /** @brief Returns the root node of the Scene hierarchy. */
        const std::shared_ptr<Node>&      rootNode() const;

        /**
         * @brief Replaces the root node of the Scene hierarchy.
         *
         * The old root is detached from this Scene and @p node is attached in its place.
         *
         * @throws std::invalid_argument if @p node is nullptr.
         */
        void                              rootNode(const std::shared_ptr<Node>& node);

        /** @brief Returns the owned VisualWorld, or nullptr if none is installed. */
        VisualWorld*                      visualWorld() const;

        /** @brief Replaces the owned VisualWorld; nullptr removes the current world. */
        void                              visualWorld(std::unique_ptr<VisualWorld> world);

        /** @brief Returns the owned PhysicsWorld, or nullptr if none is installed. */
        PhysicsWorld*                     physicsWorld() const;

        /** @brief Replaces the owned PhysicsWorld; nullptr removes the current world. */
        void                              physicsWorld(std::unique_ptr<PhysicsWorld> world);

        /** @brief Returns the owned InputContext, or nullptr if none is installed. */
        InputContext*                     inputContext() const;

        /** @brief Replaces the owned InputContext; nullptr removes the current context. */
        void                              inputContext(std::unique_ptr<InputContext> context);

        /** @brief Returns the world-space axis-aligned bounding box enclosing the Scene hierarchy. */
        AABB                              aabb(bool vertfit = false) const;

        /** @brief Returns the dimensions of the Scene world-space axis-aligned bounding box. */
        math::vec3                        extent(bool vertfit = false) const;

        /** @brief Returns the enabled Scene debug visualization options. */
        DebugOptions                      debugOptions() const;

        /** @brief Sets the enabled Scene debug visualization options. */
        void                              debugOptions(DebugOptions options);

        /** @brief Returns the callback invoked before each simulation step. */
        WillStepCallback                  willStepCallback() const;

        /** @brief Replaces the callback invoked before each simulation step. */
        void                              willStepCallback(WillStepCallback callback);

        /** @brief Returns the callback invoked after each simulation step. */
        DidStepCallback                   didStepCallback() const;

        /** @brief Replaces the callback invoked after each simulation step. */
        void                              didStepCallback(DidStepCallback callback);

        // [Internal Member Functions]

        void                              pollEvents(Profiler& profiler);
        void                              updateInput(const InputContext::UpdateInfo& info, Profiler& profiler);
        void                              stepSimulation(const StepInfo& info, Profiler& profiler);

    private:
        // [Private Member Variables]

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
