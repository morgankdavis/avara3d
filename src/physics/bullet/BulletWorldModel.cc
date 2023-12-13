//
// Created by mkd on 12/8/23.
//

#include "physics/bullet/BulletWorldModel.h"

#include "btBulletCollisionCommon.h"
#include "btBulletDynamicsCommon.h"
#include "BulletCollision/Gimpact/btGImpactShape.h"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/matrix_decompose.hpp"
#include "LinearMath/btIDebugDraw.h"
#include "magic_enum.hpp"

#include "diagnostic/logging/Logger.h"
#include "physics/PhysicsBody.h"
#include "physics/bullet/BulletBodyModel.h"
#include "physics/bullet/BulletDebugDrawer.h"
#include "physics/bullet/BulletPhysicsSimulator.h"
#include "scene/Node.h"
#include "utilities/Utilities.h"


using namespace ae;
using namespace ae::utils;
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
		PhysicalWorldModel(world)/*:
		_collisionConfiguration(make_unique<btDefaultCollisionConfiguration>()),
		_collisionDispatcher(make_unique<btCollisionDispatcher>(_collisionConfiguration.get())),
		_broadphase(make_unique<btDbvtBroadphase>()),
		_constraintSolver(make_unique<btSequentialImpulseConstraintSolver>()),
		_world(make_unique<btDiscreteDynamicsWorld>(_collisionDispatcher.get(),
													_broadphase.get(),
													_constraintSolver.get(),
													_collisionConfiguration.get()))*/ {

	// putting this in the initializer list causes a SEGFAULT at btDiscreteDynamicsWorld::addRigidBody(). (?)

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
	AE_LOG_D("Destroying BulletWorldModel {:p}", (void*)this);
}

/*********************************************************************************************
	PhysicalWorldModel
 *********************************************************************************************/

void BulletWorldModel::add(PhysicsBody& body) {
	auto bodyModel = static_cast<BulletBodyModel*>(body.model());
	_btWorld->addRigidBody(bodyModel->btBody().get());
}

void BulletWorldModel::remove(PhysicsBody& body) {
	auto bodyModel = static_cast<BulletBodyModel*>(body.model());
	_btWorld->removeRigidBody(bodyModel->btBody().get());
}

float BulletWorldModel::gravity() const {
	return _btWorld->getGravity().y();
}

void BulletWorldModel::gravity(float gravity) {
	_btWorld->setGravity({0, gravity, 0});
}

void BulletWorldModel::update(Stats& stats) {

	auto collisionObjects = _btWorld->getCollisionObjectArray();
	for (int o=0; o<_btWorld->getNumCollisionObjects(); ++o) {
		auto object = collisionObjects[o];

		if (auto btBody = dynamic_cast<btRigidBody*>(object)) {
			auto body = static_cast<PhysicsBody*>(btBody->getUserPointer());

			switch (body->type()) {

				case PHYSICS_BODY_TYPE::DYNAMIC:
					++stats.dynamicBodies;
					break;

				case PHYSICS_BODY_TYPE::KINEMATIC: {
					++stats.kinematicBodies;

					auto worldTransform = body->node()->worldTransform();
					auto toTransform = BTTransformFromGLMMat4(worldTransform);
					//	btBody->proceedToTransform(toTransform); // this appears to affect dynamic bodies
					auto motionState = btBody->getMotionState();
					motionState->setWorldTransform(toTransform); // and this kinematic...
					btBody->setMotionState(motionState);
					btBody->setActivationState(ACTIVE_TAG);
					break; }

				case PHYSICS_BODY_TYPE::STATIC:
					++stats.staticBodies;
					break;
			}
		}
	}
}

void BulletWorldModel::step(double deltaT, float speed, float timestep) {

	auto result = _btWorld->stepSimulation(deltaT * speed,
										   MAX_SUBSTEPS,
										   timestep);

	if (result == MAX_SUBSTEPS) {
		AE_LOG_W("Physics simulation max substeps reached: {}", result);
	}
}

