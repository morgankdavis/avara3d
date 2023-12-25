//
// Created by mkd on 10/29/23.
//

#include "physics/bullet/BulletBodyProxy.h"

#include "btBulletDynamicsCommon.h"
#include "BulletCollision/Gimpact/btGImpactShape.h"
#include "glm/gtc/type_ptr.hpp"
#include "LinearMath/btIDebugDraw.h"

#include "diagnostic/logging/Logger.h"
#include "geometry/Geometry.h"
#include "physics/ConvexDecomposer.h"
#include "physics/PhysicsBody.h"
#include "physics/model_proxy/PhysicsBodyModelProxy.h"
#include "physics/PhysicsShape.h"
#include "physics/model_proxy/PhysicsShapeModelProxy.h"
#include "physics/PhysicalWorld.h"
#include "physics/bullet/BulletShapeProxy.h"
#include "physics/bullet/BulletWorldProxy.h"
#include "physics/bullet/Utilities.h"
#include "scene/Node.h"
#include "scene/Scene.h"


using namespace ae;
using namespace glm;
using namespace std;


/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

BulletBodyProxy::BulletBodyProxy(PhysicsBody* body):
		PhysicsBodyModelProxy(body),
		_btBody(nullptr),
		/*_btMotionState(nullptr)*/
		_motionState(nullptr) {

	AE_LOG_D("body: {:p}", static_cast<void*>(body));

	// make a "shell" of a body and modify its properties as they are set
	// https://pybullet.org/Bullet/phpBB3/viewtopic.php?p=43923&sid=187e552b028cd64fe2e831df414d382a#p43923

//	_btMotionState = make_shared<btDefaultMotionState>(btTransform::getIdentity());
//	_motionState = make_shared<MotionState>(body, btTransform::getIdentity());
	_motionState = make_shared<MotionState>(body);
	btRigidBody::btRigidBodyConstructionInfo rigidBodyInfo(0,
														   _motionState.get(),
														   nullptr);

	_btBody = make_shared<btRigidBody>(rigidBodyInfo);

	switch (body->type()) {
		case PHYSICS_BODY_TYPE::STATIC:
			_btBody->setCollisionFlags(btCollisionObject::CF_STATIC_OBJECT);
			break;
		case PHYSICS_BODY_TYPE::DYNAMIC:
			_btBody->setCollisionFlags(btCollisionObject::CF_DYNAMIC_OBJECT);
			break;
		case PHYSICS_BODY_TYPE::KINEMATIC:
			_btBody->setCollisionFlags(btCollisionObject::CF_KINEMATIC_OBJECT);
			_btBody->setActivationState(DISABLE_DEACTIVATION);
			break;
	}

	_btBody->setUserPointer(static_cast<void*>(body));
}

BulletBodyProxy::~BulletBodyProxy() {
	AE_LOG_D("Destroying BulletBodyProxy {:p}", static_cast<void*>(this));
}

/*********************************************************************************************
	PhysicsBodyModelProxy
 *********************************************************************************************/

PHYSICS_BODY_TYPE BulletBodyProxy::type() const {

	auto flags = _btBody->getCollisionFlags();

	if (flags & btCollisionObject::CF_STATIC_OBJECT) return PHYSICS_BODY_TYPE::STATIC;
	if (flags & btCollisionObject::CF_DYNAMIC_OBJECT) return PHYSICS_BODY_TYPE::DYNAMIC;
	if (flags & btCollisionObject::CF_KINEMATIC_OBJECT) return PHYSICS_BODY_TYPE::KINEMATIC;

	return PHYSICS_BODY_TYPE::STATIC;
}

