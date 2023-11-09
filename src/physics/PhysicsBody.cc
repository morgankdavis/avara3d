//
//  PhysicsBody.cc
//	avara-engine
//
//  Created by Morgan Davis on 1/26/18.
//  Copyright © 2018 Morgan K Davis. All rights reserved.
//

#include "physics/PhysicsBody.h"

#include "magic_enum.hpp"

#include "diagnostic/logging/Logger.h"
#include "physics/PhysicsShape.h"
#include "scene/Node.h"


using namespace ae;
using namespace glm;
using namespace std;


/*********************************************************************************************
	Public Static
 *********************************************************************************************/

shared_ptr<PhysicsBody> PhysicsBody::StaticBody() {
	return make_shared<PhysicsBody>(PHYSICS_BODY_TYPE::STATIC);
}

shared_ptr<PhysicsBody> PhysicsBody::DynamicBody() {
	return make_shared<PhysicsBody>(PHYSICS_BODY_TYPE::DYNAMIC);
}

shared_ptr<PhysicsBody> PhysicsBody::KinematicBody() {
	return make_shared<PhysicsBody>(PHYSICS_BODY_TYPE::KINEMATIC);
}

/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

PhysicsBody::PhysicsBody(PHYSICS_BODY_TYPE type):
		_type(type),
		_shape({}),
		_linearFactor({1.0, 1.0, 1.0}),
		_angularFactor({1.0, 1.0, 1.0}),
		_mass(1.0),
		_friction(0.5),
		_rollingFriction(0.0),
		_restitution(0.0),
		_linearDamping(0.0),
		_angularDamping(0.0),
		_momentOfInertia({0, 0, 0}),
		//_momentOfInertia({1000, 1000, 1000}),
		_linearVelocity({0, 0, 0}),
		_angularVelocity({0, 0, 0}),
		_linearSleepingThreshold(0.8),
		_angularSleepingThreshold(1.0),
		_allowsResting(true),
		_affectedByGravity(true),
		_resting(false),
		_dirtyBits(PHYSICS_BODY_DIRTY_BITS::ALL) {

}

PhysicsBody::PhysicsBody(PHYSICS_BODY_TYPE type, shared_ptr<PhysicsShape> shape):
		PhysicsBody(type) {

	this->shape(shape);
}

PhysicsBody::~PhysicsBody() {
	AE_LOG_D("Destroying PhysicsBody {:p}", (void*)this);
}

/*********************************************************************************************
	Public
 *********************************************************************************************/

PHYSICS_BODY_TYPE PhysicsBody::type() const {
	return _type;
}

void PhysicsBody::type(PHYSICS_BODY_TYPE type) {
	_type = type;
	_dirtyBits = PHYSICS_BODY_DIRTY_BITS_ADD(_dirtyBits, PHYSICS_BODY_DIRTY_BITS::TYPE);
}

shared_ptr<PhysicsShape> PhysicsBody::shape() const {
	return _shape;
}

void PhysicsBody::shape(shared_ptr<PhysicsShape> shape) {
	_shape = shape;
	_dirtyBits = PHYSICS_BODY_DIRTY_BITS_ADD(_dirtyBits, PHYSICS_BODY_DIRTY_BITS::SHAPE);
}

float PhysicsBody::mass() const {
	return _mass;
}

void PhysicsBody::mass(float mass) {
	_mass = mass;
	_dirtyBits = PHYSICS_BODY_DIRTY_BITS_ADD(_dirtyBits, PHYSICS_BODY_DIRTY_BITS::MASS);
}

vec3 PhysicsBody::momentOfInertia() const {
	return _momentOfInertia;
}

void PhysicsBody::momentOfInertia(vec3 moment) {
	_momentOfInertia = moment;
	_dirtyBits = PHYSICS_BODY_DIRTY_BITS_ADD(_dirtyBits, PHYSICS_BODY_DIRTY_BITS::MOMENT_OF_INERTIA);
}

float PhysicsBody::friction() const {
	return _friction;
}

void PhysicsBody::friction(float friction) {
	_friction = friction;
	_dirtyBits = PHYSICS_BODY_DIRTY_BITS_ADD(_dirtyBits, PHYSICS_BODY_DIRTY_BITS::FRICTION);
}

float PhysicsBody::rollingFriction() const {
	return _rollingFriction;
}

void PhysicsBody::rollingFriction(float friction) {
	_rollingFriction = friction;
	_dirtyBits = PHYSICS_BODY_DIRTY_BITS_ADD(_dirtyBits, PHYSICS_BODY_DIRTY_BITS::ROLLING_FRICTION);
}

float PhysicsBody::restitution() const {
	return _restitution;
}

