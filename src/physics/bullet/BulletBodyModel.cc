//
// Created by mkd on 10/29/23.
//

#include "physics/bullet/BulletBodyModel.h"

#include "btBulletCollisionCommon.h"
#include "btBulletDynamicsCommon.h"
#include "BulletCollision/Gimpact/btGImpactShape.h"
#include "glm/gtc/type_ptr.hpp"
#include "LinearMath/btIDebugDraw.h"
//#include "LinearMath/btScalar.h" // btGetVersion() !
#include "magic_enum.hpp"

#include "diagnostic/logging/Logger.h"
#include "geometry/Geometry.h"
#include "physics/ConvexDecomposer.h"
#include "physics/PhysicsBody.h"
#include "physics/PhysicsBodyModel.h"
#include "physics/PhysicsShape.h"
#include "physics/PhysicalWorld.h"
#include "physics/bullet/BulletDebugDrawer.h"
#include "physics/bullet/BulletShapeModel.h"
#include "physics/bullet/BulletWorldModel.h"
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
		_btMotionState(nullptr) {

	AE_LOG_D("body: {:p}", (void*)&body);

	_body = body;

	auto node = body->node();
	auto shape = body->shape();

	shared_ptr<btRigidBody> btBody = nullptr;

	if (!shape || !node) {
		btRigidBody::btRigidBodyConstructionInfo rigidBodyInfo(0,
															   nullptr,
															   nullptr);

		btBody = make_shared<btRigidBody>(rigidBodyInfo);
	}
	else {

		auto shapeModel = static_cast<BulletShapeModel*>(body->shape()->model());

		// front is either the only btCollisionShape or a btCompound shape with child shapes at index 1+
		auto btShape = shapeModel->btShapes().front();

		AE_LOG_D("Creating rigid body for physics body {:p}...", (void *) body);

		bool wasScaled = false;
		auto transform = BulletWorldModel::BTTransformFromGLMMat4(
				BulletWorldModel::TransformByRemovingScale(node->worldTransform(),
														   wasScaled));
		if (wasScaled) {
			// TODO: do something about this
			// can hold a burned transformed vertex data in the physics body/shape?
			AE_LOG_W("Ignorning scale for Node {:p} with PhysicsBody {:p}.",
					 (void *) &node, (void *) &body);
		}

		auto localInertia = BulletWorldModel::BTVector3FromGLMVec3(body->momentOfInertia());
		auto mass = body->mass();
		if (body->type() == PHYSICS_BODY_TYPE::STATIC
			|| body->type() == PHYSICS_BODY_TYPE::KINEMATIC) {
			mass = 0;
		}
		else if (body->type() == PHYSICS_BODY_TYPE::DYNAMIC) {
			btShape->calculateLocalInertia(mass, localInertia);
		}

		auto newMotionState = make_shared<btDefaultMotionState>(transform);
		btRigidBody::btRigidBodyConstructionInfo rigidBodyInfo(mass,
															   newMotionState.get(),
															   btShape.get(),
															   localInertia);

		rigidBodyInfo.m_mass = mass;
		rigidBodyInfo.m_linearDamping = body->linearDamping();
		rigidBodyInfo.m_angularDamping = body->angularDamping();
		rigidBodyInfo.m_friction = body->friction();
		rigidBodyInfo.m_rollingFriction = body->rollingFriction();
		rigidBodyInfo.m_restitution = body->restitution();
		rigidBodyInfo.m_linearSleepingThreshold = body->linearSleepingThreshold();
		rigidBodyInfo.m_angularSleepingThreshold = body->angularSleepingThreshold();

		btBody = make_shared<btRigidBody>(rigidBodyInfo);

		_btMotionState = newMotionState; // TEMPORARY
	}

	btBody->setUserPointer((void*)&body);

	switch (body->type()) {
		case PHYSICS_BODY_TYPE::STATIC:
			btBody->setCollisionFlags(btCollisionObject::CF_STATIC_OBJECT);
			break;
		case PHYSICS_BODY_TYPE::KINEMATIC:
			btBody->setCollisionFlags(btCollisionObject::CF_KINEMATIC_OBJECT);
			break;
		case PHYSICS_BODY_TYPE::DYNAMIC:
			btBody->setCollisionFlags(btCollisionObject::CF_DYNAMIC_OBJECT);
			break;
	}

//	btWorld->addRigidBody(btBody.get());

//	bodyResources->btBody(btBody);
//	bodyResources->btMotionState(newMotionState);

	_btBody = btBody;
//	_btMotionState = newMotionState;  // TEMPORARILY DISABLED

	// I'm a cheap bastard
//	linearFactor(body->linearFactor());
//	angularFactor(body->angularFactor());
//	linearDamping(body->linearDamping());
//	angularDamping(body->angularDamping());
//	linearSleepingThreshold(body->linearSleepingThreshold());
//	angularSleepingThreshold(body->angularSleepingThreshold());
//	linearVelocity(body->linearVelocity()); // ?
//	angularVelocity(body->angularVelocity()); // ?
//	affectedByGravity(body->affectedByGravity());
//	allowsResting(body->allowsResting());
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

	int btCollisionFlags = 0;

	switch (type) {
		case PHYSICS_BODY_TYPE::STATIC:
			btCollisionFlags = btCollisionObject::CF_STATIC_OBJECT;
			break;
		case PHYSICS_BODY_TYPE::DYNAMIC:
			btCollisionFlags = btCollisionObject::CF_DYNAMIC_OBJECT;
			break;
		case PHYSICS_BODY_TYPE::KINEMATIC:
			btCollisionFlags = btCollisionObject::CF_KINEMATIC_OBJECT;
			break;
	}

	// setCollisionFlags() ?
	//  rebuild?
}

