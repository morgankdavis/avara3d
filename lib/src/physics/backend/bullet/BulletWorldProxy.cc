//
//  BulletWorldProxy.cc
//  avara3d
//
//  Created by Morgan Davis on 12/8/23.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#include "a3d/physics/backend/bullet/BulletWorldProxy.h"

#include <algorithm>
#include <cmath>
#include <functional>
#include <limits>
#include <stdexcept>
#include <thread>
#include <unordered_set>

#include <bullet/btBulletCollisionCommon.h>
#include <bullet/btBulletDynamicsCommon.h>
#include <bullet/BulletCollision/CollisionDispatch/btCollisionDispatcherMt.h>
#include <bullet/BulletCollision/Gimpact/btGImpactCollisionAlgorithm.h>
#include <bullet/BulletCollision/Gimpact/btGImpactShape.h>
#include <bullet/BulletDynamics/ConstraintSolver/btSequentialImpulseConstraintSolverMt.h>
#include <bullet/BulletDynamics/Dynamics/btDiscreteDynamicsWorldMt.h>
#include <bullet/LinearMath/btIDebugDraw.h>
#include <bullet/LinearMath/btThreads.h>
#include <magic_enum/magic_enum.hpp>

#include "a3d/log/Log.h"
#include "a3d/physics/PhysicsBody.h"
#include "a3d/physics/PhysicsWorld.h"
#include "a3d/physics/shape/PhysicsShape.h"
#include "a3d/physics/backend/bullet/BulletBodyProxy.h"
#include "a3d/physics/backend/bullet/BulletDebugDrawer.h"
#include "a3d/physics/backend/bullet/BulletUtilities.h"
#include "a3d/profile/Profile.h"
#include "a3d/scene/Node.h"
#include "a3d/util/Bitmask.h"
#include "a3d/util/Flow.h"

using namespace a3d;
using namespace a3d::math;
using namespace std;

// ! btCollisionDispatcherMt is known to be buggy. leave it off.
static constexpr bool A3D_USE_MT_DISPATCHER = false;

// bullet's MT spatial-grid contact batching can assert on large/dense
// contact islands. keep the MT world/solver infrastructure, but disable
// contact batching until/unless we patch or replace that path
static constexpr bool A3D_USE_MT_CONTACT_BATCHING = false;

// make sure bullet is built with MT enabled
//#if !defined(BT_THREADSAFE) || (BT_THREADSAFE != 1)
//#   error "Bullet requires building with BT_THREADSAFE=1"
//#endif

// native builds use bullet's multithreaded world/solver path, so require
// bullet to be compiled with thread-safe support. normal browser/wasm builds
// currently force bullet's sequential scheduler instead; they should not require
// bullet worker-thread support unless/until we ship a separate pthread wasm build
#if !(defined(__EMSCRIPTEN__) && !defined(__EMSCRIPTEN_PTHREADS__))
    #if !defined(BT_THREADSAFE) || (BT_THREADSAFE != 1)
        #error "Native threaded Bullet requires building with BT_THREADSAFE=1"
    #endif
#endif

/// Private Static Non-Member Prototypes ///

static btIDebugDraw::DebugDrawModes BTDebugDrawModesForA3DDebugOptions(const Scene::DebugOptions& options);
static int                          PickNumBTThreads(btITaskScheduler* sched);

/// Internal Lifecycle Functions ///

