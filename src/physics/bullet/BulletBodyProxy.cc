//
//  BulletBodyProxy.cc
//  avara3d
//
//  Created by Morgan Davis on 10/29/23.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#include "a3d/physics/bullet/BulletBodyProxy.h"

#include "btBulletDynamicsCommon.h"
#include "glm/gtc/type_ptr.hpp"

#include "a3d/diagnostic/logging/Logger.h"
#include "a3d/mesh/Mesh.h"
#include "a3d/physics/ConvexDecomposer.h"
#include "a3d/physics/PhysicsBody.h"
#include "a3d/physics/PhysicsShape.h"
#include "a3d/physics/PhysicalWorld.h"
#include "a3d/physics/bullet/BulletShapeProxy.h"
#include "a3d/physics/bullet/BulletWorldProxy.h"
#include "a3d/physics/bullet/BulletMotionState.h"
#include "a3d/physics/bullet/BulletUtilities.h"
#include "a3d/physics/proxy/PhysicsBodyProxy.h"
#include "a3d/physics/proxy/PhysicsShapeProxy.h"
#include "a3d/scene/Node.h"
#include "a3d/scene/Scene.h"


using namespace a3d;
using namespace glm;
using namespace std;


/*********************************************************************************************
	Internal Lifecycle
 *********************************************************************************************/

BulletBodyProxy::BulletBodyProxy(PhysicsBody& body, PhysicsBodyType type):
		PhysicsBodyProxy{body, type},
		_btBody{},
		/*_btMotionState(nullptr)*/
		_motionState{} {

	A3D_LOG_D("body: {:p}", static_cast<void*>(&body));

	// make a "shell" of a body and modify its properties as they are set
	// https://pybullet.org/Bullet/phpBB3/viewtopic.php?p=43923&sid=187e552b028cd64fe2e831df414d382a#p43923

//	_btMotionState = make_shared<btDefaultMotionState>(btTransform::getIdentity());
//	_motionState = make_shared<MotionState>(body, btTransform::getIdentity());
	_motionState = make_unique<BulletMotionState>(body);

	// it seems as though adding a body to the world with mass=0 forever casts it
	// as a static body. adding it, setting it to 0, the setting it to something
	// different seems to work fine, though.
	btRigidBody::btRigidBodyConstructionInfo rigidBodyInfo((/*body.type()*/type == PhysicsBodyType::Static
															? 0.0f
															: 1.0f), // important!
														   _motionState.get(),
														   nullptr);
	rigidBodyInfo.m_friction = 0.5;
	rigidBodyInfo.m_rollingFriction = 0.05; // don't roll forever
	rigidBodyInfo.m_spinningFriction = 0.05; // don't spin forever
	rigidBodyInfo.m_restitution = 0.0;
	rigidBodyInfo.m_linearSleepingThreshold = 1.0;
	rigidBodyInfo.m_angularSleepingThreshold = 1.0;

	_btBody = make_unique<btRigidBody>(rigidBodyInfo);

	int flags = 0;
	int activationState = _btBody->getActivationState();

	switch (/*body.type()*/type) {
		case PhysicsBodyType::Static:
			flags = btCollisionObject::CF_STATIC_OBJECT;
			activationState = activationState & ~DISABLE_DEACTIVATION;
			break;
		case PhysicsBodyType::Dynamic:
			flags = btCollisionObject::CF_DYNAMIC_OBJECT;
			activationState = activationState & ~DISABLE_DEACTIVATION;
			break;
		case PhysicsBodyType::Kinematic:
			flags = btCollisionObject::CF_KINEMATIC_OBJECT;
			activationState = activationState | DISABLE_DEACTIVATION;
			break;
	}

	_btBody->setCollisionFlags(flags);
	_btBody->setActivationState(activationState);

	_btBody->setUserPointer(static_cast<void*>(&body));
}

BulletBodyProxy::~BulletBodyProxy() {
	A3D_LOG_D("Destroying BulletBodyProxy {:p}", static_cast<void*>(this));
}

/*********************************************************************************************
	PhysicsBodyModelProxy Internal Members
 *********************************************************************************************/

PhysicsBodyType BulletBodyProxy::type() const {

	auto flags = _btBody->getCollisionFlags();

	// btCollisionObject::CF_DYNAMIC_OBJECT (0) will never bitwise AND with anything
	//	if (flags & btCollisionObject::CF_DYNAMIC_OBJECT) return PhysicsBodyType::Dynamic;
	if (flags == btCollisionObject::CF_DYNAMIC_OBJECT) return PhysicsBodyType::Dynamic;
	if (flags & btCollisionObject::CF_STATIC_OBJECT) return PhysicsBodyType::Static;
	if (flags & btCollisionObject::CF_KINEMATIC_OBJECT) return PhysicsBodyType::Kinematic;

	return PhysicsBodyType::Static;
}

