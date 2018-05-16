//
//  PhysicsBody.cpp
//	avara-engine
//
//  Created by Morgan Davis on 1/26/18.
//  Copyright © 2018 Morgan K Davis. All rights reserved.
//

#include "PhysicsBody.h"


#include "Logger.h"
#include "Node.h"
#include "Scene.h"
#include "PhysicsShape.h"
#include "PhysicsWorld.h"
#include "Utilities.h"


using namespace ae;
using namespace ae::utils;
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
	m_velocityFactor({1.0, 1.0, 1.0}),
	m_angularVelocityFactor({1.0, 1.0, 1.0}),
	m_affectedByGravity(true),
	m_mass(1.0),
	m_charge(0.0),
	m_friction(0.0),
	m_rollingFriction(0.0),
	m_restitution(0.0),
	m_damping(0.0),
	m_angularDamping(0.0),
	m_momentOfInertia({0, 0, 0}),
	m_velocity({0, 0, 0}),
	m_angularVelocity({0, 0, 0}),
	m_resting(false),
	m_allowsResting(true),
	m_node({}),
	m_simulationID(0),
	m_dirtyBits(PHYSICS_BODY_DIRTY_BITS::ALL) {
	
}

PhysicsBody::PhysicsBody(PHYSICS_BODY_TYPE type):
	PhysicsBody() {
	
		this->type(type);
}

PhysicsBody::PhysicsBody(PHYSICS_BODY_TYPE type, shared_ptr<PhysicsShape> shape):
	PhysicsBody(type) {
		
		this->type(type);
		this->shape(shape);
}

/***************************************************************************************
     Public
 ***************************************************************************************/

PHYSICS_BODY_TYPE PhysicsBody::type() const {
	return m_type;
}

void PhysicsBody::type(PHYSICS_BODY_TYPE type) {
	m_type = type;
	
	m_dirtyBits = PHYSICS_BODY_DIRTY_BITS_ADD(m_dirtyBits, PHYSICS_BODY_DIRTY_BITS::MODEL);
}

shared_ptr<PhysicsShape> PhysicsBody::shape() const {
	return m_shape;
}

void PhysicsBody::shape(shared_ptr<PhysicsShape> shape) {
	m_shape = shape;
	m_shape->attachedToBody(shared_from_this());
}

vec3 PhysicsBody::velocityFactor() const {
	return m_velocityFactor;
}

void PhysicsBody::velocityFactor(vec3 factor) {
	m_velocityFactor = factor;
}

vec3 PhysicsBody::angularVelocityFactor() const {
	return m_angularVelocityFactor;
}

void PhysicsBody::angularVelocityFactor(vec3 factor) {
	m_angularVelocityFactor = factor;
}

bool PhysicsBody::affectedByGravity() const {
	return m_affectedByGravity;
}

void PhysicsBody::affectedByGravity(bool flag) {
	m_affectedByGravity = flag;
}

float PhysicsBody::mass() const {
	return m_mass;
}

void PhysicsBody::mass(float mass) {
	m_mass = mass;
}

float PhysicsBody::charge() const {
	return m_charge;
}

void PhysicsBody::charge(float charge) {
	m_charge = charge;
}

float PhysicsBody::friction() const {
	return m_friction;
}

void PhysicsBody::friction(float friction) {
	m_friction = friction;
}

float PhysicsBody::rollingFriction() const {
	return m_rollingFriction;
}

void PhysicsBody::rollingFriction(float friction) {
	m_rollingFriction = friction;
}

float PhysicsBody::restitution() const {
	return m_restitution;
}

void PhysicsBody::restitution(float restitution) {
	m_restitution = restitution;
}

float PhysicsBody::damping() const {
	return m_damping;
}

void PhysicsBody::damping(float damping) {
	m_damping = damping;
}

float PhysicsBody::angularDamping() const {
	return m_angularDamping;
}

void PhysicsBody::angularDamping(float damping) {
	m_angularDamping = damping;
}

vec3 PhysicsBody::momentOfInertia() const {
	return m_momentOfInertia;
}

void PhysicsBody::momentOfInertia(vec3 moment) {
	m_momentOfInertia = moment;
}

vec3 PhysicsBody::velocity() const {
	return m_velocity;
}

void PhysicsBody::velocity(vec3 velocity) {
	m_velocity = velocity;
}

vec3 PhysicsBody::angularVelocity() const {
	return m_angularVelocity;
}

void PhysicsBody::angularVelocity(vec3 velocity) {
	m_angularVelocity = velocity;
}

float PhysicsBody::linearSleepingThreshold() const {
	return m_linearSleepingThreshold;
}

void PhysicsBody::linearSleepingThreshold(float threshold) {
	m_linearSleepingThreshold = threshold;
}

float PhysicsBody::angularSleepingThreshold() const {
	return m_angularSleepingThreshold;
}

void PhysicsBody::angularSleepingThreshold(float threshold) {
	m_angularSleepingThreshold = threshold;
}

bool PhysicsBody::allowsResting() const {
	return m_allowsResting;
}

void PhysicsBody::allowsResting(bool flag) {
	m_allowsResting = flag;
}

bool PhysicsBody::resting() const {
	return m_resting;
}

void PhysicsBody::resting(bool flag) {
	m_resting = flag;
}

void PhysicsBody::applyForce(vec3 force, bool impulse) {
	applyForce(force, {0, 0, 0}, impulse);
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
	//if (m_btRigidBody) m_btRigidBody->clearForces();
}

void PhysicsBody::resetTransform() {
	m_dirtyBits = PHYSICS_BODY_DIRTY_BITS_ADD(m_dirtyBits, PHYSICS_BODY_DIRTY_BITS::TRANSFORM);
}

/***************************************************************************************
     Internal
 ***************************************************************************************/

void PhysicsBody::attachedToNode(shared_ptr<Node> node) {
	m_node = node;
}

weak_ptr<Node> PhysicsBody::node() const {
	return m_node;
}

PHYSICS_BODY_ID PhysicsBody::simulationID() const {
	return m_simulationID;
}

void PhysicsBody::simulationID(PHYSICS_BODY_ID simID) {
	m_simulationID = simID;
}

PHYSICS_BODY_DIRTY_BITS PhysicsBody::dirtyBits() const {
	return m_dirtyBits;
}

void PhysicsBody::dirtyBits(PHYSICS_BODY_DIRTY_BITS bits) {
	m_dirtyBits = bits;
}