BulletWorldProxy::BulletWorldProxy(PhysicsWorld& world):
    PhysicsWorldProxy {world},
    _stats {},
    _debugDrawMode {0} {

    log::i()("Bullet Physics version: {}", btGetVersion());

//	_btScheduler = btGetOpenMPTaskScheduler();
//	if (!_btScheduler) _btScheduler = btGetTBBTaskScheduler();
//	if (!_btScheduler) _btScheduler = btGetPPLTaskScheduler();
//	if (!_btScheduler) {
//		_ownedScheduler.reset(btCreateDefaultTaskScheduler());
//		_btScheduler = _ownedScheduler.get();
//	}
//
//	_prevScheduler = btGetTaskScheduler();
//	btSetTaskScheduler(_btScheduler);
//
//	const int numThreads = PickNumBTThreads(_btScheduler);
//	_btScheduler->setNumThreads(numThreads);

    _prevScheduler = btGetTaskScheduler();

#if defined(__EMSCRIPTEN__) && !defined(__EMSCRIPTEN_PTHREADS__)

    // Normal browser/WASM build:
    // no pthreads, no Bullet worker pool.
    //
    // Keep Bullet behind the same C++ backend, but force Bullet's
    // non-threaded scheduler so btParallelFor work executes serially.
    _btScheduler = btGetSequentialTaskScheduler();
    btSetTaskScheduler(_btScheduler);

#else

    _btScheduler = btGetOpenMPTaskScheduler();
    if (!_btScheduler) {
        _btScheduler = btGetTBBTaskScheduler();
    }
    if (!_btScheduler) {
        _btScheduler = btGetPPLTaskScheduler();
    }
    if (!_btScheduler) {
        _ownedScheduler.reset(btCreateDefaultTaskScheduler());
        _btScheduler = _ownedScheduler.get();
    }

    if (!_btScheduler) {
        _btScheduler = btGetSequentialTaskScheduler();
    }

    btSetTaskScheduler(_btScheduler);

    if (_btScheduler != btGetSequentialTaskScheduler()) {
        const int numThreads = PickNumBTThreads(_btScheduler);
        _btScheduler->setNumThreads(numThreads);
    }

#endif

    log::i()("Bullet task scheduler: {} (threads: {}/{})", _btScheduler->getName(),
             _btScheduler->getNumThreads(), _btScheduler->getMaxNumThreads());

    _btCollisionConfiguration = std::make_unique<btDefaultCollisionConfiguration>();

    if constexpr (A3D_USE_MT_DISPATCHER) {
        _btCollisionDispatcher = make_unique<btCollisionDispatcherMt>(_btCollisionConfiguration.get());
        log::i()("Bullet dispatcher: MT");
    }
    else {
        _btCollisionDispatcher = make_unique<btCollisionDispatcher>(_btCollisionConfiguration.get());
        log::i()("Bullet dispatcher: ST");
    }

    btGImpactCollisionAlgorithm::registerAlgorithm(_btCollisionDispatcher.get());

    _btBroadphase = std::make_unique<btDbvtBroadphase>();

    // ! important: use the pool-size ctor so the pool owns its internal solvers
    const int poolSize = std::max(1, _btScheduler->getNumThreads() * 2);
    _btSolverPool = std::make_unique<btConstraintSolverPoolMt>(poolSize);

    if constexpr (!A3D_USE_MT_CONTACT_BATCHING) {
        btSequentialImpulseConstraintSolverMt::s_minimumContactManifoldsForBatching =
            std::numeric_limits<int>::max();
    }

    _btSolverMt = std::make_unique<btSequentialImpulseConstraintSolverMt>();

    _btWorld = std::make_unique<btDiscreteDynamicsWorldMt>(_btCollisionDispatcher.get(), _btBroadphase.get(),
                                                           _btSolverPool.get(), _btSolverMt.get(),
                                                           _btCollisionConfiguration.get());
    _btWorld->setGravity(BTVector3FromA3DVec3(world.gravity()));
    _btDebugDrawer = std::make_unique<BulletDebugDrawer>();
    _btWorld->setDebugDrawer(_btDebugDrawer.get());

    _debugLines.clear();
}

BulletWorldProxy::~BulletWorldProxy() {
    log::d()("Destroying BulletWorldProxy {:p}", static_cast<void*>(this));

    {
        std::scoped_lock lock(_btMutex);

        // remove constraints first (they reference bodies)
        for (int i = _btWorld->getNumConstraints() - 1; i >= 0; --i) {
            btTypedConstraint* c = _btWorld->getConstraint(i);
            _btWorld->removeConstraint(c);
        }

        // remove all collision objects / rigid bodies
        auto& arr = _btWorld->getCollisionObjectArray();
        for (int i = arr.size() - 1; i >= 0; --i) {
            btCollisionObject* obj = arr[i];
            if (btRigidBody* rb = btRigidBody::upcast(obj)) {
                _btWorld->removeRigidBody(rb);
            }
            else {
                _btWorld->removeCollisionObject(obj);
            }
        }

        _btWorld.reset();
    }

    // restore global scheduler if we were the ones using it
    if (btGetTaskScheduler() == _btScheduler) {
        btSetTaskScheduler(_prevScheduler ? _prevScheduler : btGetSequentialTaskScheduler());
    }
}