void BulletBodyProxy::type(PhysicsBodyType type) {

	int flags = 0;
	int activationState = _btBody->getActivationState();

	switch (type) {
		case PhysicsBodyType::Static:
			flags = btCollisionObject::CF_STATIC_OBJECT;
			activationState = activationState & ~DISABLE_DEACTIVATION;
			break;
		case PhysicsBodyType::Dynamic:
			flags = btCollisionObject::CF_DYNAMIC_OBJECT;
			if (_autocalculatesMomentOfInertia) {
				calculateMomentOfIntertia();
			}
			activationState = activationState & ~DISABLE_DEACTIVATION;
			break;
		case PhysicsBodyType::Kinematic:
			flags = btCollisionObject::CF_KINEMATIC_OBJECT;
			auto as = _btBody->getActivationState();
			activationState = activationState | DISABLE_DEACTIVATION;
			break;
	}

	_btBody->setCollisionFlags(flags);
	_btBody->setActivationState(activationState);
}

PhysicsShapeProxy* BulletBodyProxy::shapeProxy() const {
	return _shapeProxy;
}

void BulletBodyProxy::shapeProxy(PhysicsShapeProxy* proxy) {
	A3D_LOG_T("proxy: {:p}", static_cast<void*>(proxy));

	if (proxy) {
		// front is either the only btCollisionShape or a btCompound shape with child shapes at index 1+

		if (auto btShape = dynamic_cast<BulletShapeProxy*>(proxy)->btShapes().front().get()) {

			_btBody->setCollisionShape(btShape);

			switch (_body->type()) {
				case PhysicsBodyType::Static:
				case PhysicsBodyType::Kinematic:
					this->mass(0);
					break;
				case PhysicsBodyType::Dynamic:
					break;
			}

			_shapeProxy = proxy;

			if (this->type() == PhysicsBodyType::Dynamic && _autocalculatesMomentOfInertia) {
				calculateMomentOfIntertia();
			}
		}
		else {
			A3D_LOG_E("Could not get shape resources.");
			_shapeProxy = nullptr;
		}
	}
	else {
		_shapeProxy = nullptr;
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

	if (type() == PhysicsBodyType::Dynamic && _autocalculatesMomentOfInertia) {
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
		A3D_LOG_W("Ignoring moment of inertia: autocalculatesMomentOfInertia to to true.");
	}
}

glm::vec3 BulletBodyProxy::centerOfMass() const {
	return GLMVec3FromBTVector3(_btBody->getCenterOfMassPosition());
}

void BulletBodyProxy::centerOfMass(const glm::vec3& offset) {
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
	//return (_btBody->getActivationState() != DISABLE_DEACTIVATION);
	return !(_btBody->getActivationState() & DISABLE_DEACTIVATION);
}

void BulletBodyProxy::allowsResting(bool allowsResting) {

	// TODO: test this
	if (allowsResting && type() == PhysicsBodyType::Kinematic) {

		A3D_LOG_E("Cannot enable resting for kinematic bodies.");
	}
	else {

		int activationState = _btBody->getActivationState();

		activationState = (allowsResting
						   ? activationState & ~DISABLE_DEACTIVATION
						   : activationState | DISABLE_DEACTIVATION);

		_btBody->setActivationState(activationState);
	}
}

bool BulletBodyProxy::resting() const {
	//return (_btBody->getActivationState() == ISLAND_SLEEPING);
	return (_btBody->getActivationState() & ISLAND_SLEEPING);
}

void BulletBodyProxy::resting(bool resting) {

	int activationState = _btBody->getActivationState();

	activationState = (resting
			? activationState | ISLAND_SLEEPING
			: activationState | ACTIVE_TAG);

	_btBody->setActivationState(activationState);
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
//		A3D_LOG_W("Ignorning scale for Node {:p} with PhysicsBody {:p}.",
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
//		A3D_LOG_W("Ignorning scale for Node {:p} with PhysicsBody {:p}.",
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
	Internal Members
 *********************************************************************************************/

btRigidBody* BulletBodyProxy::btBody() {
	return _btBody.get();
}

/*********************************************************************************************
	Private Members
 *********************************************************************************************/

void BulletBodyProxy::calculateMomentOfIntertia() {

	if (auto btShapeModel = dynamic_cast<BulletShapeProxy*>(_shapeProxy)) {
		if (auto btShape = btShapeModel->btShapes().front().get()) {
			btVector3 localInertia;
			auto mass = _body->mass();
			btShape->calculateLocalInertia(mass, localInertia);
			_btBody->setMassProps(mass, localInertia);
			_btBody->updateInertiaTensor();
		}
		else {
			A3D_LOG_W("Missing btCollisionShape.");
		}
	}
//	else {
//		A3D_LOG_W("Missing PhysicsShapeModelProxy.");
//	}
}