void BulletWorldModel::sync() {

	auto collisionObjects = _btWorld->getCollisionObjectArray();
	for (int o=0; o<_btWorld->getNumCollisionObjects(); ++o) {
		auto object = collisionObjects[o];

		if (auto btBody = dynamic_cast<btRigidBody*>(object)) {
			auto body = static_cast<PhysicsBody*>(btBody->getUserPointer());

			switch (body->type()) {

				case PHYSICS_BODY_TYPE::DYNAMIC:
				case PHYSICS_BODY_TYPE::KINEMATIC:
					static btTransform btWorldTransform;
					//btWorldTransform.setIdentity();
					//btMotionState->getWorldTransform(btWorldTransform); // crash?
					btBody->getMotionState()->getWorldTransform(btWorldTransform);
					body->node()->applyPhysicsTransform(GLMMat4FromBTTransform(btWorldTransform));
					break;

				case PHYSICS_BODY_TYPE::STATIC:
					break;
			}
		}
	}
}

void BulletWorldModel::drawDebug(Renderer &renderer,
								 const glm::mat4 &viewMat,
								 const glm::mat4 &projectionMat,
								 const DEBUG_OPTIONS &debugOptions) {

#ifdef OPENGL_CORE
//	auto resources = static_cast<BulletWorldModel*>(world.model());
//	auto btWorld = resources->btWorld();
//	auto debugDrawer = resources->btDebugDrawer();

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

//btDefaultCollisionConfiguration* BulletWorldModel::collisionConfiguration() const {
//	return _btCollisionConfiguration.get();
//}
//
//btCollisionDispatcher* BulletWorldModel::collisionDispatcher() const {
//	return _collisionDispatcher.get();
//}
//
//btDbvtBroadphase* BulletWorldModel::broadphase() const {
//	return _broadphase.get();
//}
//
//btSequentialImpulseConstraintSolver* BulletWorldModel::constraintSolver() const {
//	return _constraintSolver.get();
//}

#ifdef DESKTOP
BulletDebugDrawer* BulletWorldModel::btDebugDrawer() const {
	return _btDebugDrawer.get();
}
#endif

/*********************************************************************************************
	Public Static
 *********************************************************************************************/

vec3 BulletWorldModel::GLMVec3FromBTVector3(const btVector3& from) {
	return vec3(from.x(), from.y(), from.z());
}

vec4 BulletWorldModel::GLMVec4FromBTVector4(const btVector4& from) {
	return vec4(from.x(), from.y(), from.z(), from.w());
}

mat4 BulletWorldModel::GLMMat4FromBTTransform(const btTransform& from) {
	mat4 glmMat;
	from.getOpenGLMatrix(value_ptr(glmMat));
	return glmMat;
}

btVector3 BulletWorldModel::BTVector3FromGLMVec3(const vec3& from) {
	return btVector3(from.x, from.y, from.z);
}

btVector4 BulletWorldModel::BTVector4FromGLMVec4(const vec4& from) {
	return btVector4(from.x, from.y, from.z, from.w);
}

btQuaternion BulletWorldModel::BTQuaternionFromGLMQuat(const quat& from) {

	return btQuaternion(from.x, from.y, from.z, from.w);

}

btTransform BulletWorldModel::BTTransformFromGLMMat4(const mat4& from) {

	// this version (probably) does not strip scale & sheer

	btTransform bulletTransform;
	bulletTransform.setIdentity();
	bulletTransform.setFromOpenGLMatrix(value_ptr(from));
	return bulletTransform;


//	// THIS VERSION STRIPS (hopefully!) scale & sheer
//
//	btTransform bulletTransform;
//	bulletTransform.setIdentity();
//
//	vec3 scale;
//	quat orientation;
//	vec3 translation;
//	vec3 skew;
//	vec4 perspective;
//
//	decompose(from,
//			  scale,
//			  orientation,
//			  translation,
//			  skew,
//			  perspective);
//
//	bulletTransform.setOrigin(BTVector3FromGLMVec3(translation));
//	bulletTransform.setRotation(BTQuaternionFromGLMQuat(orientation));
//
//	return bulletTransform;
}

mat4 BulletWorldModel::TransformByRemovingScale(const mat4& m, bool& scaled) {
	// TODO: optimize

	vec3 scale;
	quat orientation;
	vec3 translation;
	vec3 skew;
	vec4 perspective;

	decompose(m,
			  scale,
			  orientation,
			  translation,
			  skew,
			  perspective);

	scaled = !Equal(scale, {1, 1, 1});
	if (scaled) return translate(mat4(1.0), translation) * mat4_cast(orientation) * mat4(1.0);
	else return m;
}

btTransform& BulletWorldModel::BTIdentityTransform() {
	// TODO: optimize
	static auto identityTransform = btTransform();
	identityTransform.setIdentity();
	return identityTransform;
}

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