/// PhysicsWorldModelProxy Internal Member Functions ///

void BulletWorldProxy::add(PhysicsBody& body) {
    std::scoped_lock lock(_btMutex);

    log::d()("body: {:p}", static_cast<void*>(&body));

    auto bodyProxy = static_cast<BulletBodyProxy*>(body.proxy());
    auto btBody = bodyProxy->btBody();

    if (btBody->isInWorld()) { // ! NOTE: not necessarily THIS world
        log::w()("btRigidBody already in world.");
        return;
    }

    _btWorld->addRigidBody(btBody);

    if (btBody->isInWorld() && btBody->getBroadphaseHandle()) {
        _btWorld->updateSingleAabb(btBody);
    }

    switch (body.type()) {
        case PhysicsBody::Type::Static:
            ++_stats.numStaticBodies;
            break;
        case PhysicsBody::Type::Dynamic:
            ++_stats.numDynamicBodies;
            break;
        case PhysicsBody::Type::Kinematic:
            ++_stats.numKinematicBodies;
            break;
    }

    auto shapePtr = body.shape().get();
    if (shapePtr) {
        switch (shapePtr->type()) {
            case PhysicsShape::Type::ConvexHull:
                ++_stats.convexHullShapes[shapePtr];
                break;
            case PhysicsShape::Type::ConcavePolyhedron:
                ++_stats.concavePolyhedronShapes[shapePtr];
                break;
            case PhysicsShape::Type::BoundingBox:
                ++_stats.boundingBoxShapes[shapePtr];
                break;
            case PhysicsShape::Type::Primitive:
                ++_stats.primitiveShapes[shapePtr];
                break;
        }
    }
}

void BulletWorldProxy::remove(PhysicsBody& body) {
    std::scoped_lock lock(_btMutex);

    log::d()("body: {:p}", static_cast<void*>(&body));

    auto bodyProxy = static_cast<BulletBodyProxy*>(body.proxy());
    auto btBody = bodyProxy->btBody();

    if (!btBody->isInWorld()) { // ! NOTE:  not necessarily THIS world
        log::w()("btRigidBody not in world.");
        return;
    }

    _btWorld->removeRigidBody(btBody);

    switch (body.type()) {
        case PhysicsBody::Type::Static:
            --_stats.numStaticBodies;
            break;
        case PhysicsBody::Type::Dynamic:
            --_stats.numDynamicBodies;
            break;
        case PhysicsBody::Type::Kinematic:
            --_stats.numKinematicBodies;
            break;
    }

    auto shapePtr = body.shape().get();
    if (shapePtr) {
        switch (shapePtr->type()) {
            case PhysicsShape::Type::ConvexHull: {
                auto it = _stats.convexHullShapes.find(shapePtr);
                if (it != _stats.convexHullShapes.end() && --it->second == 0) {
                    _stats.convexHullShapes.erase(it);
                }
                break;
            }
            case PhysicsShape::Type::ConcavePolyhedron: {
                auto it = _stats.concavePolyhedronShapes.find(shapePtr);
                if (it != _stats.concavePolyhedronShapes.end() && --it->second == 0) {
                    _stats.concavePolyhedronShapes.erase(it);
                }
                break;
            }
            case PhysicsShape::Type::BoundingBox: {
                auto it = _stats.boundingBoxShapes.find(shapePtr);
                if (it != _stats.boundingBoxShapes.end() && --it->second == 0) {
                    _stats.boundingBoxShapes.erase(it);
                }
                break;
            }
            case PhysicsShape::Type::Primitive: {
                auto it = _stats.primitiveShapes.find(shapePtr);
                if (it != _stats.primitiveShapes.end() && --it->second == 0) {
                    _stats.primitiveShapes.erase(it);
                }
                break;
            }
        }
    }
}

