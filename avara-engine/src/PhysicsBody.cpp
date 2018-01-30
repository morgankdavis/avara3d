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
using namespace glm;
using namespace std;
using namespace ae::utils;


/***************************************************************************************
     MARK:   Lifecycle
 **************************************************************************************/

PhysicsBody::PhysicsBody(PhysicsBodyType type):
	m_type(type),
	m_shape(nullptr),
	m_velocityFactor(1.0),
	m_angularVelocityFactor(1.0),
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
	m_angularVelocity({0, 0, 0, 0}),
	m_resting(false),
	m_allowsResting(true),
	m_node(nullptr),
	m_btMotionState(nullptr),
	m_btRigidBody(nullptr) {

}

PhysicsBody::PhysicsBody(PhysicsBodyType type, shared_ptr<PhysicsShape> shape):
	PhysicsBody(type) {
	
		m_shape = shape;
}

/***************************************************************************************
     MARK:   Public
 **************************************************************************************/

PhysicsBodyType PhysicsBody::type() const {
	return m_type;
}

/* REMOVE? */ void PhysicsBody::type(PhysicsBodyType type) {
	m_type = type;
}

shared_ptr<PhysicsShape> PhysicsBody::shape() const {
	return m_shape;
}

/* REMOVE? */ void PhysicsBody::shape(shared_ptr<PhysicsShape> shape) {
	m_shape = shape;
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

vec4 PhysicsBody::angularVelocity() const {
	return m_angularVelocity;
}

void PhysicsBody::angularVelocity(vec4 velocity) {
	m_angularVelocity = velocity;
}

bool PhysicsBody::resting() const {
	return m_resting;
}

void PhysicsBody::resting(bool flag) {
	m_resting = flag;
}

bool PhysicsBody::allowsResting() const {
	return m_allowsResting;
}

void PhysicsBody::allowsResting(bool flag) {
	m_allowsResting = flag;
}

void PhysicsBody::applyForce(glm::vec3 force, bool impulse) {
	
}

void PhysicsBody::applyForce(glm::vec3 force, glm::vec3 location, bool impulse) {
	
}

void PhysicsBody::applyTorque(glm::vec4 force, bool impulse) {
	
}

void PhysicsBody::clearForces() {
	
}

void PhysicsBody::resetTransform() {
	
}

/***************************************************************************************
     MARK:   Internal
 **************************************************************************************/

void PhysicsBody::addedToNode(Node& node) {
	m_node = &node;
	
//	if (!m_shape) {
//		m_shape = make_shared<PhysicsShape>(m_node->geometry(), PhysicsShapeType_ConvexHull);
//	}
	
	if (m_shape->btShape() == nullptr) {
		m_shape->createBTShape();
	}
	
	btTransform transform;
	transform.setFromOpenGLMatrix(value_ptr(m_node->worldTransform()));
	m_btMotionState = make_shared<btDefaultMotionState>(transform);

	btCollisionShape* collisionShape = static_cast<btCollisionShape*>(m_shape->btShape().get());

	btVector3 localInertia(0, 0, 0);
	if (m_mass != 0) collisionShape->calculateLocalInertia(m_mass, localInertia);
	
	btRigidBody::btRigidBodyConstructionInfo rigidBodyInfo((m_type == PhysicsBodyType_Static ? 0 : m_mass),
														   m_btMotionState.get(),
														   collisionShape,
														   localInertia);
	
	// velocity factor
	// angular velocity factor
	// afected by gravity
	rigidBodyInfo.m_mass = m_mass;
	// charge
	rigidBodyInfo.m_friction = m_friction;
	rigidBodyInfo.m_rollingFriction = m_rollingFriction;
	rigidBodyInfo.m_restitution = m_restitution;
	rigidBodyInfo.m_linearDamping = m_damping;
	rigidBodyInfo.m_angularDamping = m_angularDamping;
	// moment of inertia
	// velocity
	// angular velocity
	// resting
	// allows resting

	m_btRigidBody = make_shared<btRigidBody>(rigidBodyInfo);
	
	m_node->scene()->physicsWorld()->btWorld()->addRigidBody(m_btRigidBody.get());
}

shared_ptr<btDefaultMotionState> PhysicsBody::btMotionState() const {
	return m_btMotionState;
}

