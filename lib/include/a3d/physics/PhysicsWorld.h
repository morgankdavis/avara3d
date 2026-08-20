//
//  PhysicsWorld.h
//  avara3d
//
//  Created by Morgan Davis on 1/26/18.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_PHYSICS_PHYSICSWORLD_H
#define AVARA3D_PHYSICS_PHYSICSWORLD_H

#include <functional>
#include <memory>
#include <optional>
#include <vector>

#include "a3d/Math.h"
#include "a3d/scene/HitTestResult.h"

namespace a3d {

    class HitTestResult;
    class Line;
    class PhysicsBody;
    class PhysicsContact;
    class PhysicsShape;
    class PhysicsWorldProxy;
    class Profiler;
    class Scene;

    /**
     * @brief Simulates rigid bodies and performs physics collision queries for a Scene.
     *
     * Physics bodies attached to nodes in a Scene become members of its PhysicsWorld
     * when they have a usable collision shape. The world applies its gravity to
     * participating dynamic bodies and reports logical contact lifecycles through
     * optional callbacks.
     */
    class PhysicsWorld {

    public:
        // [Public Types]

        /** @brief Options controlling a ray collision query. */
        struct RayTestOptions {
            /** How matching ray hits are selected. */
            HitTestSearchMode searchMode {HitTestSearchMode::Closest};
        };

        /** @brief Options controlling a convex sweep collision query. */
        struct ConvexSweepTestOptions {
            /** How matching sweep hits are selected. */
            HitTestSearchMode searchMode {HitTestSearchMode::Closest};
        };

        /**
         * @brief Callback invoked when a pair of physics bodies begins a logical contact.
         *
         * A body pair produces one logical contact lifecycle even when multiple
         * underlying contact points exist.
         */
        using DidBeginContactCallback =
            std::function<void(PhysicsWorld& physicsWorld, const PhysicsContact& contact)>;

        /**
         * @brief Callback invoked once per simulation step while a body pair remains in contact.
         *
         * Ordering relative to callbacks for unrelated body pairs is unspecified.
         */
        using DidContinueContactCallback =
            std::function<void(PhysicsWorld& physicsWorld, const PhysicsContact& contact)>;

        /**
         * @brief Callback invoked when a tracked body pair ceases to be in contact.
         *
         * Removing a physics body silently terminates its tracked contacts and does
         * not generate end-contact callbacks. Ordering relative to callbacks for
         * unrelated body pairs is unspecified.
         */
        using DidEndContactCallback =
            std::function<void(PhysicsWorld& physicsWorld, const PhysicsContact& contact)>;

        // [Public Lifecycle Functions]

        /** @brief Creates a PhysicsWorld with Earth-like gravity of (0, -9.807, 0). */
        PhysicsWorld();

        PhysicsWorld(const PhysicsWorld&)            = delete;
        PhysicsWorld& operator=(const PhysicsWorld&) = delete;

        PhysicsWorld(PhysicsWorld&&)            = delete;
        PhysicsWorld& operator=(PhysicsWorld&&) = delete;

        ~PhysicsWorld();

        // [Public Member Functions]

        /** @brief Returns the world gravity acceleration vector. */
        const math::vec3&             gravity() const;

        /** @brief Sets the world gravity and immediately applies it to affected dynamic bodies. */
        void                          gravity(const math::vec3& gravity);

        /**
         * @brief Tests for contact between @p bodyA and @p bodyB.
         *
         * If the bodies have multiple contact points, the returned PhysicsContact
         * represents the point with greatest collision impulse, using penetration
         * depth as a tie-breaker.
         *
         * @return The representative contact, or an empty optional if the bodies are not in contact.
         */
        std::optional<PhysicsContact> contactTest(const PhysicsBody& bodyA, const PhysicsBody& bodyB) const;

        /**
         * @brief Returns current contacts between @p body and other physics bodies.
         *
         * At most one representative PhysicsContact is returned for each body pair.
         */
        std::vector<PhysicsContact>   contactTest(const PhysicsBody& body) const;