vec3 BulletWorldProxy::gravity() const {

    std::scoped_lock lock(_btMutex);

    return A3DVec3FromBTVector3(_btWorld->getGravity());
}

void BulletWorldProxy::gravity(const vec3& gravity) {

    std::scoped_lock lock(_btMutex);

    const auto btGravity = BTVector3FromA3DVec3(gravity);
    _btWorld->setGravity(btGravity);

    auto& objects = _btWorld->getCollisionObjectArray();
    for (int i = 0; i < objects.size(); ++i) {
        auto* body = btRigidBody::upcast(objects[i]);
        if (!body || body->isStaticOrKinematicObject()) {
            continue;
        }
        if (body->getFlags() & BT_DISABLE_WORLD_GRAVITY) {
            continue;
        }
        body->setGravity(btGravity);
        body->activate(true);
    }
}

bool BulletWorldProxy::acceptsStepDelta(double deltaTime) const {

    if (!std::isfinite(deltaTime) || deltaTime <= 0.0
        || deltaTime > static_cast<double>(std::numeric_limits<btScalar>::max())) {
        return false;
    }

    const auto btDeltaTime = btScalar(deltaTime);

    return std::isfinite(btDeltaTime) && btDeltaTime > btScalar(0) && !btFuzzyZero(btDeltaTime);
}

const PhysicsWorldProxy::ContactEvents& BulletWorldProxy::step(double deltaTime, Profiler& profiler) {

    if (!acceptsStepDelta(deltaTime)) {
        throw invalid_argument("BulletWorldProxy::step() requires an accepted positive, finite delta time.");
    }

    _contactEvents.clear();

    auto result = prof::profile(profiler, Profiler::Tag::Physics, [&] {
        std::scoped_lock lock(_btMutex);
        const auto       result = _btWorld->stepSimulation(btScalar(deltaTime), 0);
        extractCurrentContacts();
        return result;
    });

    if (result != 1) {
        throw runtime_error("BulletWorldProxy::step() expected exactly one Bullet simulation step.");
    }

    return _contactEvents;
}