PhysicsShapeModel* BulletBodyModel::shape() const {
	// meh?
}

void BulletBodyModel::shape(PhysicsShapeModel* shape) {
	// rebuild



	// https://pybullet.org/Bullet/phpBB3/viewtopic.php?p=43923&sid=187e552b028cd64fe2e831df414d382a#p43923






	auto shapeModel = static_cast<BulletShapeModel*>(shape);

	// front is either the only btCollisionShape or a btCompound shape with child shapes at index 1+
	auto btShape = shapeModel->btShapes().front();

	AE_LOG_D("Creating rigid body for physics body {:p}...", (void *)_body);

	bool wasScaled = false;
	auto transform = BulletWorldModel::BTTransformFromGLMMat4(
			BulletWorldModel::TransformByRemovingScale(_body->node()->worldTransform(),
													   wasScaled));
	if (wasScaled) {
		// TODO: do something about this
		// can hold a burned transformed vertex data in the physics body/shape?
		AE_LOG_W("Ignorning scale for Node {:p} with PhysicsBody {:p}.",
				 (void *)_body->node(), (void *)_body);
	}

	auto localInertia = BulletWorldModel::BTVector3FromGLMVec3(_body->momentOfInertia());
	auto mass = _body->mass();
	if (_body->type() == PHYSICS_BODY_TYPE::STATIC
		|| _body->type() == PHYSICS_BODY_TYPE::KINEMATIC) {
		mass = 0;
	}
	else if (_body->type() == PHYSICS_BODY_TYPE::DYNAMIC) {
		btShape->calculateLocalInertia(mass, localInertia);
	}


	_btBody->setCollisionShape(btShape.get());
	_btBody->setMassProps(mass, localInertia);
	_btBody->updateInertiaTensor();

	auto newMotionState = make_shared<btDefaultMotionState>(transform);
//	btRigidBody::btRigidBodyConstructionInfo rigidBodyInfo(mass,
//														   newMotionState.get(),
//														   btShape.get(),
//														   localInertia);

//	rigidBodyInfo.m_mass = mass;
//	rigidBodyInfo.m_linearDamping = body->linearDamping();
//	rigidBodyInfo.m_angularDamping = body->angularDamping();
//	rigidBodyInfo.m_friction = body->friction();
//	rigidBodyInfo.m_rollingFriction = body->rollingFriction();
//	rigidBodyInfo.m_restitution = body->restitution();
//	rigidBodyInfo.m_linearSleepingThreshold = body->linearSleepingThreshold();
//	rigidBodyInfo.m_angularSleepingThreshold = body->angularSleepingThreshold();

	//_btBody = make_shared<btRigidBody>(rigidBodyInfo);

	_btMotionState = newMotionState; // TEMPORARY
}