void BulletBodyProxy::type(PHYSICS_BODY_TYPE type) {

	int flags = 0;
	switch (type) {
		case PHYSICS_BODY_TYPE::STATIC:
			flags = btCollisionObject::CF_STATIC_OBJECT;
			break;
		case PHYSICS_BODY_TYPE::DYNAMIC:
			flags = btCollisionObject::CF_DYNAMIC_OBJECT;
			if (_autocalculatesMomentOfInertia) {
				calculateMomentOfIntertia();
			}
			break;
		case PHYSICS_BODY_TYPE::KINEMATIC:
			flags = btCollisionObject::CF_KINEMATIC_OBJECT;
			_btBody->setActivationState(DISABLE_DEACTIVATION);
			break;
	}
	_btBody->setCollisionFlags(flags);
}

PhysicsShapeModelProxy* BulletBodyProxy::shapeModel() const {
	return _shapeModel;
}

void BulletBodyProxy::shapeModel(PhysicsShapeModelProxy* shape) {
	AE_LOG_T("shape: {:p}", static_cast<void*>(shape));

	if (shape) {
		// front is either the only btCollisionShape or a btCompound shape with child shapes at index 1+
		if (auto btShape = dynamic_cast<BulletShapeProxy *>(shape)->btShapes().front()) {

			_btBody->setCollisionShape(btShape.get());

			auto mass = BulletBodyProxy::mass();
			switch (_body->type()) {
				case PHYSICS_BODY_TYPE::STATIC:
				case PHYSICS_BODY_TYPE::KINEMATIC:
					mass = 0;
					break;
				case PHYSICS_BODY_TYPE::DYNAMIC:
					break;
			}

			_shapeModel = shape;

			if (_autocalculatesMomentOfInertia) {
				calculateMomentOfIntertia();
			}
		}
		else {
			AE_LOG_E("Could not get shape resources.");
			_shapeModel = nullptr;
		}
	}
	else {
		_shapeModel = nullptr;
	}
}

float BulletBodyProxy::mass() const {

	auto invMass = _btBody->getInvMass();
	return (invMass != 0
			? 1.0f/_btBody->getInvMass()
			: 0);
}

void BulletBodyProxy::mass(float mass) {

	_btBody->setMassProps(mass, _btBody->getLocalInertia());

	if (_autocalculatesMomentOfInertia) {
		calculateMomentOfIntertia();
	}

	_btBody->setActivationState(ACTIVE_TAG);
}

glm::vec3 BulletBodyProxy::momentOfInertia() const {
	return GLMVec3FromBTVector3(_btBody->getLocalInertia());
}

void BulletBodyProxy::momentOfInertia(const glm::vec3& moment) {

	if (!_autocalculatesMomentOfInertia) {
		_btBody->setMassProps(mass(),
							  BTVector3FromGLMVec3(moment));
		_btBody->updateInertiaTensor();
	}
	else {
		AE_LOG_W("Ignoring moment of inertia: autocalculatesMomentOfInertia to to true.");
	}
}

glm::vec3 BulletBodyProxy::centerOfMass() const {
	return GLMVec3FromBTVector3(_btBody->getCenterOfMassPosition());
}

void BulletBodyProxy::centerOfMass(const glm::vec3 offset) {
	_btBody->setCenterOfMassTransform(
			BTTransformFromGLMMat4(translate(mat4(1.0), offset)));
}

float BulletBodyProxy::friction() const {
	return _btBody->getFriction();
}

void BulletBodyProxy::friction(float friction) {
	_btBody->setFriction(friction);
}

float BulletBodyProxy::rollingFriction() const {
	return _btBody->getRollingFriction();
}

void BulletBodyProxy::rollingFriction(float friction) {
	_btBody->setRollingFriction(friction);
}

float BulletBodyProxy::restitution() const {
	return _btBody->getRestitution();
}

void BulletBodyProxy::restitution(float restitution) {
	_btBody->setRestitution(restitution);
}

glm::vec3 BulletBodyProxy::linearVelocity() const {
	return GLMVec3FromBTVector3(_btBody->getLinearVelocity());
}

void BulletBodyProxy::linearVelocity(const glm::vec3& velocity) {
	_btBody->setLinearVelocity(BTVector3FromGLMVec3(velocity));
}

