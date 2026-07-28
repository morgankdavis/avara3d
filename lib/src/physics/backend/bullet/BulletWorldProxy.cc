//
//  BulletWorldProxy.cc
//  avara3d
//
//  Created by Morgan Davis on 12/8/23.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#include "a3d/physics/backend/bullet/BulletWorldProxy.h"

#include <algorithm>
#include <thread>

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

#include "a3d/Configuration.h"
#include "a3d/log/Log.h"
#include "a3d/physics/PhysicsBody.h"
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

// make sure bullet is built with MT enabled
//#if !defined(BT_THREADSAFE) || (BT_THREADSAFE != 1)
//#   error "Bullet requires building with BT_THREADSAFE=1"
//#endif

// native builds use bullet's multithreaded world/solver path, so require
// bullet to be compiled with thread-safe support. normal browser/wasm builds
// currently force bullet's sequential scheduler instead; they should not require
// bullet worker-thread support unless/until we ship a separate pthread wasm build
#if !(defined(__EMSCRIPTEN__) && !defined(__EMSCRIPTEN_PTHREADS__))
#   if !defined(BT_THREADSAFE) || (BT_THREADSAFE != 1)
#       error "Native threaded Bullet requires building with BT_THREADSAFE=1"
#   endif
#endif

/// Private Static Non-Member Prototypes ///

static btIDebugDraw::DebugDrawModes BTDebugDrawModesForA3DDebugOptions(
		const Scene::DebugOptions& options);
static int PickNumBTThreads(btITaskScheduler* sched);

/// Internal Lifecycle Functions ///

