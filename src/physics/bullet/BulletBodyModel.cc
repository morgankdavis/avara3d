//
// Created by mkd on 10/29/23.
//

#include "physics/bullet/BulletBodyModel.h"

#include "btBulletCollisionCommon.h"
#include "btBulletDynamicsCommon.h"
#include "BulletCollision/Gimpact/btGImpactShape.h"
#include "glm/gtc/type_ptr.hpp"
#include "LinearMath/btIDebugDraw.h"

#include "diagnostic/logging/Logger.h"
#include "geometry/Geometry.h"
#include "physics/ConvexDecomposer.h"
#include "physics/PhysicsBody.h"
#include "physics/PhysicsBodyModel.h"
#include "physics/PhysicsShape.h"
#include "physics/PhysicsShapeModel.h"
#include "physics/PhysicalWorld.h"
#include "physics/bullet/BulletShapeModel.h"
#include "physics/bullet/BulletWorldModel.h"
#include "physics/bullet/Utilities.h"
#include "scene/Node.h"
#include "scene/Scene.h"


using namespace ae;
using namespace glm;
using namespace std;


/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

BulletBodyModel::BulletBodyModel(PhysicsBody* body):
		PhysicsBodyModel(body),
		_btBody(nullptr),
		/*_btMotionState(nullptr)*/
		_motionState(nullptr) {

	AE_LOG_D("body: {:p}", (void*)body);

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
		case PHYSICS_BODY_TYPE::KINEMATIC:
			_btBody->setCollisionFlags(btCollisionObject::CF_KINEMATIC_OBJECT);
			break;
		case PHYSICS_BODY_TYPE::DYNAMIC:
			_btBody->setCollisionFlags(btCollisionObject::CF_DYNAMIC_OBJECT);
			break;
	}

	_btBody->setUserPointer((void*)body);
}

BulletBodyModel::~BulletBodyModel() {
	AE_LOG_D("Destroying BulletBodyModel {:p}", (void*)this);
}

/*********************************************************************************************
	PhysicsBodyModel
 *********************************************************************************************/

PHYSICS_BODY_TYPE BulletBodyModel::type() const {
	switch (_btBody->getCollisionFlags()) {
		case btCollisionObject::CF_STATIC_OBJECT: return PHYSICS_BODY_TYPE::STATIC;
		case btCollisionObject::CF_DYNAMIC_OBJECT: return PHYSICS_BODY_TYPE::DYNAMIC;;
		case btCollisionObject::CF_KINEMATIC_OBJECT: return PHYSICS_BODY_TYPE::KINEMATIC;
	}
}

void BulletBodyModel::type(PHYSICS_BODY_TYPE type) {

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
			break;
	}
	_btBody->setCollisionFlags(flags);
}

PhysicsShapeModel* BulletBodyModel::shape() const {
	return _shapeModel;
}

void BulletBodyModel::shape(PhysicsShapeModel* shape) {
	AE_LOG_T("shape: {:p}", (void*)shape);

	// front is either the only btCollisionShape or a btCompound shape with child shapes at index 1+
	if (auto btShape = dynamic_cast<BulletShapeModel*>(shape)->btShapes().front()) {

		_btBody->setCollisionShape(btShape.get());

		auto mass = BulletBodyModel::mass(); // BAD NO WORKIE
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
	}
}

float BulletBodyModel::mass() const {
//	return 1.0;
	auto invMass = _btBody->getInvMass();
	return (invMass != 0
			? 1.0f/_btBody->getInvMass()
			: 0);
}

void BulletBodyModel::mass(float mass) {

	_btBody->setMassProps(mass, _btBody->getLocalInertia());

	if (_autocalculatesMomentOfInertia) {
		calculateMomentOfIntertia();
	}

	_btBody->setActivationState(ACTIVE_TAG);
}

glm::vec3 BulletBodyModel::momentOfInertia() const {
	return GLMVec3FromBTVector3(_btBody->getLocalInertia());
}

void BulletBodyModel::momentOfInertia(const glm::vec3& moment) {

	if (!_autocalculatesMomentOfInertia) {
		_btBody->setMassProps(mass(),
							  BTVector3FromGLMVec3(moment));
		_btBody->updateInertiaTensor();
	}
	else {
		AE_LOG_W("Ignoring moment of inertia: autocalculatesMomentOfInertia to to true.");
	}
}

glm::vec3 BulletBodyModel::centerOfMass() const {
	return GLMVec3FromBTVector3(_btBody->getCenterOfMassPosition());
}

void BulletBodyModel::centerOfMass(const glm::vec3 offset) {
	_btBody->setCenterOfMassTransform(
			BTTransformFromGLMMat4(translate(mat4(1.0), offset)));
}

float BulletBodyModel::friction() const {
	return _btBody->getFriction();
}

void BulletBodyModel::friction(float friction) {
	_btBody->setFriction(friction);
}

float BulletBodyModel::rollingFriction() const {
	return _btBody->getRollingFriction();
}

void BulletBodyModel::rollingFriction(float friction) {
	_btBody->setRollingFriction(friction);
}

float BulletBodyModel::restitution() const {
	return _btBody->getRestitution();
}

void BulletBodyModel::restitution(float restitution) {
	_btBody->setRestitution(restitution);
}

glm::vec3 BulletBodyModel::linearVelocity() const {
	return GLMVec3FromBTVector3(_btBody->getLinearVelocity());
}

void BulletBodyModel::linearVelocity(const glm::vec3& velocity) {
	_btBody->setLinearVelocity(BTVector3FromGLMVec3(velocity));
}

glm::vec3 BulletBodyModel::angularVelocity() const {
	return GLMVec3FromBTVector3(_btBody->getAngularVelocity());
}