glm::vec3 BulletBodyProxy::angularVelocity() const {
	return GLMVec3FromBTVector3(_btBody->getAngularVelocity());
}

void BulletBodyProxy::angularVelocity(const glm::vec3& velocity) {
	_btBody->setAngularVelocity(BTVector3FromGLMVec3(velocity));
}

glm::vec3 BulletBodyProxy::linearFactor() const {
	return GLMVec3FromBTVector3(_btBody->getLinearFactor());
}

void BulletBodyProxy::linearFactor(const glm::vec3& factor) {
	_btBody->setLinearFactor(BTVector3FromGLMVec3(factor));
}

glm::vec3 BulletBodyProxy::angularFactor() const {
	return GLMVec3FromBTVector3(_btBody->getAngularFactor());
}

void BulletBodyProxy::angularFactor(const glm::vec3& factor) {
	_btBody->setAngularFactor(BTVector3FromGLMVec3(factor));
}

float BulletBodyProxy::linearDamping() const {
	return _btBody->getLinearDamping();
}

void BulletBodyProxy::linearDamping(float damping) {
	_btBody->setDamping(damping, _btBody->getAngularDamping());
}

float BulletBodyProxy::angularDamping() const {
	return _btBody->getAngularDamping();
}

void BulletBodyProxy::angularDamping(float damping) {
	_btBody->setDamping(_btBody->getLinearDamping(), damping);
}

float BulletBodyProxy::linearSleepingThreshold() const {
	return _btBody->getLinearSleepingThreshold();
}

void BulletBodyProxy::linearSleepingThreshold(float threshold) {
	_btBody->setSleepingThresholds(threshold, _btBody->getAngularSleepingThreshold());
}

float BulletBodyProxy::angularSleepingThreshold() const {
	return _btBody->getAngularSleepingThreshold();
}

void BulletBodyProxy::angularSleepingThreshold(float threshold) {
	_btBody->setSleepingThresholds(_btBody->getLinearSleepingThreshold(), threshold);
}

void BulletBodyProxy::applyForce(const vec3& force, const vec3& location) {
	_btBody->applyForce(BTVector3FromGLMVec3(force),
						BTVector3FromGLMVec3(location));
}

void BulletBodyProxy::applyCentralForce(const vec3& force) {
	_btBody->applyCentralForce(BTVector3FromGLMVec3(force));
}

void BulletBodyProxy::applyImpulse(const vec3& impulse, const vec3& location) {
	_btBody->applyImpulse(BTVector3FromGLMVec3(impulse),
						BTVector3FromGLMVec3(location));
}

void BulletBodyProxy::applyCentralImpulse(const vec3& impulse) {
	_btBody->applyCentralImpulse(BTVector3FromGLMVec3(impulse));
}

void BulletBodyProxy::applyTorque(const vec3& torque) {
	_btBody->applyTorque(BTVector3FromGLMVec3(torque));
}

void BulletBodyProxy::applyTorqueImpulse(const vec3& torque) {
	_btBody->applyTorqueImpulse(BTVector3FromGLMVec3(torque));
}

bool BulletBodyProxy::affectedByGravity() const {
	// *** test this ***
	auto gravity = _btBody->getGravity();
	return (gravity.x() != 0)
		   || (gravity.y() != 0)
		   || (gravity.z() != 0);
}

vec3 BulletBodyProxy::totalForce() const {
	return GLMVec3FromBTVector3(_btBody->getTotalForce());
}

vec3 BulletBodyProxy::totalTorque() const {
	return GLMVec3FromBTVector3(_btBody->getTotalTorque());
}

void BulletBodyProxy::affectedByGravity(bool affectedByGravity) {
	// *** test this ***
	_btBody->setGravity(affectedByGravity
						? btVector3{1.0, 1.0, 1.0}
						: btVector3{0, 0, 0});
}

