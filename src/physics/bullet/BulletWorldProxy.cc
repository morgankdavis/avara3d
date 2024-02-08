//
// Created by mkd on 12/8/23.
//

#include "ae/physics/bullet/BulletWorldProxy.h"

#include "btBulletCollisionCommon.h"
#include "btBulletDynamicsCommon.h"
#include "BulletCollision/Gimpact/btGImpactShape.h"
#include "glm/gtc/type_ptr.hpp"
#include "LinearMath/btIDebugDraw.h"
#include "magic_enum.hpp"

#include "ae/diagnostic/logging/Logger.h"
#include "ae/physics/PhysicsBody.h"
#include "ae/physics/bullet/BulletBodyProxy.h"
#include "ae/physics/bullet/BulletDebugDrawer.h"
#include "ae/scene/Node.h"


using namespace ae;
using namespace glm;
using namespace std;


constexpr unsigned MAX_SUBSTEPS = 0; // move

/*********************************************************************************************
	Static Prototypes
 *********************************************************************************************/

static btIDebugDraw::DebugDrawModes BTDebugDrawModesForAEDebugOptions(const DebugOptions& options);

/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

BulletWorldProxy::BulletWorldProxy(PhysicalWorld* world):
		PhysicalWorldProxy(world) {

	_btCollisionConfiguration = make_unique<btDefaultCollisionConfiguration>();
	_btCollisionDispatcher = make_unique<btCollisionDispatcher>(_btCollisionConfiguration.get());
	_btBroadphase = make_unique<btDbvtBroadphase>();
	_btConstraintSolver = make_unique<btSequentialImpulseConstraintSolver>();
	_btWorld = make_unique<btDiscreteDynamicsWorld>(_btCollisionDispatcher.get(),
												  _btBroadphase.get(),
												  _btConstraintSolver.get(),
												  _btCollisionConfiguration.get());

	AE_LOG_I("Bullet Physics version: {}",  btGetVersion());

#ifdef OPENGL_DESKTOP
	_btDebugDrawer = make_unique<BulletDebugDrawer>();
	_btWorld->setDebugDrawer(_btDebugDrawer.get());
#endif
}

BulletWorldProxy::~BulletWorldProxy()/*:
		PhysicalWorldModelProxy::PhysicalWorldModelProxyProxy()*/ {

	//PhysicalWorldModelProxy::PhysicalWorldModelProxyProxy();

	AE_LOG_D("Destroying BulletWorldProxy {:p}", static_cast<void*>(this));
}

/*********************************************************************************************
	PhysicalWorldModelProxy
 *********************************************************************************************/

void BulletWorldProxy::add(PhysicsBody& body) {
	AE_LOG_D("body: {:p}", static_cast<void*>(&body));

	auto bodyProxy = static_cast<BulletBodyProxy*>(body.proxy());
	//bodyModel->btBody()->setWorldTransform(BTTransformFromGLMMat4(body.node()->worldTransform()));
	auto b = bodyProxy->btBody().get();
	_btWorld->addRigidBody(b);
}

void BulletWorldProxy::remove(PhysicsBody& body) {
	AE_LOG_D("body: {:p}", static_cast<void*>(&body));

	auto bodyProxy = static_cast<BulletBodyProxy*>(body.proxy());
	_btWorld->removeRigidBody(bodyProxy->btBody().get());
}

//unique_ptr<vector<PhysicsBody*>> BulletWorldProxy::bodies() const {
//
//	auto collisionObjects = _btWorld->getCollisionObjectArray();
//	auto numCollisionObjects = _btWorld->getNumCollisionObjects();
//	auto bodies = make_unique<vector<PhysicsBody*>>();
//	bodies->reserve(numCollisionObjects);
//	for (int o=0; o<numCollisionObjects; ++o) {
//		auto object = collisionObjects[o];
//
//		if (auto btBody = dynamic_cast<btRigidBody*>(object)) {
//			auto body = static_cast<PhysicsBody*>(btBody->getUserPointer());
//			bodies->push_back(body);
//		}
//	}
//
//	return bodies;
//}

float BulletWorldProxy::gravity() const {
	return _btWorld->getGravity().y();
}

void BulletWorldProxy::gravity(float gravity) {
	_btWorld->setGravity({0, gravity, 0});
}

//void BulletWorldProxy::update(Stats& stats) {
//
//	auto collisionObjects = _btWorld->getCollisionObjectArray();
//	for (int o=0; o<_btWorld->getNumCollisionObjects(); ++o) {
//		auto object = collisionObjects[o];
//
//		if (auto btBody = dynamic_cast<btRigidBody*>(object)) {
//			auto body = static_cast<PhysicsBody*>(btBody->getUserPointer());
//
//			switch (body->type()) {
//
//				case PHYSICS_BODY_TYPE::DYNAMIC:
//					++stats.dynamicBodies;
//					break;
//
//				case PHYSICS_BODY_TYPE::KINEMATIC: {
//					++stats.kinematicBodies;
//
//					auto worldTransform = body->node()->worldTransform();
//					auto toTransform = BTTransformFromGLMMat4(worldTransform);
//
//					btBody->setWorldTransform(toTransform);
//
//					// works
////					auto motionState = btBody->getMotionState();
////					motionState->setWorldTransform(toTransform); // and this kinematic...
////					btBody->setMotionState(motionState);
////					btBody->setActivationState(ACTIVE_TAG);
//					break; }
//
//				case PHYSICS_BODY_TYPE::STATIC:
//					++stats.staticBodies;
//					break;
//			}
//		}
//	}
//}

void BulletWorldProxy::step(double deltaT, float speed, float timestep) {

	auto result = _btWorld->stepSimulation(deltaT * speed,
										   MAX_SUBSTEPS,
										   timestep);

	if (result == MAX_SUBSTEPS) {
		AE_LOG_W("Max physics simulation substeps reached: {}", result);
	}
}

