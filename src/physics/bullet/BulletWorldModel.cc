//
// Created by mkd on 12/8/23.
//

#include "physics/bullet/BulletWorldModel.h"

#include "btBulletCollisionCommon.h"
#include "btBulletDynamicsCommon.h"
#include "BulletCollision/Gimpact/btGImpactShape.h"
#include "glm/gtc/type_ptr.hpp"
#include "LinearMath/btIDebugDraw.h"
#include "magic_enum.hpp"

#include "diagnostic/logging/Logger.h"
#include "physics/PhysicsBody.h"
#include "physics/bullet/BulletBodyModel.h"
#include "physics/bullet/BulletDebugDrawer.h"
#include "scene/Node.h"


using namespace ae;
using namespace glm;
using namespace std;


constexpr unsigned MAX_SUBSTEPS = 20; // move

/*********************************************************************************************
	Static Prototypes
 *********************************************************************************************/

static btIDebugDraw::DebugDrawModes BTDebugDrawModesForAEDebugOptions(const DEBUG_OPTIONS& options);

/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

BulletWorldModel::BulletWorldModel(PhysicalWorld* world):
		PhysicalWorldModel(world) {

	_btCollisionConfiguration = make_unique<btDefaultCollisionConfiguration>();
	_btCollisionDispatcher = make_unique<btCollisionDispatcher>(_btCollisionConfiguration.get());
	_btBroadphase = make_unique<btDbvtBroadphase>();
	_btConstraintSolver = make_unique<btSequentialImpulseConstraintSolver>();
	_btWorld = make_unique<btDiscreteDynamicsWorld>(_btCollisionDispatcher.get(),
												  _btBroadphase.get(),
												  _btConstraintSolver.get(),
												  _btCollisionConfiguration.get());

	AE_LOG_I("Bullet Physics version: {}",  btGetVersion());

#ifdef OPENGL_CORE
	_btDebugDrawer = make_unique<BulletDebugDrawer>();
	_btWorld->setDebugDrawer(_btDebugDrawer.get());
#endif
}

BulletWorldModel::~BulletWorldModel() {
	AE_LOG_D("Destroying BulletWorldModel {:p}", static_cast<void*>(this));
}

/*********************************************************************************************
	PhysicalWorldModel
 *********************************************************************************************/

void BulletWorldModel::add(PhysicsBody& body) {
	AE_LOG_D("body: {:p}", static_cast<void*>(&body));

	auto bodyModel = static_cast<BulletBodyModel*>(body.model());
	//bodyModel->btBody()->setWorldTransform(BTTransformFromGLMMat4(body.node()->worldTransform()));
	auto b = bodyModel->btBody().get();
	_btWorld->addRigidBody(b);
}

void BulletWorldModel::remove(PhysicsBody& body) {
	AE_LOG_D("body: {:p}", static_cast<void*>(&body));

	auto bodyModel = static_cast<BulletBodyModel*>(body.model());
	_btWorld->removeRigidBody(bodyModel->btBody().get());
}

float BulletWorldModel::gravity() const {
	return _btWorld->getGravity().y();
}

void BulletWorldModel::gravity(float gravity) {
	_btWorld->setGravity({0, gravity, 0});
}

//void BulletWorldModel::update(Stats& stats) {
//
////	auto collisionObjects = _btWorld->getCollisionObjectArray();
////	for (int o=0; o<_btWorld->getNumCollisionObjects(); ++o) {
////		auto object = collisionObjects[o];
////
////		if (auto btBody = dynamic_cast<btRigidBody*>(object)) {
////			auto body = static_cast<PhysicsBody*>(btBody->getUserPointer());
////
////			switch (body->type()) {
////
////				case PHYSICS_BODY_TYPE::DYNAMIC:
////					++stats.dynamicBodies;
////					break;
////
////				case PHYSICS_BODY_TYPE::KINEMATIC: {
////					++stats.kinematicBodies;
////
////					auto worldTransform = body->node()->worldTransform();
////					auto toTransform = BTTransformFromGLMMat4(worldTransform);
////
////					btBody->setWorldTransform(toTransform);
////
////					// works
//////					auto motionState = btBody->getMotionState();
//////					motionState->setWorldTransform(toTransform); // and this kinematic...
//////					btBody->setMotionState(motionState);
//////					btBody->setActivationState(ACTIVE_TAG);
////					break; }
////
////				case PHYSICS_BODY_TYPE::STATIC:
////					++stats.staticBodies;
////					break;
////			}
////		}
////	}
//}

