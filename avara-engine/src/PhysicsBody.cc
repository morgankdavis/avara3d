//
//  PhysicsBody.cc
//	avara-engine
//
//  Created by Morgan Davis on 1/26/18.
//  Copyright © 2018 Morgan K Davis. All rights reserved.
//

#include "PhysicsBody.h"


#include "Logger.h"
#include "Node.h"
#include "PhysicsShape.h"
#include "PhysicsWorld.h"


using namespace ae;
using namespace glm;
using namespace std;


/***************************************************************************************
     Public Static
 ***************************************************************************************/

shared_ptr<PhysicsBody> PhysicsBody::StaticBody() {
	return make_shared<PhysicsBody>(PHYSICS_BODY_TYPE::STATIC);
}

shared_ptr<PhysicsBody> PhysicsBody::DynamicBody() {
	return make_shared<PhysicsBody>(PHYSICS_BODY_TYPE::DYNAMIC);
}

shared_ptr<PhysicsBody> PhysicsBody::KinematicBody() {
	return make_shared<PhysicsBody>(PHYSICS_BODY_TYPE::KINEMATIC);
}

/***************************************************************************************
     Lifecycle
 ***************************************************************************************/

PhysicsBody::PhysicsBody():
	m_type(PHYSICS_BODY_TYPE::STATIC),
	m_shape(nullptr),
	m_linearFactor({1.0, 1.0, 1.0}),
	m_angularFactor({1.0, 1.0, 1.0}),
	m_mass(1.0),
	m_friction(0.5),
	m_rollingFriction(0.0),
	m_restitution(0.0),
	m_linearDamping(0.0),
	m_angularDamping(0.0),
	m_momentOfInertia({0, 0, 0}),
	//m_momentOfInertia({1000, 1000, 1000}),
	m_linearVelocity({0, 0, 0}),
	m_angularVelocity({0, 0, 0}),
	m_linearSleepingThreshold(0.8),
	m_angularSleepingThreshold(1.0),
	m_allowsResting(true),
	m_affectedByGravity(true),
	m_resting(false),
	m_node({}),
	m_dirtyBits(PHYSICS_BODY_DIRTY_BITS::ALL) {
	
}

PhysicsBody::PhysicsBody(PHYSICS_BODY_TYPE type):
	PhysicsBody() {
	
		this->type(type);
}

//PhysicsBody::PhysicsBody(PHYSICS_BODY_TYPE type, shared_ptr<PhysicsShape> shape):
//	PhysicsBody(type) {
//		
//		this->type(type);
//		this->shape(shape);
//}

PhysicsBody::~PhysicsBody() {
	AE_LOG_D("Destroying PhysicsBody {:p}", (void*)this);
}

/***************************************************************************************
     Public
 ***************************************************************************************/

PHYSICS_BODY_TYPE PhysicsBody::type() const {
	return m_type;
}

void PhysicsBody::type(PHYSICS_BODY_TYPE type) {
	m_type = type;
	
	m_dirtyBits = PHYSICS_BODY_DIRTY_BITS_ADD(m_dirtyBits, PHYSICS_BODY_DIRTY_BITS::TYPE);
}

shared_ptr<PhysicsShape> PhysicsBody::shape() const {
	return m_shape;
}

void PhysicsBody::shape(shared_ptr<PhysicsShape> shape) {
	m_shape = shape;
	m_shape->attachedToBody(shared_from_this());
	
	m_dirtyBits = PHYSICS_BODY_DIRTY_BITS_ADD(m_dirtyBits, PHYSICS_BODY_DIRTY_BITS::SHAPE);
}

float PhysicsBody::mass() const {
	return m_mass;
}

void PhysicsBody::mass(float mass) {
	m_mass = mass;
	
	m_dirtyBits = PHYSICS_BODY_DIRTY_BITS_ADD(m_dirtyBits, PHYSICS_BODY_DIRTY_BITS::MASS);
}

vec3 PhysicsBody::momentOfInertia() const {
	return m_momentOfInertia;
}

void PhysicsBody::momentOfInertia(vec3 moment) {
	m_momentOfInertia = moment;
	
	m_dirtyBits = PHYSICS_BODY_DIRTY_BITS_ADD(m_dirtyBits, PHYSICS_BODY_DIRTY_BITS::MOMENT_OF_INERTIA);
}

float PhysicsBody::friction() const {
	return m_friction;
}