        /**
         * @brief Tests the world-space segment from @p from to @p to against physics bodies.
         *
         * @param options controls whether any, the closest, or all hits are returned.
         * @return Physics hit results selected according to @p options.
         */
        std::vector<HitTestResult>    rayTest(const math::vec3&     from,
                                              const math::vec3&     to,
                                              const RayTestOptions& options) const;

        /**
         * @brief Tests the world-space segment from @p from to @p to and returns the closest physics hit.
         *
         * @return Zero or one HitTestResult.
         */
        std::vector<HitTestResult>    rayTest(const math::vec3& from, const math::vec3& to) const;

        // ! NOT IMPLEMENTED !
        /**
         * @brief Sweeps @p shape between two transforms. Not currently implemented.
         *
         * @throws std::runtime_error always; convex sweep tests are not currently implemented.
         */
        std::vector<PhysicsContact>   convexSweepTest(const PhysicsShape&           shape,
                                                      const math::mat4&             fromMat,
                                                      const math::mat4&             toMat,
                                                      const ConvexSweepTestOptions& options) const;

        // ! NOT IMPLEMENTED !
        /**
         * @brief Sweeps @p shape between two transforms. Not currently implemented.
         *
         * @throws std::runtime_error always; convex sweep tests are not currently implemented.
         */
        std::vector<PhysicsContact>   convexSweepTest(const PhysicsShape& shape,
                                                      const math::mat4&   fromMat,
                                                      const math::mat4&   toMat) const;

        /** @brief Forces collision-overlap pairs to be recomputed from the current physics state. */
        void                          updateCollisionPairs();

        /** @brief Returns the Scene containing this PhysicsWorld, or nullptr when it is not installed in a Scene. */
        Scene*                        scene() const;

        /** @brief Returns the callback invoked when a body pair begins contact. */
        DidBeginContactCallback       didBeginContactCallback() const;

        /** @brief Replaces the begin-contact callback; an empty callback disables it. */
        void                          didBeginContactCallback(DidBeginContactCallback function);

        /** @brief Returns the callback invoked while a body pair remains in contact. */
        DidContinueContactCallback    didContinueContactCallback() const;

        /** @brief Replaces the continuing-contact callback; an empty callback disables it. */
        void                          didContinueContactCallback(DidContinueContactCallback function);

        /** @brief Returns the callback invoked when a tracked body pair ends contact. */
        DidEndContactCallback         didEndContactCallback() const;

        /** @brief Replaces the end-contact callback; an empty callback disables it. */
        void                          didEndContactCallback(DidEndContactCallback function);

        // [Internal Types]

        struct Inventory {
            unsigned staticBodies {0};
            unsigned dynamicBodies {0};
            unsigned kinematicBodies {0};
            unsigned primitiveShapes {0};
            unsigned boundingBoxShapes {0};
            unsigned convexHullShapes {0};
            unsigned concavePolyhedronShapes {0};
            unsigned activeContacts {0};
        };

        // [Internal Member Functions]

        void               attachedToScene(Scene& scene);
        void               detachedFromScene(Scene& scene);

        void               add(PhysicsBody& body);
        void               remove(PhysicsBody& body);

        void               step(double deltaTime, Profiler& profiler);

        Inventory          inventory() const;

        void               appendDebugLines(std::vector<Line>& out) const;

        PhysicsWorldProxy* proxy() const;

    private:
        // [Private Member Variables]

        math::vec3                         _gravity;
        std::unique_ptr<PhysicsWorldProxy> _proxy;
        Scene*                             _scene;
        DidBeginContactCallback            _didBeginContactCallback;
        DidContinueContactCallback         _didContinueContactCallback;
        DidEndContactCallback              _didEndContactCallback;
    };

}

#endif /* AVARA3D_PHYSICS_PHYSICSWORLD_H */
