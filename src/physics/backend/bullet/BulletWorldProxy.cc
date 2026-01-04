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
#include "a3d/physics/PhysicsShape.h"
#include "a3d/physics/backend/bullet/BulletBodyProxy.h"
#include "a3d/physics/backend/bullet/BulletDebugDrawer.h"
#include "a3d/physics/backend/bullet/BulletUtilities.h"
#include "a3d/profiling/Profiling.h"
#include "a3d/scene/Node.h"
#include "a3d/util/flow.h"

using namespace a3d;
using namespace a3d::math;
using namespace std;

// ! btCollisionDispatcherMt is known to be buggy. leave it off.
static constexpr bool A3D_USE_MT_DISPATCHER = false;

// make sure bullet is built with MT enabled
#if !defined(BT_THREADSAFE) || (BT_THREADSAFE != 1)
#   error "Bullet requires building with BT_THREADSAFE=1"
#endif

/// Private Static Non-Member Prototypes ///

static btIDebugDraw::DebugDrawModes BTDebugDrawModesForA3DDebugOptions(const DebugOptions& options);
static int PickNumBTThreads(btITaskScheduler* sched);

/// Internal Lifecycle Functions ///

BulletWorldProxy::BulletWorldProxy(PhysicalWorld& world):
		PhysicalWorldProxy{world},
		_stats{}/*,
		_debugLines{}*/ {

	log::i()("Bullet Physics version: {}", btGetVersion());

	_btScheduler = btGetOpenMPTaskScheduler();
	if (!_btScheduler) _btScheduler = btGetTBBTaskScheduler();
	if (!_btScheduler) _btScheduler = btGetPPLTaskScheduler();
	if (!_btScheduler) {
		_ownedScheduler.reset(btCreateDefaultTaskScheduler());
		_btScheduler = _ownedScheduler.get();
	}

	_prevScheduler = btGetTaskScheduler();
	btSetTaskScheduler(_btScheduler);

	const int numThreads = PickNumBTThreads(_btScheduler);
	_btScheduler->setNumThreads(numThreads);

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

#ifdef A3D_GL_DESKTOP
	_btDebugDrawer = std::make_unique<BulletDebugDrawer>();
	_btWorld->setDebugDrawer(_btDebugDrawer.get());
	_debugLines.clear();
#endif
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

/// PhysicalWorldModelProxy Internal Member Functions ///

void BulletWorldProxy::add(PhysicsBody& body) {
	std::scoped_lock lock(_btMutex);

	log::d()("body: {:p}", static_cast<void*>(&body));

	auto bodyProxy = static_cast<BulletBodyProxy*>(body.proxy());
	auto btBody = bodyProxy->btBody();

	if (btBody->isInWorld()) { // ! NOTE:  not necessarily THIS world
		log::w()("btRigidBody already in world.");
		return;
	}

	// since at the time of creation, the PhysicsBody isn't attached to a Node,
	// we use an empty motion state in BulletBodyProxy::BulletBodyProxy(),
	// so the btBody's transform is the identity matrix.
	// now that the body has a Node, set its initial transform here.
	if (auto node = body.node().lock()) {
		auto nodeTransform = node->worldTransform();
		auto btTransform = BTTransformFromA3DMat4(nodeTransform);
		btBody->setWorldTransform(btTransform);
		// without proceedToTransform(), objects still spawn at the origin for 1st step (?)
		btBody->proceedToTransform(btTransform);
	}
	else {
		log::w()("Adding PhysicsBody without a Node??");
		// TODO: throw?
	}

	_btWorld->addRigidBody(btBody);

	switch (body.type()) {
		case PhysicsBodyType::Static:
			++_stats.numStaticBodies;
			break;
		case PhysicsBodyType::Dynamic:
			++_stats.numDynamicBodies;
			break;
		case PhysicsBodyType::Kinematic:
			++_stats.numKinematicBodies;
			break;
	}

	auto shapePtr = body.shape().get();
	if (shapePtr) {
		switch (shapePtr->type()) {
			case PhysicsShapeType::ConvexHull:
					_stats.convexHullShapes.insert(shapePtr);
				break;
			case PhysicsShapeType::ConcavePolyhedron:
					_stats.concavePolyhedronShapes.insert(shapePtr);
				break;
			case PhysicsShapeType::BoundingBox:
					_stats.boundingBoxShapes.insert(shapePtr);
				break;
			case PhysicsShapeType::Primitive:
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
		case PhysicsBodyType::Static:
			--_stats.numStaticBodies;
			break;
		case PhysicsBodyType::Dynamic:
			--_stats.numDynamicBodies;
			break;
		case PhysicsBodyType::Kinematic:
			--_stats.numKinematicBodies;
			break;
	}

	auto shapePtr = body.shape().get();
	switch (shapePtr->type()) {
		case PhysicsShapeType::ConvexHull:
			_stats.convexHullShapes.erase(shapePtr);
			break;
		case PhysicsShapeType::ConcavePolyhedron:
			_stats.concavePolyhedronShapes.erase(shapePtr);
			break;
		case PhysicsShapeType::BoundingBox:
			_stats.boundingBoxShapes.erase(shapePtr);
			break;
		case PhysicsShapeType::Primitive:
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
							FrameStats& stats,
							Profiler& profiler) {

	auto result = prof::profile(profiler, Profiler::Tag::Physics, [&] {

		std::scoped_lock lock(_btMutex);

//		// (optional debug check)
//		const auto& arr = _btWorld->getCollisionObjectArray();
//		for (int i = 0; i < arr.size(); ++i) {
//			const btCollisionObject* obj = arr[i];
//			if (!btRigidBody::upcast(obj)) {
//				log::e()("Non-rigid collision object in dynamics world! idx={} ptr={:p} flags=0x{:x}",
//						 i, (void*)obj, obj->getCollisionFlags());
//				btAssert(false);
//			}
//		}

		return _btWorld->stepSimulation(btScalar(deltaT * speed),
										config::MAX_PHYSICS_SUBSTEPS,
										timestep);
	});

//	if (result > config::MAX_PHYSICS_SUBSTEPS) {
//		log::w()("Max physics simulation substeps exceeded: {}/{}",
//				  result, config::MAX_PHYSICS_SUBSTEPS);
//	}

	stats.numStaticBodies += _stats.numStaticBodies;
	stats.numDynamicBodies += _stats.numDynamicBodies;
	stats.numKinematicBodies += _stats.numKinematicBodies;
	stats.numConvexHullShapes = _stats.convexHullShapes.size();
	stats.numConcavePolyhedronShapes = _stats.concavePolyhedronShapes.size();
	stats.numBoundingBoxShapes = _stats.boundingBoxShapes.size();
	stats.numPrimitiveShapes = _stats.primitiveShapes.size();
}

void BulletWorldProxy::updateCollisionPairs() {
	std::scoped_lock lock(_btMutex);
	_btWorld->getCollisionWorld()->computeOverlappingPairs();
}

vector<Line> BulletWorldProxy::debugLines(const DebugOptions &debugOptions) {

#ifdef A3D_GL_DESKTOP

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

	return _debugLines;
#else
	return vector<Line>{};
#endif
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

btIDebugDraw::DebugDrawModes BTDebugDrawModesForA3DDebugOptions(const DebugOptions& options) {
	btIDebugDraw::DebugDrawModes btModes = btIDebugDraw::DBG_NoDebug;

	if (A3D_MASK_CONTAINS(options, DebugOptions::ShowPhysicsBoundingBoxes)) {
		btModes = (btIDebugDraw::DebugDrawModes)(btModes | btIDebugDraw::DBG_DrawAabb);
	}
	if (A3D_MASK_CONTAINS(options, DebugOptions::ShowPhysicsWireframes)) {
		btModes = (btIDebugDraw::DebugDrawModes)(btModes | btIDebugDraw::DBG_DrawWireframe);
	}
	if (A3D_MASK_CONTAINS(options, DebugOptions::ShowPhysicsContactPoints)) {
		btModes = (btIDebugDraw::DebugDrawModes)(btModes | btIDebugDraw::DBG_DrawContactPoints);
	}
	if (A3D_MASK_CONTAINS(options, DebugOptions::ShowPhysicsNormals)) {
		btModes = (btIDebugDraw::DebugDrawModes)(btModes | btIDebugDraw::DBG_DrawNormals);
	}
	if (A3D_MASK_CONTAINS(options, DebugOptions::ShowPhysicsConstraints)) {
		btModes = (btIDebugDraw::DebugDrawModes)(btModes | btIDebugDraw::DBG_DrawConstraints);
	}
	if (A3D_MASK_CONTAINS(options, DebugOptions::ShowPhysicsConstraintLimits)) {
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