void PhysicsBody::friction(float friction) {
	m_friction = friction;
	
	m_dirtyBits = PHYSICS_BODY_DIRTY_BITS_ADD(m_dirtyBits, PHYSICS_BODY_DIRTY_BITS::FRICTION);
}

float PhysicsBody::rollingFriction() const {
	return m_rollingFriction;
}

void PhysicsBody::rollingFriction(float friction) {
	m_rollingFriction = friction;
	
	m_dirtyBits = PHYSICS_BODY_DIRTY_BITS_ADD(m_dirtyBits, PHYSICS_BODY_DIRTY_BITS::ROLLING_FRICTION);
}

float PhysicsBody::restitution() const {
	return m_restitution;
}

void PhysicsBody::restitution(float restitution) {
	m_restitution = restitution;
	
	m_dirtyBits = PHYSICS_BODY_DIRTY_BITS_ADD(m_dirtyBits, PHYSICS_BODY_DIRTY_BITS::RESTITUTION);
}

vec3 PhysicsBody::linearVelocity() const {
	return m_linearVelocity;
}

void PhysicsBody::linearVelocity(vec3 velocity, bool setDirty) {
	m_linearVelocity = velocity;
	
	if (setDirty) {
		m_dirtyBits = PHYSICS_BODY_DIRTY_BITS_ADD(m_dirtyBits, PHYSICS_BODY_DIRTY_BITS::LINEAR_VELOCITY);
	}
}

vec3 PhysicsBody::angularVelocity() const {
	return m_angularVelocity;
}

void PhysicsBody::angularVelocity(vec3 velocity, bool setDirty) {
	m_angularVelocity = velocity;
	
	if (setDirty) {
		m_dirtyBits = PHYSICS_BODY_DIRTY_BITS_ADD(m_dirtyBits, PHYSICS_BODY_DIRTY_BITS::ANGULAR_VELOCITY);
	}
}

vec3 PhysicsBody::linearFactor() const {
	return m_linearFactor;
}

void PhysicsBody::linearFactor(vec3 factor) {
	m_linearFactor = factor;
	
	m_dirtyBits = PHYSICS_BODY_DIRTY_BITS_ADD(m_dirtyBits, PHYSICS_BODY_DIRTY_BITS::LINEAR_FACTOR);
}

vec3 PhysicsBody::angularFactor() const {
	return m_angularFactor;
}

void PhysicsBody::angularFactor(vec3 factor) {
	m_angularFactor = factor;
	
	m_dirtyBits = PHYSICS_BODY_DIRTY_BITS_ADD(m_dirtyBits, PHYSICS_BODY_DIRTY_BITS::ANGULAR_FACTOR);
}

float PhysicsBody::linearDamping() const {
	return m_linearDamping;
}

void PhysicsBody::linearDamping(float damping) {
	m_linearDamping = damping;
	
	m_dirtyBits = PHYSICS_BODY_DIRTY_BITS_ADD(m_dirtyBits, PHYSICS_BODY_DIRTY_BITS::LINEAR_DAMPING);
}

float PhysicsBody::angularDamping() const {
	return m_angularDamping;
}

void PhysicsBody::angularDamping(float damping) {
	m_angularDamping = damping;
	
	m_dirtyBits = PHYSICS_BODY_DIRTY_BITS_ADD(m_dirtyBits, PHYSICS_BODY_DIRTY_BITS::ANGULAR_DAMPING);
}

float PhysicsBody::linearSleepingThreshold() const {
	return m_linearSleepingThreshold;
}

void PhysicsBody::linearSleepingThreshold(float threshold) {
	m_linearSleepingThreshold = threshold;
	
	m_dirtyBits = PHYSICS_BODY_DIRTY_BITS_ADD(m_dirtyBits, PHYSICS_BODY_DIRTY_BITS::LINEAR_SLEEPING_THRESHOLD);
}

float PhysicsBody::angularSleepingThreshold() const {
	return m_angularSleepingThreshold;
}

void PhysicsBody::angularSleepingThreshold(float threshold) {
	m_angularSleepingThreshold = threshold;
	
	m_dirtyBits = PHYSICS_BODY_DIRTY_BITS_ADD(m_dirtyBits, PHYSICS_BODY_DIRTY_BITS::ANGULAR_SLEEPING_THRESHOLD);
}

bool PhysicsBody::affectedByGravity() const {
	return m_affectedByGravity;
}

void PhysicsBody::affectedByGravity(bool flag) {
	m_affectedByGravity = flag;
	
	m_dirtyBits = PHYSICS_BODY_DIRTY_BITS_ADD(m_dirtyBits, PHYSICS_BODY_DIRTY_BITS::AFFECTED_BY_GRAVITY);
}