bool BulletBodyProxy::allowsResting() const {
	// *** test this ***
	return (_btBody->getActivationState() != DISABLE_DEACTIVATION);
}

void BulletBodyProxy::allowsResting(bool allowsResting) {
	// *** test this ***
	if (allowsResting
	&& type() == PHYSICS_BODY_TYPE::KINEMATIC) {
		AE_LOG_E("Cannot enable resting for kinematic bodies.");
	}
	else {
		_btBody->setActivationState(allowsResting
									? ACTIVE_TAG
									: DISABLE_DEACTIVATION);
	}
}

bool BulletBodyProxy::resting() const {
	return (_btBody->getActivationState() == ISLAND_SLEEPING);
}

void BulletBodyProxy::resting(bool resting) {
	_btBody->setActivationState(resting
								? ISLAND_SLEEPING
								: ACTIVE_TAG);
}

//glm::mat4 BulletBodyProxy::worldTransform() const {
//
//	static btTransform transform;
//	_btBody->getMotionState()->getWorldTransform(transform);
//	return GLMMat4FromBTTransform(transform);
//}
//
//void BulletBodyProxy::worldTransform(const glm::mat4& transform) {
//
//	bool wasScaled = false;
//	auto btTransform = BTTransformFromGLMMat4(
//			TransformByRemovingScale(_body->node()->worldTransform(), wasScaled));
//
//	if (wasScaled) {
//		// TODO: do something about this
//		AE_LOG_W("Ignorning scale for Node {:p} with PhysicsBody {:p}.",
//				 (void *)_body->node(), (void *)_body);
//	}
//
////	_btMotionState = make_shared<btDefaultMotionState>(btTransform);
////	_btBody->setMotionState(_btMotionState.get());
//
//	_motionState = make_shared<MotionState>(_body, btTransform);
//	_btBody->setMotionState(_motionState.get());
//}

void BulletBodyProxy::worldTransform(const glm::mat4& transform) {

//	bool wasScaled = false;
//	auto btTransform = BTTransformFromGLMMat4(
//			TransformByRemovingScale(_body->node()->worldTransform(), wasScaled));
//
//	if (wasScaled) {
//		// TODO: do something about this
//		AE_LOG_W("Ignorning scale for Node {:p} with PhysicsBody {:p}.",
//				 (void *)_body->node(), (void *)_body);
//	}
//
////	_btMotionState = make_shared<btDefaultMotionState>(btTransform);
////	_btBody->setMotionState(_btMotionState.get());
//
//	_motionState = make_shared<MotionState>(_body, btTransform);
//	_btBody->setMotionState(_motionState.get());


	_btBody->setWorldTransform(BTTransformFromGLMMat4(transform));

	//_motionState = make_shared<MotionState>(_body, btTransform);
	//_btBody->setMotionState(_motionState.get());
}

void BulletBodyProxy::clearForces() {
	_btBody->clearForces();
}

/*********************************************************************************************
	Internal
 *********************************************************************************************/

shared_ptr<btRigidBody> BulletBodyProxy::btBody() {
	return _btBody;
}

//shared_ptr<btDefaultMotionState> BulletBodyProxy::btMotionState() {
//	return _btMotionState;
//}

shared_ptr<MotionState> BulletBodyProxy::motionState() {
	return _motionState;
}

/*********************************************************************************************
	Private
 *********************************************************************************************/

void BulletBodyProxy::calculateMomentOfIntertia() {

	if (auto btShapeModel = dynamic_cast<BulletShapeProxy*>(_shapeModel)) {
		if (auto btShape = btShapeModel->btShapes().front()) {
			btVector3 localInertia;
			auto mass = _body->mass();
			btShape->calculateLocalInertia(mass, localInertia);
			_btBody->setMassProps(mass, localInertia);
			_btBody->updateInertiaTensor();
		}
		else {
			AE_LOG_W("Missing btCollisionShape.");
		}
	}
	else {
		AE_LOG_W("Missing PhysicsShapeModelProxy.");
	}
}