vector<HitTestResult> BulletWorldProxy::rayTest(const vec3&       from,
                                                const vec3&       to,
                                                HitTestSearchMode searchMode) const {

    scoped_lock lock(_btMutex);

    const auto btFrom = BTVector3FromA3DVec3(from);
    const auto btTo = BTVector3FromA3DVec3(to);

    auto makeHitResult = [](const btCollisionObject* collisionObject, const btVector3& hitPointWorld,
                            const btVector3& hitNormalWorld) -> optional<HitTestResult> {
        auto body = static_cast<PhysicsBody*>(collisionObject->getUserPointer());
        if (!body) {
            return nullopt;
        }

        auto node = body->node().lock();
        if (!node) {
            return nullopt;
        }

        const vec3 worldCoordinates = A3DVec3FromBTVector3(hitPointWorld);
        const vec3 worldNormal = normalize(A3DVec3FromBTVector3(hitNormalWorld));
        const mat4 modelTransform = node->worldTransform();
        const mat4 inverseModelTransform = inverse(modelTransform);
        const vec4 localCoordinates4 = inverseModelTransform * vec4 {worldCoordinates, 1.0f};
        const vec3 localCoordinates {localCoordinates4.x / localCoordinates4.w,
                                     localCoordinates4.y / localCoordinates4.w,
                                     localCoordinates4.z / localCoordinates4.w};
        const vec4 localNormal4 = transpose(modelTransform) * vec4 {worldNormal, 0.0f};
        const vec3 localNormal = normalize(vec3 {localNormal4});

        return HitTestResult {node,        nullptr,          nullptr,
                              nullopt,     localCoordinates, worldCoordinates,
                              localNormal, worldNormal,      modelTransform};
    };

    switch (searchMode) {

        case HitTestSearchMode::Any: {

            struct AnyRayResultCallback : btCollisionWorld::RayResultCallback {

                AnyRayResultCallback(const btVector3& from, const btVector3& to):
                    rayFromWorld {from},
                    rayToWorld {to} {}

                btScalar addSingleResult(btCollisionWorld::LocalRayResult& rayResult,
                                         bool                              normalInWorldSpace) override {

                    m_collisionObject = rayResult.m_collisionObject;

                    if (normalInWorldSpace) {
                        hitNormalWorld = rayResult.m_hitNormalLocal;
                    }
                    else {
                        hitNormalWorld =
                            m_collisionObject->getWorldTransform().getBasis() * rayResult.m_hitNormalLocal;
                    }

                    hitPointWorld.setInterpolate3(rayFromWorld, rayToWorld, rayResult.m_hitFraction);

                    // tell Bullet that nothing farther along the ray matters.
                    m_closestHitFraction = btScalar(0.0);

                    return btScalar(0.0);
                }

                btVector3 rayFromWorld;
                btVector3 rayToWorld;
                btVector3 hitNormalWorld;
                btVector3 hitPointWorld;
            };

            AnyRayResultCallback callback(btFrom, btTo);

            _btWorld->rayTest(btFrom, btTo, callback);

            if (!callback.hasHit()) {
                return {};
            }

            if (auto hit = makeHitResult(callback.m_collisionObject, callback.hitPointWorld,
                                         callback.hitNormalWorld)) {
                return {std::move(*hit)};
            }

            return {};
        }

        case HitTestSearchMode::Closest: {

            btCollisionWorld::ClosestRayResultCallback callback(btFrom, btTo);

            _btWorld->rayTest(btFrom, btTo, callback);

            if (!callback.hasHit()) {
                return {};
            }

            if (auto hit = makeHitResult(callback.m_collisionObject, callback.m_hitPointWorld,
                                         callback.m_hitNormalWorld)) {
                return {std::move(*hit)};
            }

            return {};
        }

        case HitTestSearchMode::All: {

            btCollisionWorld::AllHitsRayResultCallback callback(btFrom, btTo);

            _btWorld->rayTest(btFrom, btTo, callback);

            if (!callback.hasHit()) {
                return {};
            }

            vector<int> order;
            order.reserve(callback.m_hitFractions.size());

            for (int i = 0; i < callback.m_hitFractions.size(); ++i) {
                order.push_back(i);
            }

            sort(order.begin(), order.end(), [&](int a, int b) {
                return callback.m_hitFractions[a] < callback.m_hitFractions[b];
            });

            vector<HitTestResult> hits;
            hits.reserve(order.size());

            unordered_set<const btCollisionObject*> seen;

            for (int i : order) {

                const auto* collisionObject = callback.m_collisionObjects[i];

                // a body may produce multiple low-level Bullet hits
                // (for example, entry/exit surfaces). keep only its nearest hit.
                if (!seen.insert(collisionObject).second) {
                    continue;
                }

                if (auto hit = makeHitResult(collisionObject, callback.m_hitPointWorld[i],
                                             callback.m_hitNormalWorld[i])) {
                    hits.push_back(std::move(*hit));
                }
            }

            return hits;
        }
    }

    return {};
}

PhysicsInventory BulletWorldProxy::inventory() const {
    std::scoped_lock lock(_btMutex);

    return {.staticBodies = _stats.numStaticBodies,
            .dynamicBodies = _stats.numDynamicBodies,
            .kinematicBodies = _stats.numKinematicBodies,
            .primitiveShapes = static_cast<unsigned>(_stats.primitiveShapes.size()),
            .boundingBoxShapes = static_cast<unsigned>(_stats.boundingBoxShapes.size()),
            .convexHullShapes = static_cast<unsigned>(_stats.convexHullShapes.size()),
            .concavePolyhedronShapes = static_cast<unsigned>(_stats.concavePolyhedronShapes.size())};
}

void BulletWorldProxy::updateCollisionPairs() {
    std::scoped_lock lock(_btMutex);
    _btWorld->getCollisionWorld()->computeOverlappingPairs();
}

