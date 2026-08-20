//
//  BulletWorldProxy.h
//  avara3d
//
//  Created by Morgan Davis on 12/8/23.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_PHYSICS_BACKEND_BULLET_BULLETWORLDPROXY_H
#define AVARA3D_PHYSICS_BACKEND_BULLET_BULLETWORLDPROXY_H

#include <chrono>
#include <memory>
#include <mutex>
#include <unordered_map>
#include <vector>

#include "a3d/mesh/Line.h"
#include "a3d/physics/PhysicsWorld.h"
#include "a3d/physics/proxy/PhysicsWorldProxy.h"
#include "a3d/scene/Scene.h"

class btCollisionDispatcher;
class btCollisionObject;
class btConstraintSolver;
class btConstraintSolverPoolMt;
class btDiscreteDynamicsWorld;
class btITaskScheduler;
class btManifoldPoint;
class btSequentialImpulseConstraintSolver;
class btSequentialImpulseConstraintSolverMt;
class btDefaultCollisionConfiguration;

struct btDbvtBroadphase;

namespace a3d {

    class BulletDebugDrawer;
    class PhysicsShape;
    class RenderContext;

    class BulletWorldProxy : public PhysicsWorldProxy {

    public:
        // [Internal Lifecycle Functions]

        explicit BulletWorldProxy(PhysicsWorld& world);
        ~BulletWorldProxy() override;

        // [PhysicsWorldModelProxy Internal Member Functions]

        void                          add(PhysicsBody& body) override;
        void                          remove(PhysicsBody& body) override;

        math::vec3                    gravity() const override;
        void                          gravity(const math::vec3& gravity) override;

        const ContactEvents&          step(double deltaTime, Profiler& profiler) override;

        std::optional<PhysicsContact> contactTest(const PhysicsBody& bodyA,
                                                  const PhysicsBody& bodyB) const override;
        std::vector<PhysicsContact>   contactTest(const PhysicsBody& body) const override;
        std::vector<HitTestResult>    rayTest(const math::vec3& from,
                                              const math::vec3& to,
                                              HitTestSearchMode searchMode) const override;
        // ! NOT IMPLEMENTED !
        std::vector<PhysicsContact>   convexSweepTest(const PhysicsShape& shape,
                                                      const math::mat4&   fromMat,
                                                      const math::mat4&   toMat,
                                                      HitTestSearchMode   searchMode) const override;

        PhysicsWorld::Inventory       inventory() const override;

        void                          updateCollisionPairs() override;

        void appendDebugLines(std::vector<Line>& out, Scene::DebugOptions debugOptions) override;

        // [Internal Member Functions]

        btDiscreteDynamicsWorld* btWorld();

    private:
        // [Private Types]

        struct InventoryState {
            unsigned                                          staticBodies {0};
            unsigned                                          dynamicBodies {0};
            unsigned                                          kinematicBodies {0};
            std::unordered_map<const PhysicsShape*, unsigned> primitiveShapeRefs;
            std::unordered_map<const PhysicsShape*, unsigned> boundingBoxShapeRefs;
            std::unordered_map<const PhysicsShape*, unsigned> convexHullShapeRefs;
            std::unordered_map<const PhysicsShape*, unsigned> concavePolyhedronShapeRefs;
        };

        struct BodyPairContact {
            PhysicsBody*   bodyA;
            PhysicsBody*   bodyB;
            PhysicsContact contact;
        };

        using BodyPairContacts = std::vector<BodyPairContact>;

        // [Private Member Functions]

        void                              extractCurrentContacts();
        void                              buildContactEvents();
        void                              removeTrackedContacts(PhysicsBody& body);
        std::optional<BodyPairContact>    makeBodyPairContact(const btCollisionObject* objectA,
                                                              const btCollisionObject* objectB,
                                                              const btManifoldPoint&   point) const;

        // [Private Member Variables]

        // scheduler
        btITaskScheduler*                 _btScheduler;
        std::unique_ptr<btITaskScheduler> _ownedScheduler;
        btITaskScheduler*                 _prevScheduler; // non-owning

        // config/dispatcher/broadphase
        std::unique_ptr<btDefaultCollisionConfiguration>       _btCollisionConfiguration;
        std::unique_ptr<btCollisionDispatcher>                 _btCollisionDispatcher;
        std::unique_ptr<btDbvtBroadphase>                      _btBroadphase;

        // solvers
        std::unique_ptr<btConstraintSolverPoolMt>              _btSolverPool;
        std::unique_ptr<btSequentialImpulseConstraintSolverMt> _btSolverMt;

        std::unique_ptr<BulletDebugDrawer>                     _btDebugDrawer;
        std::vector<Line>                                      _debugLines;

        InventoryState                                         _inventoryState;

        ContactEvents                                          _contactEvents;
        BodyPairContacts                                       _currentContacts;
        BodyPairContacts                                       _activeContacts;

        mutable std::mutex                                     _btMutex;

        // MUST be last so destroyed first
        std::unique_ptr<btDiscreteDynamicsWorld>               _btWorld;

        std::chrono::steady_clock::time_point                  _nextDebugLineUpdate;
        int                                                    _debugDrawMode;
    };

}

#endif //AVARA3D_PHYSICS_BACKEND_BULLET_BULLETWORLDPROXY_H