bool PhysicsBody::allowsResting() const {
	return m_allowsResting;
}

void PhysicsBody::allowsResting(bool flag) {
	m_allowsResting = flag;

	m_dirtyBits = PHYSICS_BODY_DIRTY_BITS_ADD(m_dirtyBits, PHYSICS_BODY_DIRTY_BITS::ALLOWS_RESTING);
}

bool PhysicsBody::resting() const {
	
	return m_resting;
}

void PhysicsBody::applyForce(vec3 force, bool impulse) {
	#warning fix
	
	//applyForce(force, {0, 0, 0}, impulse);
//	void 	applyCentralImpulse (const btVector3 &impulse)
//	
//	void 	applyTorqueImpulse (const btVector3 &torque)
}

void PhysicsBody::applyForce(vec3 force, vec3 location, bool impulse) {
	
#warning save list of applied forces
	
//	if (m_btRigidBody) {
//		/**********
//		if (impulse) m_btRigidBody->applyImpulse(BTVector3FromGLMVec3(force), BTVector3FromGLMVec3(location));
//		else m_btRigidBody->applyForce(BTVector3FromGLMVec3(force), BTVector3FromGLMVec3(location));
//		 *********/
//	}
	
	m_dirtyBits = PHYSICS_BODY_DIRTY_BITS_ADD(m_dirtyBits, PHYSICS_BODY_DIRTY_BITS::FORCES);
}

void PhysicsBody::applyTorque(vec3 torque, bool impulse) {
	
#warning save list of applied torques
	
//	if (m_btRigidBody) {
//		/**********
//		if (impulse) m_btRigidBody->applyTorqueImpulse(BTVector3FromGLMVec3(torque));
//		else  m_btRigidBody->applyTorque(BTVector3FromGLMVec3(torque));
//		 *********/
//	}
	
	m_dirtyBits = PHYSICS_BODY_DIRTY_BITS_ADD(m_dirtyBits, PHYSICS_BODY_DIRTY_BITS::TORQUES);
}

void PhysicsBody::clearForces() {
	#warning FIX
	//if (m_btRigidBody) m_btRigidBody->clearForces();
}

void PhysicsBody::resetTransform() {
	#warning FIX
	//m_dirtyBits = PHYSICS_BODY_DIRTY_BITS_ADD(m_dirtyBits, PHYSICS_BODY_DIRTY_BITS::TRANSFORM);
	
	//proceedToTransform (const btTransform &newTrans)
}

/***************************************************************************************
     Internal
 ***************************************************************************************/

void PhysicsBody::resting(bool resting) {
	m_resting = resting;
	
#warning need to update BT motion state?
}

void PhysicsBody::attachedToNode(shared_ptr<Node> node) {
	m_node = node;
	if (node) {
		checkShape();
	}
}

void PhysicsBody::geometryAttachedToNode(std::shared_ptr<Geometry> geometry) {
	if (geometry) {
		checkShape();
	}
}

weak_ptr<Node> PhysicsBody::node() const {
	return m_node;
}

PHYSICS_BODY_DIRTY_BITS PhysicsBody::dirtyBits() const {
	return m_dirtyBits;
}

void PhysicsBody::dirtyBits(PHYSICS_BODY_DIRTY_BITS bits) {
	m_dirtyBits = bits;
}

/**************************************************************************************
     Private
 **************************************************************************************/

void PhysicsBody::checkShape() {
//	if (!m_shape) {
//		if (auto node = m_node.lock()) {
//			auto geometry = node->geometry();
//			if (geometry) {
//				shape(make_shared<PhysicsShape>(geometry, PHYSICS_SHAPE_TYPE::CONVEX_HULL));
//			}
//			else {
//				shape(make_shared<PhysicsShape>(node, PHYSICS_SHAPE_TYPE::CONVEX_HULL));
//			}
//		}
//	}
	
	
	
	if (auto node = m_node.lock()) {
		auto geometry = node->geometry();
		if (!m_shape) {
			if (geometry) {
				shape(make_shared<PhysicsShape>(geometry, PHYSICS_SHAPE_TYPE::CONVEX_HULL));
			}
			else {
				shape(make_shared<PhysicsShape>(node, PHYSICS_SHAPE_TYPE::CONVEX_HULL));
			}
		}
		else {
			if (geometry) {
				m_shape->sourceGeometry(geometry);
			}
			else {
				m_shape->sourceNode(node);
			}
		}
	}
}