BulletWorldProxy::BulletWorldProxy(PhysicsWorld& world):
		PhysicsWorldProxy{world},
		_stats{}/*,
		_debugLines{}*/ {

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
    if (!_btScheduler) _btScheduler = btGetTBBTaskScheduler();
    if (!_btScheduler) _btScheduler = btGetPPLTaskScheduler();
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

	log::i()("Bullet task scheduler: {} (threads: {}/{})",
			 _btScheduler->getName(),
			 _btScheduler->getNumThreads(),
			 _btScheduler->getMaxNumThreads());

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
	_btSolverMt   = std::make_unique<btSequentialImpulseConstraintSolverMt>();

	_btWorld = std::make_unique<btDiscreteDynamicsWorldMt>(
			_btCollisionDispatcher.get(),
			_btBroadphase.get(),
			_btSolverPool.get(),
			_btSolverMt.get(),
			_btCollisionConfiguration.get()
	);

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
			} else {
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

	// keep motion state, rigid body transform, interpolation transform,
	// and broadphase AABB synchronized before the first simulation step
	if (auto node = body.node().lock()) {
		auto nodeTransform = node->worldTransform();
		auto btTransform = BTTransformFromA3DMat4(nodeTransform);

		if (auto* ms = btBody->getMotionState()) {
			ms->setWorldTransform(btTransform);
		}

		btBody->setWorldTransform(btTransform);
		btBody->setInterpolationWorldTransform(btTransform);
		btBody->proceedToTransform(btTransform);
		btBody->activate(true);
	}
	else {
		log::w()("Adding PhysicsBody without a Node??");
		// TODO: throw?
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
					_stats.convexHullShapes.insert(shapePtr);
				break;
			case PhysicsShape::Type::ConcavePolyhedron:
					_stats.concavePolyhedronShapes.insert(shapePtr);
				break;
			case PhysicsShape::Type::BoundingBox:
					_stats.boundingBoxShapes.insert(shapePtr);
				break;
			case PhysicsShape::Type::Primitive:
				_stats.primitiveShapes.insert(shapePtr);
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
	switch (shapePtr->type()) {
		case PhysicsShape::Type::ConvexHull:
			_stats.convexHullShapes.erase(shapePtr);
			break;
		case PhysicsShape::Type::ConcavePolyhedron:
			_stats.concavePolyhedronShapes.erase(shapePtr);
			break;
		case PhysicsShape::Type::BoundingBox:
			_stats.boundingBoxShapes.erase(shapePtr);
			break;
		case PhysicsShape::Type::Primitive:
			_stats.primitiveShapes.erase(shapePtr);
			break;
	}
}

float BulletWorldProxy::gravity() const {
	std::scoped_lock lock(_btMutex);
	return _btWorld->getGravity().y();
}

void BulletWorldProxy::gravity(float gravity) {
	std::scoped_lock lock(_btMutex);
	_btWorld->setGravity({0, gravity, 0});
}

void BulletWorldProxy::step(double deltaT,
							float speed,
							float timestep,
							Profiler& profiler) {

	auto result = prof::profile(profiler, Profiler::Tag::Physics, [&] {

		std::scoped_lock lock(_btMutex);

		return _btWorld->stepSimulation(btScalar(deltaT * speed),
										config::MAX_PHYSICS_SUBSTEPS,
										timestep);
	});

//	if (result > config::MAX_PHYSICS_SUBSTEPS) {
//		log::w()("Max physics simulation substeps exceeded: {}/{}",
//				  result, config::MAX_PHYSICS_SUBSTEPS);
//	}
}

PhysicsInventory BulletWorldProxy::inventory() const {
	std::scoped_lock lock(_btMutex);

	return {
		.staticBodies = _stats.numStaticBodies,
		.dynamicBodies = _stats.numDynamicBodies,
		.kinematicBodies = _stats.numKinematicBodies,
		.primitiveShapes = static_cast<unsigned>(_stats.primitiveShapes.size()),
		.boundingBoxShapes = static_cast<unsigned>(_stats.boundingBoxShapes.size()),
		.convexHullShapes = static_cast<unsigned>(_stats.convexHullShapes.size()),
		.concavePolyhedronShapes =
			static_cast<unsigned>(_stats.concavePolyhedronShapes.size())
	};
}

void BulletWorldProxy::updateCollisionPairs() {
	std::scoped_lock lock(_btMutex);
	_btWorld->getCollisionWorld()->computeOverlappingPairs();
}

void BulletWorldProxy::appendDebugLines(vector<Line>& out,
										Scene::DebugOptions debugOptions) {

	// TODO: this is still rather inefficient.

//#ifdef A3D_GL_DESKTOP

	static const float UPDATE_RATE = 30.0; // frames/sec

	util::flow::every(chrono::duration<float>(1.0f/UPDATE_RATE), [&] {

		_debugLines.clear();

		auto btDebugModes = BTDebugDrawModesForA3DDebugOptions(debugOptions);
		if (btDebugModes == btIDebugDraw::DBG_NoDebug) return;

		std::scoped_lock lock(_btMutex);

		_btDebugDrawer->setDebugMode(btDebugModes);
		_btDebugDrawer->clear();

		_btWorld->debugDrawWorld();

		_debugLines = std::move(_btDebugDrawer->lines());
	});

	for (auto& line : _debugLines) {
		out.push_back(line);
	}

//#endif
}

/// Internal Member Functions ///

btDiscreteDynamicsWorld* BulletWorldProxy::btWorld() {
	return _btWorld.get();
}

/// Private Static Non-Member Functions ///

int PickNumBTThreads(btITaskScheduler* sched) {
	const int hw = math::max(1u, std::thread::hardware_concurrency());
	const int maxT = sched ? sched->getMaxNumThreads() : hw;
	// bullet MT often benefits from "not all cores", but start simple...
	return math::clamp(hw, 1, maxT);
}

btIDebugDraw::DebugDrawModes BTDebugDrawModesForA3DDebugOptions(
		const Scene::DebugOptions& options) {

	using DebugOptions = Scene::DebugOptions;

	btIDebugDraw::DebugDrawModes btModes = btIDebugDraw::DBG_NoDebug;

	if (util::bitmask::contains(options, DebugOptions::ShowPhysicsBoundingBoxes)) {
		btModes = (btIDebugDraw::DebugDrawModes)(btModes | btIDebugDraw::DBG_DrawAabb);
	}
	if (util::bitmask::contains(options, DebugOptions::ShowPhysicsWireframes)) {
		btModes = (btIDebugDraw::DebugDrawModes)(btModes | btIDebugDraw::DBG_DrawWireframe);
	}
	if (util::bitmask::contains(options, DebugOptions::ShowPhysicsContactPoints)) {
		btModes = (btIDebugDraw::DebugDrawModes)(btModes | btIDebugDraw::DBG_DrawContactPoints);
	}
	if (util::bitmask::contains(options, DebugOptions::ShowPhysicsNormals)) {
		btModes = (btIDebugDraw::DebugDrawModes)(btModes | btIDebugDraw::DBG_DrawNormals);
	}
	if (util::bitmask::contains(options, DebugOptions::ShowPhysicsConstraints)) {
		btModes = (btIDebugDraw::DebugDrawModes)(btModes | btIDebugDraw::DBG_DrawConstraints);
	}
	if (util::bitmask::contains(options, DebugOptions::ShowPhysicsConstraintLimits)) {
		btModes = (btIDebugDraw::DebugDrawModes)(btModes | btIDebugDraw::DBG_DrawConstraintLimits);
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