void PhysicsBody::restitution(float restitution) {
	_restitution = restitution;
	_dirtyBits = PHYSICS_BODY_DIRTY_BITS_ADD(_dirtyBits, PHYSICS_BODY_DIRTY_BITS::RESTITUTION);
}

vec3 PhysicsBody::linearVelocity() const {
	return _linearVelocity;
}

void PhysicsBody::linearVelocity(vec3 velocity, bool setDirty) {
	_linearVelocity = velocity;
	if (setDirty) {
		_dirtyBits = PHYSICS_BODY_DIRTY_BITS_ADD(_dirtyBits, PHYSICS_BODY_DIRTY_BITS::LINEAR_VELOCITY);
	}
}

vec3 PhysicsBody::angularVelocity() const {
	return _angularVelocity;
}

void PhysicsBody::angularVelocity(vec3 velocity, bool setDirty) {
	_angularVelocity = velocity;
	if (setDirty) {
		_dirtyBits = PHYSICS_BODY_DIRTY_BITS_ADD(_dirtyBits, PHYSICS_BODY_DIRTY_BITS::ANGULAR_VELOCITY);
	}
}

vec3 PhysicsBody::linearFactor() const {
	return _linearFactor;
}

void PhysicsBody::linearFactor(vec3 factor) {
	_linearFactor = factor;
	_dirtyBits = PHYSICS_BODY_DIRTY_BITS_ADD(_dirtyBits, PHYSICS_BODY_DIRTY_BITS::LINEAR_FACTOR);
}

vec3 PhysicsBody::angularFactor() const {
	return _angularFactor;
}

void PhysicsBody::angularFactor(vec3 factor) {
	_angularFactor = factor;
	_dirtyBits = PHYSICS_BODY_DIRTY_BITS_ADD(_dirtyBits, PHYSICS_BODY_DIRTY_BITS::ANGULAR_FACTOR);
}

float PhysicsBody::linearDamping() const {
	return _linearDamping;
}

void PhysicsBody::linearDamping(float damping) {
	_linearDamping = damping;
	_dirtyBits = PHYSICS_BODY_DIRTY_BITS_ADD(_dirtyBits, PHYSICS_BODY_DIRTY_BITS::LINEAR_DAMPING);
}

float PhysicsBody::angularDamping() const {
	return _angularDamping;
}

void PhysicsBody::angularDamping(float damping) {
	_angularDamping = damping;
	_dirtyBits = PHYSICS_BODY_DIRTY_BITS_ADD(_dirtyBits, PHYSICS_BODY_DIRTY_BITS::ANGULAR_DAMPING);
}

float PhysicsBody::linearSleepingThreshold() const {
	return _linearSleepingThreshold;
}

void PhysicsBody::linearSleepingThreshold(float threshold) {
	_linearSleepingThreshold = threshold;
	_dirtyBits = PHYSICS_BODY_DIRTY_BITS_ADD(_dirtyBits, PHYSICS_BODY_DIRTY_BITS::LINEAR_SLEEPING_THRESHOLD);
}

float PhysicsBody::angularSleepingThreshold() const {
	return _angularSleepingThreshold;
}

void PhysicsBody::angularSleepingThreshold(float threshold) {
	_angularSleepingThreshold = threshold;
	_dirtyBits = PHYSICS_BODY_DIRTY_BITS_ADD(_dirtyBits, PHYSICS_BODY_DIRTY_BITS::ANGULAR_SLEEPING_THRESHOLD);
}

bool PhysicsBody::affectedByGravity() const {
	return _affectedByGravity;
}

void PhysicsBody::affectedByGravity(bool flag) {
	_affectedByGravity = flag;
	_dirtyBits = PHYSICS_BODY_DIRTY_BITS_ADD(_dirtyBits, PHYSICS_BODY_DIRTY_BITS::AFFECTED_BY_GRAVITY);
}

bool PhysicsBody::allowsResting() const {
	return _allowsResting;
}

void PhysicsBody::allowsResting(bool flag) {
	_allowsResting = flag;
	_dirtyBits = PHYSICS_BODY_DIRTY_BITS_ADD(_dirtyBits, PHYSICS_BODY_DIRTY_BITS::ALLOWS_RESTING);
}

bool PhysicsBody::resting() const {
	
	return _resting;
}

void PhysicsBody::applyForce(vec3 force, bool impulse) {
	#warning fix
	
	//applyForce(force, {0, 0, 0}, impulse);
//	void 	applyCentralImpulse (const btVector3 &impulse)
//	void 	applyTorqueImpulse (const btVector3 &torque)
}

