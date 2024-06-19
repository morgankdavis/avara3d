//
//  BulletWorldProxy.cc
//  avara3d
//
//  Created by Morgan Davis on 12/8/23.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#include "a3d/physics/bullet/BulletWorldProxy.h"

#include "btBulletCollisionCommon.h"
#include "btBulletDynamicsCommon.h"
#include "BulletCollision/Gimpact/btGImpactShape.h"
#include "glm/gtc/type_ptr.hpp"
#include "LinearMath/btIDebugDraw.h"
#include "magic_enum.hpp"

#include "a3d/Configuration.h"
#include "a3d/diagnostic/logging/Logger.h"
#include "a3d/physics/PhysicsBody.h"
#include "a3d/physics/PhysicsShape.h"
#include "a3d/physics/bullet/BulletBodyProxy.h"
#include "a3d/physics/bullet/BulletDebugDrawer.h"
#include "a3d/physics/bullet/BulletUtilities.h"
#include "a3d/scene/Node.h"


using namespace a3d;
using namespace glm;
using namespace std;


/*********************************************************************************************
	Private Static Non-Member Prototypes
 *********************************************************************************************/

static btIDebugDraw::DebugDrawModes BTDebugDrawModesForA3DDebugOptions(const DebugOptions& options);

/*********************************************************************************************
	Internal Lifecycle
 *********************************************************************************************/

BulletWorldProxy::BulletWorldProxy(PhysicalWorld& world):
		PhysicalWorldProxy{world},
		_stats{} {

	_btCollisionConfiguration = make_unique<btDefaultCollisionConfiguration>();
	_btCollisionDispatcher = make_unique<btCollisionDispatcher>(_btCollisionConfiguration.get());
	_btBroadphase = make_unique<btDbvtBroadphase>();
	_btConstraintSolver = make_unique<btSequentialImpulseConstraintSolver>();
	_btWorld = make_unique<btDiscreteDynamicsWorld>(_btCollisionDispatcher.get(),
													_btBroadphase.get(),
													_btConstraintSolver.get(),
													_btCollisionConfiguration.get());

	A3D_LOG_I("Bullet Physics version: {}",  btGetVersion());

#ifdef OPENGL_CORE
	_btDebugDrawer = make_unique<BulletDebugDrawer>();
	_btWorld->setDebugDrawer(_btDebugDrawer.get());
#endif
}

BulletWorldProxy::~BulletWorldProxy() {

	A3D_LOG_D("Destroying BulletWorldProxy {:p}", static_cast<void*>(this));
}

/*********************************************************************************************
	PhysicalWorldModelProxy Internal Members
 *********************************************************************************************/

void BulletWorldProxy::add(PhysicsBody& body) {
	A3D_LOG_D("body: {:p}", static_cast<void*>(&body));

	auto bodyProxy = static_cast<BulletBodyProxy*>(body.proxy());
	auto btBody = bodyProxy->btBody();

	// since at the time of creation, the PhysicsBody isn't attached to a Node,
	// we use an empty motion state in BulletBodyProxy::BulletBodyProxy(),
	// so the btBody's transform is the identity matrix.
	// now that the body has a Node, set its initial transform here.
	if (auto node = body.node().lock()) {
		auto nodeTransform = node->worldTransform();
		auto btTransform = BTTransformFromGLMMat4(nodeTransform);
		btBody->setWorldTransform(btTransform);
		// without proceedToTransform(), objects still spawn at the origin for 1st step (?)
		btBody->proceedToTransform(btTransform);
	}
	else {
		A3D_LOG_W("Adding PhysicsBody without a Node??");
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
	A3D_LOG_D("body: {:p}", static_cast<void*>(&body));

	auto bodyProxy = static_cast<BulletBodyProxy*>(body.proxy());
	_btWorld->removeRigidBody(bodyProxy->btBody());

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
	return _btWorld->getGravity().y();
}

void BulletWorldProxy::gravity(float gravity) {
	_btWorld->setGravity({0, gravity, 0});
}

void BulletWorldProxy::step(double deltaT,
							float speed,
							float timestep,
							Stats& stats) {

	auto result = _btWorld->stepSimulation(btScalar(deltaT * speed),
										   MAX_PHYSICS_SUBSTEPS,
										   timestep);

//	if (result >= MAX_PHYSICS_SUBSTEPS) {
//		A3D_LOG_W("Max physics simulation substeps reached: {}", result);
//	}

	stats.staticBodies += _stats.numStaticBodies;
	stats.dynamicBodies += _stats.numDynamicBodies;
	stats.kinematicBodies += _stats.numKinematicBodies;
	stats.convexHullShapes = _stats.convexHullShapes.size();
	stats.concavePolyhedronShapes = _stats.concavePolyhedronShapes.size();
	stats.boundingBoxShapes = _stats.boundingBoxShapes.size();
	stats.primitiveShapes = _stats.primitiveShapes.size();
}

void BulletWorldProxy::updateCollisionPairs() {
	_btWorld->getCollisionWorld()->computeOverlappingPairs();
}

void BulletWorldProxy::drawDebug(Renderer &renderer,
								 const glm::mat4 &viewMat,
								 const glm::mat4 &projectionMat,
								 const DebugOptions &debugOptions) {

#ifdef OPENGL_CORE
	auto btDebugModes = BTDebugDrawModesForA3DDebugOptions(debugOptions);

	_btDebugDrawer->setDebugMode(btDebugModes);
	_btDebugDrawer->clear();
	_btWorld->debugDrawWorld();
	_btDebugDrawer->draw(renderer, viewMat, projectionMat);
#endif
}

/*********************************************************************************************
	Private Static Non-Members
 *********************************************************************************************/

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
		A3D_LOG_D("Bullet debug modes: {}", magic_enum::enum_name(btModes));
	}
	previousModes = btModes;

	return btModes;
}