void BulletWorldModel::step(double deltaT, float speed, float timestep) {

	auto result = _btWorld->stepSimulation(deltaT * speed,
										   MAX_SUBSTEPS,
										   timestep);

	if (result == MAX_SUBSTEPS) {
		AE_LOG_W("Max physics simulation substeps reached: {}", result);
	}
}

//void BulletWorldModel::sync() {
//
////	auto collisionObjects = _btWorld->getCollisionObjectArray();
////	for (int o=0; o<_btWorld->getNumCollisionObjects(); ++o) {
////		auto object = collisionObjects[o];
////
////		if (auto btBody = dynamic_cast<btRigidBody*>(object)) {
////			auto body = static_cast<PhysicsBody*>(btBody->getUserPointer());
////
////			switch (body->type()) {
////
////				case PHYSICS_BODY_TYPE::DYNAMIC:
////					static btTransform btWorldTransform;
////					btBody->getMotionState()->getWorldTransform(btWorldTransform);
////					body->node()->applyPhysicsTransform(GLMMat4FromBTTransform(btWorldTransform));
////					break;
////
////				case PHYSICS_BODY_TYPE::KINEMATIC:
////				case PHYSICS_BODY_TYPE::STATIC:
////					break;
////			}
////		}
////	}
//}

void BulletWorldModel::updateCollisionPairs() {
	_btWorld->getCollisionWorld()->computeOverlappingPairs();
}

void BulletWorldModel::drawDebug(Renderer &renderer,
								 const glm::mat4 &viewMat,
								 const glm::mat4 &projectionMat,
								 const DEBUG_OPTIONS &debugOptions) {

#ifdef OPENGL_CORE
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

btDiscreteDynamicsWorld* BulletWorldModel::btWorld() const {
	return _btWorld.get();
}

#ifdef DESKTOP
BulletDebugDrawer* BulletWorldModel::btDebugDrawer() const {
	return _btDebugDrawer.get();
}
#endif

/*********************************************************************************************
	Static
 *********************************************************************************************/

btIDebugDraw::DebugDrawModes BTDebugDrawModesForAEDebugOptions(const DEBUG_OPTIONS& options) {
	btIDebugDraw::DebugDrawModes btModes = btIDebugDraw::DBG_NoDebug;

	if (DEBUG_OPTIONS_CONTAINS(options, DEBUG_OPTIONS::SHOW_PHYSICS_BOUNDING_BOXES)) {
		btModes = (btIDebugDraw::DebugDrawModes)(btModes | btIDebugDraw::DBG_DrawAabb);
	}
	if (DEBUG_OPTIONS_CONTAINS(options, DEBUG_OPTIONS::SHOW_PHYSICS_WIREFRAMES)) {
		btModes = (btIDebugDraw::DebugDrawModes)(btModes | btIDebugDraw::DBG_DrawWireframe);
	}
	if (DEBUG_OPTIONS_CONTAINS(options, DEBUG_OPTIONS::SHOW_PHYSICS_CONTACT_POINTS)) {
		btModes = (btIDebugDraw::DebugDrawModes)(btModes | btIDebugDraw::DBG_DrawContactPoints);
	}
	if (DEBUG_OPTIONS_CONTAINS(options, DEBUG_OPTIONS::SHOW_PHYSICS_NORMALS)) {
		btModes = (btIDebugDraw::DebugDrawModes)(btModes | btIDebugDraw::DBG_DrawNormals);
	}
	if (DEBUG_OPTIONS_CONTAINS(options, DEBUG_OPTIONS::SHOW_PHYSICS_CONSTRAINTS)) {
		btModes = (btIDebugDraw::DebugDrawModes)(btModes | btIDebugDraw::DBG_DrawConstraints);
	}
	if (DEBUG_OPTIONS_CONTAINS(options, DEBUG_OPTIONS::SHOW_PHYSICS_CONSTRAINT_LIMITS)) {
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
		AE_LOG_D("Bullet debug modes: {}", magic_enum::enum_name(btModes));
	}
	previousModes = btModes;

	return btModes;
}