void BulletWorldProxy::appendDebugLines(vector<Line>& out, Scene::DebugOptions debugOptions) {

    static constexpr float DEBUG_LINE_UPDATE_RATE = 30.0f;

    static constexpr auto DEBUG_LINE_UPDATE_INTERVAL =
        std::chrono::duration_cast<std::chrono::steady_clock::duration>(std::chrono::duration<float> {
            1.0f / DEBUG_LINE_UPDATE_RATE});

    const auto debugMode = BTDebugDrawModesForA3DDebugOptions(debugOptions);
    const auto now = chrono::steady_clock::now();

    const auto modeValue = static_cast<int>(debugMode);
    const bool modeChanged = modeValue != _debugDrawMode;

    if (debugMode == btIDebugDraw::DBG_NoDebug) {
        _debugLines.clear();
        _debugDrawMode = modeValue;
        _nextDebugLineUpdate = {};
        return;
    }

    if (modeChanged || now >= _nextDebugLineUpdate) {
        std::scoped_lock lock(_btMutex);

        _btDebugDrawer->setDebugMode(debugMode);
        _btDebugDrawer->clear();

        _btWorld->debugDrawWorld();

        _debugLines = std::move(_btDebugDrawer->lines());
        _debugDrawMode = modeValue;
        _nextDebugLineUpdate = now + DEBUG_LINE_UPDATE_INTERVAL;
    }

    out.insert(out.end(), _debugLines.begin(), _debugLines.end());
}

/// Internal Member Functions ///

btDiscreteDynamicsWorld* BulletWorldProxy::btWorld() {
    return _btWorld.get();
}

/// Private Member Functions ///

void BulletWorldProxy::extractCurrentContacts() {

    _currentContacts.clear();

    const auto bodyLess = std::less<PhysicsBody*> {};

    const int manifoldCount = _btCollisionDispatcher->getNumManifolds();

    for (int manifoldIndex = 0; manifoldIndex < manifoldCount; ++manifoldIndex) {

        auto manifold = _btCollisionDispatcher->getManifoldByIndexInternal(manifoldIndex);
        if (!manifold) {
            continue;
        }

        auto rawBodyA = static_cast<PhysicsBody*>(manifold->getBody0()->getUserPointer());
        auto rawBodyB = static_cast<PhysicsBody*>(manifold->getBody1()->getUserPointer());

        if (!rawBodyA || !rawBodyB || rawBodyA == rawBodyB) {
            continue;
        }

        const bool swapped = bodyLess(rawBodyB, rawBodyA);

        PhysicsBody* bodyA = swapped ? rawBodyB : rawBodyA;
        PhysicsBody* bodyB = swapped ? rawBodyA : rawBodyB;

        auto nodeA = bodyA->node().lock();
        auto nodeB = bodyB->node().lock();

        if (!nodeA || !nodeB) {
            continue;
        }

        for (int contactIndex = 0; contactIndex < manifold->getNumContacts(); ++contactIndex) {

            const auto& point = manifold->getContactPoint(contactIndex);

            if (point.getDistance() > btScalar(0)) {
                continue;
            }

            const btVector3 btContactPoint =
                (point.getPositionWorldOnA() + point.getPositionWorldOnB()) * btScalar(0.5);

            btVector3 btContactNormal = point.m_normalWorldOnB;

            if (swapped) {
                btContactNormal = -btContactNormal;
            }

            const vec3 contactPoint = A3DVec3FromBTVector3(btContactPoint);
            const vec3 contactNormal = A3DVec3FromBTVector3(btContactNormal);

            const float collisionImpulse = static_cast<float>(point.m_appliedImpulse);

            const float penetrationDistance = math::max(0.0f, -static_cast<float>(point.getDistance()));

            _currentContacts.push_back({bodyA, bodyB,
                                        PhysicsContact {nodeA, nodeB, contactPoint, contactNormal,
                                                        collisionImpulse, penetrationDistance, 0.0f}});
        }
    }

    sort(_currentContacts.begin(), _currentContacts.end(),
         [&](const BodyPairContact& lhs, const BodyPairContact& rhs) {
             if (bodyLess(lhs.bodyA, rhs.bodyA)) {
                 return true;
             }
             if (bodyLess(rhs.bodyA, lhs.bodyA)) {
                 return false;
             }

             if (bodyLess(lhs.bodyB, rhs.bodyB)) {
                 return true;
             }
             if (bodyLess(rhs.bodyB, lhs.bodyB)) {
                 return false;
             }

             if (lhs.contact.collisionImpulse() > rhs.contact.collisionImpulse()) {
                 return true;
             }
             if (lhs.contact.collisionImpulse() < rhs.contact.collisionImpulse()) {
                 return false;
             }

             return lhs.contact.penetrationDistance() > rhs.contact.penetrationDistance();
         });

    const auto uniqueEnd = unique(_currentContacts.begin(), _currentContacts.end(),
                                  [](const BodyPairContact& lhs, const BodyPairContact& rhs) {
                                      return lhs.bodyA == rhs.bodyA && lhs.bodyB == rhs.bodyB;
                                  });

    _currentContacts.erase(uniqueEnd, _currentContacts.end());
}

