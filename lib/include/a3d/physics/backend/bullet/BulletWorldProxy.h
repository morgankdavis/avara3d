//
//  BulletWorldProxy.h
//  avara3d
//
//  Created by Morgan Davis on 12/8/23.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_PHYSICS_BACKEND_BULLET_BULLETWORLDPROXY_H
#define AVARA3D_PHYSICS_BACKEND_BULLET_BULLETWORLDPROXY_H

#include <chrono>
#include <memory>
#include <mutex>
#include <vector>

#include "a3d/mesh/Line.h"
#include "a3d/physics/backend/bullet/BulletStats.h"
#include "a3d/physics/proxy/PhysicsWorldProxy.h"
#include "a3d/scene/Scene.h"

struct btDbvtBroadphase;

class btCollisionDispatcher;
class btConstraintSolver;
class btConstraintSolverPoolMt;
class btDiscreteDynamicsWorld;
class btITaskScheduler;
class btSequentialImpulseConstraintSolver;
class btSequentialImpulseConstraintSolverMt;
class btDefaultCollisionConfiguration;

namespace a3d {

    class BulletDebugDrawer;
    class RenderContext;

    class BulletWorldProxy : public PhysicsWorldProxy {

    public:
        /// Internal Lifecycle Functions ///

        explicit BulletWorldProxy(PhysicsWorld& world);
        ~BulletWorldProxy() override;

        /// PhysicsWorldModelProxy Internal Member Functions ///

        void                       add(PhysicsBody& body) override;
        void                       remove(PhysicsBody& body) override;

        math::vec3                 gravity() const override;
        void                       gravity(const math::vec3& gravity) override;

        const ContactEvents&       step(double deltaTime, Profiler& profiler) override;

        std::vector<HitTestResult> rayTest(const math::vec3& from,
                                           const math::vec3& to,
                                           HitTestSearchMode searchMode) const override;

        PhysicsWorld::Inventory    inventory() const override;

        void                       updateCollisionPairs() override;

        void appendDebugLines(std::vector<Line>& out, Scene::DebugOptions debugOptions) override;

        /// Internal Member Functions ///

        btDiscreteDynamicsWorld* btWorld();

    private:
        /// Private Types ///

        struct BodyPairContact {
            PhysicsBody*   bodyA;
            PhysicsBody*   bodyB;
            PhysicsContact contact;
        };

        using BodyPairContacts = std::vector<BodyPairContact>;

        /// Private Member Functions ///

        void                                                   extractCurrentContacts();
        void                                                   buildContactEvents();
        void                                                   removeTrackedContacts(PhysicsBody& body);

        ///  Private Member Variables ///

        // scheduler
        btITaskScheduler*                                      _btScheduler;
        std::unique_ptr<btITaskScheduler>                      _ownedScheduler;
        btITaskScheduler*                                      _prevScheduler; // non-owning

        // config/dispatcher/broadphase
        std::unique_ptr<btDefaultCollisionConfiguration>       _btCollisionConfiguration;
        std::unique_ptr<btCollisionDispatcher>                 _btCollisionDispatcher;
        std::unique_ptr<btDbvtBroadphase>                      _btBroadphase;

        // solvers
        std::unique_ptr<btConstraintSolverPoolMt>              _btSolverPool;
        std::unique_ptr<btSequentialImpulseConstraintSolverMt> _btSolverMt;

        std::unique_ptr<BulletDebugDrawer>                     _btDebugDrawer;
        std::vector<Line>                                      _debugLines;

        BulletStats                                            _stats;

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