//void BulletWorldProxy::sync() {
//
//	auto collisionObjects = _btWorld->getCollisionObjectArray();
//	for (int o=0; o<_btWorld->getNumCollisionObjects(); ++o) {
//		auto object = collisionObjects[o];
//
//		if (auto btBody = dynamic_cast<btRigidBody*>(object)) {
//			auto body = static_cast<PhysicsBody*>(btBody->getUserPointer());
//
//			switch (body->type()) {
//
//				case PHYSICS_BODY_TYPE::DYNAMIC:
//					static btTransform btWorldTransform;
//					btBody->getMotionState()->getWorldTransform(btWorldTransform);
//					body->node()->applyPhysicsTransform(GLMMat4FromBTTransform(btWorldTransform));
//					break;
//
//				case PHYSICS_BODY_TYPE::KINEMATIC:
//				case PHYSICS_BODY_TYPE::STATIC:
//					break;
//			}
//		}
//	}
//}

void BulletWorldProxy::updateCollisionPairs() {
	_btWorld->getCollisionWorld()->computeOverlappingPairs();
}

void BulletWorldProxy::drawDebug(Renderer &renderer,
								 const glm::mat4 &viewMat,
								 const glm::mat4 &projectionMat,
								 const DebugOptions &debugOptions) {

#ifdef OPENGL_DESKTOP
	auto btDebugModes = BTDebugDrawModesForAEDebugOptions(debugOptions);

	_btDebugDrawer->setDebugMode(btDebugModes);
	_btDebugDrawer->clear();
	_btWorld->debugDrawWorld();
	_btDebugDrawer->draw(renderer, viewMat, projectionMat);
#endif
}

/*********************************************************************************************
	Internal
 *********************************************************************************************/

btDiscreteDynamicsWorld* BulletWorldProxy::btWorld() const {
	return _btWorld.get();
}

#ifdef DESKTOP
BulletDebugDrawer* BulletWorldProxy::btDebugDrawer() const {
	return _btDebugDrawer.get();
}
#endif

/*********************************************************************************************
	Static
 *********************************************************************************************/

btIDebugDraw::DebugDrawModes BTDebugDrawModesForAEDebugOptions(const DebugOptions& options) {
	btIDebugDraw::DebugDrawModes btModes = btIDebugDraw::DBG_NoDebug;

//	auto opt2 = DebugOptions::ShowPhysicsBoundingBoxes | DebugOptions::ShowPhysicsContactPoints;

	if ((options & DebugOptions::ShowPhysicsBoundingBoxes) != DebugOptions::None) {
		btModes = (btIDebugDraw::DebugDrawModes)(btModes | btIDebugDraw::DBG_DrawAabb);
	}
	if ((options & DebugOptions::ShowPhysicsWireframes) != DebugOptions::None) {
		btModes = (btIDebugDraw::DebugDrawModes)(btModes | btIDebugDraw::DBG_DrawWireframe);
	}
	if ((options & DebugOptions::ShowPhysicsContactPoints) != DebugOptions::None) {
		btModes = (btIDebugDraw::DebugDrawModes)(btModes | btIDebugDraw::DBG_DrawContactPoints);
	}
	if ((options & DebugOptions::ShowPhysicsNormals) != DebugOptions::None) {
		btModes = (btIDebugDraw::DebugDrawModes)(btModes | btIDebugDraw::DBG_DrawNormals);
	}
	if ((options & DebugOptions::ShowPhysicsConstraints) != DebugOptions::None) {
		btModes = (btIDebugDraw::DebugDrawModes)(btModes | btIDebugDraw::DBG_DrawConstraints);
	}
	if ((options & DebugOptions::ShowPhysicsConstraintLimits) != DebugOptions::None) {
		btModes = (btIDebugDraw::DebugDrawModes)(btModes | btIDebugDraw::DBG_DrawConstraintLimits);
	}

//	if (DEBUG_OPTIONS_CONTAINS(options, DebugOptions::ShowPhysicsBoundingBoxes)) {
//		btModes = (btIDebugDraw::DebugDrawModes)(btModes | btIDebugDraw::DBG_DrawAabb);
//	}
//	if (DEBUG_OPTIONS_CONTAINS(options, DebugOptions::ShowPhysicsWireframes)) {
//		btModes = (btIDebugDraw::DebugDrawModes)(btModes | btIDebugDraw::DBG_DrawWireframe);
//	}
//	if (DEBUG_OPTIONS_CONTAINS(options, DebugOptions::ShowPhysicsContactPoints)) {
//		btModes = (btIDebugDraw::DebugDrawModes)(btModes | btIDebugDraw::DBG_DrawContactPoints);
//	}
//	if (DEBUG_OPTIONS_CONTAINS(options, DebugOptions::ShowPhysicsNormals)) {
//		btModes = (btIDebugDraw::DebugDrawModes)(btModes | btIDebugDraw::DBG_DrawNormals);
//	}
//	if (DEBUG_OPTIONS_CONTAINS(options, DebugOptions::ShowPhysicsConstraints)) {
//		btModes = (btIDebugDraw::DebugDrawModes)(btModes | btIDebugDraw::DBG_DrawConstraints);
//	}
//	if (DEBUG_OPTIONS_CONTAINS(options, DebugOptions::ShowPhysicsConstraintLimits)) {
//		btModes = (btIDebugDraw::DebugDrawModes)(btModes | btIDebugDraw::DBG_DrawConstraintLimits);
//	}

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
		AE_LOG_D("Bullet debug modes: {}", magic_enum::enum_name(btModes));
	}
	previousModes = btModes;

	return btModes;
}