/// Private Static Non-Member Functions ///

int PickNumBTThreads(btITaskScheduler* sched) {
    const int hw = math::max(1u, std::thread::hardware_concurrency());
    const int maxT = sched ? sched->getMaxNumThreads() : hw;
    // bullet MT often benefits from "not all cores", but start simple...
    return math::clamp(hw, 1, maxT);
}

btIDebugDraw::DebugDrawModes BTDebugDrawModesForA3DDebugOptions(const Scene::DebugOptions& options) {

    using DebugOptions = Scene::DebugOptions;

    btIDebugDraw::DebugDrawModes btModes = btIDebugDraw::DBG_NoDebug;

    if (util::bitmask::contains(options, DebugOptions::ShowPhysicsBoundingBoxes)) {
        btModes = (btIDebugDraw::DebugDrawModes) (btModes | btIDebugDraw::DBG_DrawAabb);
    }
    if (util::bitmask::contains(options, DebugOptions::ShowPhysicsWireframes)) {
        btModes = (btIDebugDraw::DebugDrawModes) (btModes | btIDebugDraw::DBG_DrawWireframe);
    }
    if (util::bitmask::contains(options, DebugOptions::ShowPhysicsContactPoints)) {
        btModes = (btIDebugDraw::DebugDrawModes) (btModes | btIDebugDraw::DBG_DrawContactPoints);
    }
    if (util::bitmask::contains(options, DebugOptions::ShowPhysicsNormals)) {
        btModes = (btIDebugDraw::DebugDrawModes) (btModes | btIDebugDraw::DBG_DrawNormals);
    }
    if (util::bitmask::contains(options, DebugOptions::ShowPhysicsConstraints)) {
        btModes = (btIDebugDraw::DebugDrawModes) (btModes | btIDebugDraw::DBG_DrawConstraints);
    }
    if (util::bitmask::contains(options, DebugOptions::ShowPhysicsConstraintLimits)) {
        btModes = (btIDebugDraw::DebugDrawModes) (btModes | btIDebugDraw::DBG_DrawConstraintLimits);
    }

    /* what do these do?

	 btModes = (btIDebugDraw::DebugDrawModes)
	 (btModes | btIDebugDraw::DBG_ProfileTimings);

	 btModes = (btIDebugDraw::DebugDrawModes)
	 (btModes | btIDebugDraw::DBG_DrawFeaturesText);

	 btModes = (btIDebugDraw::DebugDrawModes)
	 (btModes | btIDebugDraw::DBG_DrawFrames);

	 btModes = (btIDebugDraw::DebugDrawModes)
	 (btModes | btIDebugDraw::DBG_EnableCCD); */

    static btIDebugDraw::DebugDrawModes previousModes = btIDebugDraw::DBG_NoDebug;
    if (btModes != previousModes) {
        log::d()("Bullet debug modes: {}", magic_enum::enum_name(btModes));
    }
    previousModes = btModes;

    return btModes;
}