void BulletBodyModel::angularVelocity(const glm::vec3& velocity) {
	_btBody->setAngularVelocity(BTVector3FromGLMVec3(velocity));
}

glm::vec3 BulletBodyModel::linearFactor() const {
	return GLMVec3FromBTVector3(_btBody->getLinearFactor());
}

void BulletBodyModel::linearFactor(const glm::vec3& factor) {
	_btBody->setLinearFactor(BTVector3FromGLMVec3(factor));
}

glm::vec3 BulletBodyModel::angularFactor() const {
	return GLMVec3FromBTVector3(_btBody->getAngularFactor());
}

void BulletBodyModel::angularFactor(const glm::vec3& factor) {
	_btBody->setAngularFactor(BTVector3FromGLMVec3(factor));
}

float BulletBodyModel::linearDamping() const {
	return _btBody->getLinearDamping();
}

void BulletBodyModel::linearDamping(float damping) {
	_btBody->setDamping(damping, _btBody->getAngularDamping());
}

float BulletBodyModel::angularDamping() const {
	return _btBody->getAngularDamping();
}

void BulletBodyModel::angularDamping(float damping) {
	_btBody->setDamping(_btBody->getLinearDamping(), damping);
}

float BulletBodyModel::linearSleepingThreshold() const {
	return _btBody->getLinearSleepingThreshold();
}

void BulletBodyModel::linearSleepingThreshold(float threshold) {
	_btBody->setSleepingThresholds(threshold, _btBody->getAngularSleepingThreshold());
}

float BulletBodyModel::angularSleepingThreshold() const {
	return _btBody->getAngularSleepingThreshold();
}

void BulletBodyModel::angularSleepingThreshold(float threshold) {
	_btBody->setSleepingThresholds(_btBody->getLinearSleepingThreshold(), threshold);
}

void BulletBodyModel::applyForce(const vec3& force, const vec3& location) {
	_btBody->applyForce(BTVector3FromGLMVec3(force),
						BTVector3FromGLMVec3(location));
}

void BulletBodyModel::applyCentralForce(const vec3& force) {
	_btBody->applyCentralForce(BTVector3FromGLMVec3(force));
}

void BulletBodyModel::applyImpulse(const vec3& impulse, const vec3& location) {
	_btBody->applyImpulse(BTVector3FromGLMVec3(impulse),
						BTVector3FromGLMVec3(location));
}

void BulletBodyModel::applyCentralImpulse(const vec3& impulse) {
	_btBody->applyCentralImpulse(BTVector3FromGLMVec3(impulse));
}

void BulletBodyModel::applyTorque(const vec3& torque) {
	_btBody->applyTorque(BTVector3FromGLMVec3(torque));
}

void BulletBodyModel::applyTorqueImpulse(const vec3& torque) {
	_btBody->applyTorqueImpulse(BTVector3FromGLMVec3(torque));
}

bool BulletBodyModel::affectedByGravity() const {
	// *** test this ***
	auto gravity = _btBody->getGravity();
	return (gravity.x() != 0)
		   || (gravity.y() != 0)
		   || (gravity.z() != 0);
}

vec3 BulletBodyModel::totalForce() const {
	return GLMVec3FromBTVector3(_btBody->getTotalForce());
}

vec3 BulletBodyModel::totalTorque() const {
	return GLMVec3FromBTVector3(_btBody->getTotalTorque());
}

void BulletBodyModel::affectedByGravity(bool affectedByGravity) {
	// *** test this ***
	_btBody->setGravity(affectedByGravity
						? btVector3{1.0, 1.0, 1.0}
						: btVector3{0, 0, 0});
}

bool BulletBodyModel::allowsResting() const {
	// *** test this ***
	return (_btBody->getActivationState() != DISABLE_DEACTIVATION);
}

void BulletBodyModel::allowsResting(bool allowsResting) {
	// *** test this ***
	_btBody->setActivationState(allowsResting
							   ? ACTIVE_TAG
							   : DISABLE_DEACTIVATION);
}

bool BulletBodyModel::resting() const {
	return (_btBody->getActivationState() == ISLAND_SLEEPING);
}

void BulletBodyModel::resting(bool resting) {
	_btBody->setActivationState(resting
								? ISLAND_SLEEPING
								: ACTIVE_TAG);
}

//glm::mat4 BulletBodyModel::worldTransform() const {
//
//	static btTransform transform;
//	_btBody->getMotionState()->getWorldTransform(transform);
//	return GLMMat4FromBTTransform(transform);
//}
//
//void BulletBodyModel::worldTransform(const glm::mat4& transform) {
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

void BulletBodyModel::clearForces() {
	_btBody->clearForces();
}

/*********************************************************************************************
	Internal
 *********************************************************************************************/

//void BulletBodyModel::worldTransform(const glm::mat4& worldTransform) {
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

shared_ptr<btRigidBody> BulletBodyModel::btBody() {
	return _btBody;
}

//shared_ptr<btDefaultMotionState> BulletBodyModel::btMotionState() {
//	return _btMotionState;
//}

shared_ptr<MotionState> BulletBodyModel::motionState() {
	return _motionState;
}

/*********************************************************************************************
	Private
 *********************************************************************************************/

void BulletBodyModel::calculateMomentOfIntertia() {

	if (auto btShapeModel = dynamic_cast<BulletShapeModel*>(_shapeModel)) {
		if (auto btShape = btShapeModel->btShapes().front()) {
			btVector3 localInertia;
			auto mass = _body->mass();
			btShape->calculateLocalInertia(mass, localInertia);
			_btBody->setMassProps(mass, localInertia);
			_btBody->updateInertiaTensor();
		}
		else{
			AE_LOG_W("Missing btCollisionShape.");
		}
	}
	else{
		AE_LOG_W("Missing PhysicsShapeModel.");
	}
}
