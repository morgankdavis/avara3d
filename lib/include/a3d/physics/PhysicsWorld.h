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

    class PhysicsWorld {

    public:
        /// Public Types ///

        struct RayTestOptions {
            HitTestSearchMode searchMode {HitTestSearchMode::Closest};
        };

        struct ConvexSweepTestOptions {
            HitTestSearchMode searchMode {HitTestSearchMode::Closest};
        };

        // TODO: Doxygen:
        // - Contact callbacks describe one logical contact lifecycle per pair of physics bodies.
        // - Continue callbacks occur once per simulation step while the pair remains in contact.
        // - Removing a physics body silently terminates its tracked contacts and does not generate
        //   an end-contact callback.
        // - Ordering between callbacks for unrelated body pairs is unspecified.

        using DidBeginContactCallback =
            std::function<void(PhysicsWorld& physicsWorld, const PhysicsContact& contact)>;
        using DidContinueContactCallback =
            std::function<void(PhysicsWorld& physicsWorld, const PhysicsContact& contact)>;
        using DidEndContactCallback =
            std::function<void(PhysicsWorld& physicsWorld, const PhysicsContact& contact)>;

        /// Public Lifecycle Functions ///

        PhysicsWorld();

        PhysicsWorld(const PhysicsWorld&)            = delete;
        PhysicsWorld& operator=(const PhysicsWorld&) = delete;

        PhysicsWorld(PhysicsWorld&&)            = delete;
        PhysicsWorld& operator=(PhysicsWorld&&) = delete;

        ~PhysicsWorld();

        /// Public Member Functions ///

        const math::vec3&             gravity() const;
        void                          gravity(const math::vec3& gravity);

        std::optional<PhysicsContact> contactTest(const PhysicsBody& bodyA, const PhysicsBody& bodyB) const;
        std::vector<PhysicsContact>   contactTest(const PhysicsBody& body) const;
        std::vector<HitTestResult>    rayTest(const math::vec3&     from,
                                              const math::vec3&     to,
                                              const RayTestOptions& options) const;
        std::vector<HitTestResult>    rayTest(const math::vec3& from, const math::vec3& to) const;
        // ! NOT IMPLEMENTED !
        std::vector<PhysicsContact>   convexSweepTest(const PhysicsShape&           shape,
                                                      const math::mat4&             fromMat,
                                                      const math::mat4&             toMat,
                                                      const ConvexSweepTestOptions& options) const;
        // ! NOT IMPLEMENTED !
        std::vector<PhysicsContact>   convexSweepTest(const PhysicsShape& shape,
                                                      const math::mat4&   fromMat,
                                                      const math::mat4&   toMat) const;

        void                          updateCollisionPairs();

        Scene*                        scene() const;

        DidBeginContactCallback       didBeginContactCallback() const;
        void                          didBeginContactCallback(DidBeginContactCallback function);

        DidContinueContactCallback    didContinueContactCallback() const;
        void                          didContinueContactCallback(DidContinueContactCallback function);

        DidEndContactCallback         didEndContactCallback() const;
        void                          didEndContactCallback(DidEndContactCallback function);

        /// Internal Types ///

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

        /// Internal Member Functions ///

        void               attachedToScene(Scene& scene);
        void               detachedFromScene(Scene& scene);

        void               add(PhysicsBody& body);
        void               remove(PhysicsBody& body);

        void               step(double deltaTime, Profiler& profiler);

        Inventory          inventory() const;

        void               appendDebugLines(std::vector<Line>& out) const;

        PhysicsWorldProxy* proxy() const;

    private:
        /// Private Member Variables ///

        math::vec3                         _gravity;
        std::unique_ptr<PhysicsWorldProxy> _proxy;
        Scene*                             _scene;
        DidBeginContactCallback            _didBeginContactCallback;
        DidContinueContactCallback         _didContinueContactCallback;
        DidEndContactCallback              _didEndContactCallback;
    };

}

#endif /* AVARA3D_PHYSICS_PHYSICSWORLD_H */