//glm::mat4 BulletBodyModel::worldTransform() const {
//
//}

//void BulletBodyModel::worldTransform(const glm::mat4& transform) {
//
//}

float BulletBodyModel::mass() const {
	// *** TEST THIS ***
	return 1.0f/_btBody->getInvMass();
}

void BulletBodyModel::mass(float mass) {
	// *** TEST THIS ***
	// https://stackoverflow.com/questions/26229836/bullet-physics-library-switching-rigidbody-between-static-object-and-dynamic-obj
	// https://pybullet.org/Bullet/phpBB3/viewtopic.php?p=43923&sid=187e552b028cd64fe2e831df414d382a#p43923
//	btVector3 inertia;
//	_btBody->getCollisionShape()->calculateLocalInertia(mass, inertia);
//	_btBody->setMassProps(mass, inertia);
//	_btBody->setActivationState(ACTIVE_TAG);
}

glm::vec3 BulletBodyModel::momentOfInertia() const {
	return BulletWorldModel::GLMVec3FromBTVector3(_btBody->getLocalInertia());
}

void BulletBodyModel::momentOfInertia(const glm::vec3& moment) {
	// re-create?
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
	return BulletWorldModel::GLMVec3FromBTVector3(_btBody->getLinearVelocity());
}

void BulletBodyModel::linearVelocity(const glm::vec3& velocity) {
	_btBody->setLinearVelocity(BulletWorldModel::BTVector3FromGLMVec3(velocity));
}

glm::vec3 BulletBodyModel::angularVelocity() const {
	return BulletWorldModel::GLMVec3FromBTVector3(_btBody->getAngularVelocity());
}

void BulletBodyModel::angularVelocity(const glm::vec3& velocity) {
	_btBody->setAngularVelocity(BulletWorldModel::BTVector3FromGLMVec3(velocity));
}

glm::vec3 BulletBodyModel::linearFactor() const {
	return BulletWorldModel::GLMVec3FromBTVector3(_btBody->getLinearFactor());
}

void BulletBodyModel::linearFactor(const glm::vec3& factor) {
	_btBody->setLinearFactor(BulletWorldModel::BTVector3FromGLMVec3(factor));
}

glm::vec3 BulletBodyModel::angularFactor() const {
	return BulletWorldModel::GLMVec3FromBTVector3(_btBody->getAngularFactor());
}

void BulletBodyModel::angularFactor(const glm::vec3& factor) {
	_btBody->setAngularFactor(BulletWorldModel::BTVector3FromGLMVec3(factor));
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

bool BulletBodyModel::affectedByGravity() const {
	// *** test this ***
	auto gravity = _btBody->getGravity();
	return (gravity.x() != 0)
		   || (gravity.y() != 0)
		   || (gravity.z() != 0);
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
/*********************************************************************************************
	Internal
 *********************************************************************************************/

shared_ptr<btRigidBody> BulletBodyModel::btBody() {
	return _btBody;
}

void BulletBodyModel::btBody(shared_ptr<btRigidBody> body) {
	_btBody = body;
}

shared_ptr<btDefaultMotionState> BulletBodyModel::btMotionState() {
	return _btMotionState;
}

void BulletBodyModel::btMotionState(shared_ptr<btDefaultMotionState> motionState) {
	_btMotionState = motionState;
}