void PhysicsBody::applyForce(vec3 force, vec3 location, bool impulse) {
	
#warning save list of applied forces
	
//	if (_btRigidBody) {
//		/**********
//		if (impulse) _btRigidBody->applyImpulse(BTVector3FromGLMVec3(force), BTVector3FromGLMVec3(location));
//		else _btRigidBody->applyForce(BTVector3FromGLMVec3(force), BTVector3FromGLMVec3(location));
//		 *********/
//	}
	
	_dirtyBits = PHYSICS_BODY_DIRTY_BITS_ADD(_dirtyBits, PHYSICS_BODY_DIRTY_BITS::FORCES);
}

void PhysicsBody::applyTorque(vec3 torque, bool impulse) {
	
#warning save list of applied torques
	
//	if (_btRigidBody) {
//		/**********
//		if (impulse) _btRigidBody->applyTorqueImpulse(BTVector3FromGLMVec3(torque));
//		else  _btRigidBody->applyTorque(BTVector3FromGLMVec3(torque));
//		 *********/
//	}
	
	_dirtyBits = PHYSICS_BODY_DIRTY_BITS_ADD(_dirtyBits, PHYSICS_BODY_DIRTY_BITS::TORQUES);
}

void PhysicsBody::clearForces() {
	#warning FIX
	//if (_btRigidBody) _btRigidBody->clearForces();
}

void PhysicsBody::resetTransform() {
	#warning FIX
	//_dirtyBits = PHYSICS_BODY_DIRTY_BITS_ADD(_dirtyBits, PHYSICS_BODY_DIRTY_BITS::TRANSFORM);
	//proceedToTransform (const btTransform &newTrans)
}

/*********************************************************************************************
	Internal
 *********************************************************************************************/

void PhysicsBody::resting(bool resting) {
	_resting = resting;
	
#warning need to update BT motion state?
}

void PhysicsBody::attachedToNode(shared_ptr<Node> node) {
	if (node) {
		checkAutocreateShape(node);
	}
}

void PhysicsBody::geometryAttachedToNode(shared_ptr<Geometry> geometry) {
	if (geometry) {
		checkAutocreateShape(geometry);
	}
}

PHYSICS_BODY_DIRTY_BITS PhysicsBody::dirtyBits() const {
	return _dirtyBits;
}

void PhysicsBody::dirtyBits(PHYSICS_BODY_DIRTY_BITS bits) {
	_dirtyBits = bits;
}

/*********************************************************************************************
	Private
 *********************************************************************************************/

void PhysicsBody::checkAutocreateShape(shared_ptr<Node> node) {
	if (!_shape) {
		if (auto geometry = node->geometry()) {
			// make a shape based on the geometry
			checkAutocreateShape(geometry);
		}
		else {
			// make a shape based on the node
			if (type() == PHYSICS_BODY_TYPE::STATIC) {
				AE_LOG_D("Autocreating {} PhysicsShape for Node {:p}...",
						 magic_enum::enum_name(PHYSICS_SHAPE_TYPE::CONCAVE_POLYHEDRON), (void*)node.get());
				shape(make_shared<PhysicsShape>(PHYSICS_SHAPE_TYPE::CONCAVE_POLYHEDRON, node));
				_shape->sourceObject(node);
			} else {
				AE_LOG_D("Autocreating {} PhysicsShape for Node {:p}...",
						 magic_enum::enum_name(PHYSICS_SHAPE_TYPE::CONVEX_HULL), (void*)node.get());
				shape(make_shared<PhysicsShape>(PHYSICS_SHAPE_TYPE::CONVEX_HULL, node));
				_shape->sourceObject(node);
			}
		}
	}
}

void PhysicsBody::checkAutocreateShape(shared_ptr<Geometry> geometry) {

	if (!_shape) {
		if (type() == PHYSICS_BODY_TYPE::STATIC) {
			AE_LOG_D("Autocreating {} PhysicsShape for Geometry {:p}...",
					 magic_enum::enum_name(PHYSICS_SHAPE_TYPE::CONCAVE_POLYHEDRON), (void*)geometry.get());
			shape(make_shared<PhysicsShape>(PHYSICS_SHAPE_TYPE::CONCAVE_POLYHEDRON, geometry));
			_shape->sourceObject(geometry);
		}
		else {
			AE_LOG_D("Autocreating {} PhysicsShape for Geometry {:p}...",
					 magic_enum::enum_name(PHYSICS_SHAPE_TYPE::CONVEX_HULL), (void*)geometry.get());
			shape(make_shared<PhysicsShape>(PHYSICS_SHAPE_TYPE::CONVEX_HULL, geometry));
			_shape->sourceObject(geometry);
		}
	}
	else {
		AE_LOG_I("PhysicsBody already has a PhysicsShape.  Not auto-creating becuase of node geometry addition.");
	}
}
