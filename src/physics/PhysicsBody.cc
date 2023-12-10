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
#include "physics/PhysicsSimulator.h"
#include "physics/PhysicalWorld.h"
#include "physics/bullet/BulletBodyResources.h"
#include "scene/Node.h"
#include "utilities/Utilities.h"


using namespace ae;
using namespace ae::utils;
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
		_node(nullptr),
		_dirtyMask(PHYSICS_BODY_DIRTY_MASK::ALL),
		_resources(make_shared<BulletBodyResources>()){ }

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
	AE_LOG_T("type: {}", magic_enum::enum_name(type));

	if (type != _type) {

		_type = type;
		_dirtyMask = PHYSICS_BODY_DIRTY_MASK_ADD(_dirtyMask, PHYSICS_BODY_DIRTY_MASK::TYPE);
	}
}

shared_ptr<PhysicsShape> PhysicsBody::shape() const {
	return _shape;
}

void PhysicsBody::shape(shared_ptr<PhysicsShape> shape) {
	AE_LOG_T("shape: {:p}", (void*)shape.get());

//	if (shape != _shape) {

		if (_shape) {
			_shape->detachedFromBody(this);
		}

		_shape = shape;
		_dirtyMask = PHYSICS_BODY_DIRTY_MASK_ADD(_dirtyMask, PHYSICS_BODY_DIRTY_MASK::SHAPE);

		if (shape) {
			shape->attachedToBody(this);
		}
//	}
}

float PhysicsBody::mass() const {
	return _mass;
}

void PhysicsBody::mass(float mass) {

	_mass = mass;
	_dirtyMask = PHYSICS_BODY_DIRTY_MASK_ADD(_dirtyMask, PHYSICS_BODY_DIRTY_MASK::MASS);
}

vec3 PhysicsBody::momentOfInertia() const {
	return _momentOfInertia;
}

void PhysicsBody::momentOfInertia(vec3 moment) {
	_momentOfInertia = moment;
	_dirtyMask = PHYSICS_BODY_DIRTY_MASK_ADD(_dirtyMask, PHYSICS_BODY_DIRTY_MASK::MOMENT_OF_INERTIA);
}

float PhysicsBody::friction() const {
	return _friction;
}

void PhysicsBody::friction(float friction) {
	_friction = friction;
	_dirtyMask = PHYSICS_BODY_DIRTY_MASK_ADD(_dirtyMask, PHYSICS_BODY_DIRTY_MASK::FRICTION);
}

float PhysicsBody::rollingFriction() const {
	return _rollingFriction;
}

void PhysicsBody::rollingFriction(float friction) {
	_rollingFriction = friction;
	_dirtyMask = PHYSICS_BODY_DIRTY_MASK_ADD(_dirtyMask, PHYSICS_BODY_DIRTY_MASK::ROLLING_FRICTION);
}

float PhysicsBody::restitution() const {
	return _restitution;
}

void PhysicsBody::restitution(float restitution) {
	_restitution = restitution;
	_dirtyMask = PHYSICS_BODY_DIRTY_MASK_ADD(_dirtyMask, PHYSICS_BODY_DIRTY_MASK::RESTITUTION);
}

vec3 PhysicsBody::linearVelocity() const {
	return _linearVelocity;
}

void PhysicsBody::linearVelocity(vec3 velocity, bool setDirty) {
	_linearVelocity = velocity;
	if (setDirty) {
		_dirtyMask = PHYSICS_BODY_DIRTY_MASK_ADD(_dirtyMask, PHYSICS_BODY_DIRTY_MASK::LINEAR_VELOCITY);
	}
}

vec3 PhysicsBody::angularVelocity() const {
	return _angularVelocity;
}

void PhysicsBody::angularVelocity(vec3 velocity, bool setDirty) {
	_angularVelocity = velocity;
	if (setDirty) {
		_dirtyMask = PHYSICS_BODY_DIRTY_MASK_ADD(_dirtyMask, PHYSICS_BODY_DIRTY_MASK::ANGULAR_VELOCITY);
	}
}

vec3 PhysicsBody::linearFactor() const {
	return _linearFactor;
}

void PhysicsBody::linearFactor(vec3 factor) {
	_linearFactor = factor;
	_dirtyMask = PHYSICS_BODY_DIRTY_MASK_ADD(_dirtyMask, PHYSICS_BODY_DIRTY_MASK::LINEAR_FACTOR);
}

vec3 PhysicsBody::angularFactor() const {
	return _angularFactor;
}

void PhysicsBody::angularFactor(vec3 factor) {
	_angularFactor = factor;
	_dirtyMask = PHYSICS_BODY_DIRTY_MASK_ADD(_dirtyMask, PHYSICS_BODY_DIRTY_MASK::ANGULAR_FACTOR);
}

float PhysicsBody::linearDamping() const {
	return _linearDamping;
}

void PhysicsBody::linearDamping(float damping) {
	_linearDamping = damping;
	_dirtyMask = PHYSICS_BODY_DIRTY_MASK_ADD(_dirtyMask, PHYSICS_BODY_DIRTY_MASK::LINEAR_DAMPING);
}

float PhysicsBody::angularDamping() const {
	return _angularDamping;
}

void PhysicsBody::angularDamping(float damping) {
	_angularDamping = damping;
	_dirtyMask = PHYSICS_BODY_DIRTY_MASK_ADD(_dirtyMask, PHYSICS_BODY_DIRTY_MASK::ANGULAR_DAMPING);
}

float PhysicsBody::linearSleepingThreshold() const {
	return _linearSleepingThreshold;
}

void PhysicsBody::linearSleepingThreshold(float threshold) {
	_linearSleepingThreshold = threshold;
	_dirtyMask = PHYSICS_BODY_DIRTY_MASK_ADD(_dirtyMask, PHYSICS_BODY_DIRTY_MASK::LINEAR_SLEEPING_THRESHOLD);
}

float PhysicsBody::angularSleepingThreshold() const {
	return _angularSleepingThreshold;
}

void PhysicsBody::angularSleepingThreshold(float threshold) {
	_angularSleepingThreshold = threshold;
	_dirtyMask = PHYSICS_BODY_DIRTY_MASK_ADD(_dirtyMask, PHYSICS_BODY_DIRTY_MASK::ANGULAR_SLEEPING_THRESHOLD);
}

bool PhysicsBody::affectedByGravity() const {
	return _affectedByGravity;
}

void PhysicsBody::affectedByGravity(bool flag) {
	_affectedByGravity = flag;
	_dirtyMask = PHYSICS_BODY_DIRTY_MASK_ADD(_dirtyMask, PHYSICS_BODY_DIRTY_MASK::AFFECTED_BY_GRAVITY);
}

bool PhysicsBody::allowsResting() const {
	return _allowsResting;
}

void PhysicsBody::allowsResting(bool flag) {
	_allowsResting = flag;
	_dirtyMask = PHYSICS_BODY_DIRTY_MASK_ADD(_dirtyMask, PHYSICS_BODY_DIRTY_MASK::ALLOWS_RESTING);
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

	_dirtyMask = PHYSICS_BODY_DIRTY_MASK_ADD(_dirtyMask, PHYSICS_BODY_DIRTY_MASK::FORCES);
}

void PhysicsBody::applyTorque(vec3 torque, bool impulse) {

#warning save list of applied torques

//	if (_btRigidBody) {
//		/**********
//		if (impulse) _btRigidBody->applyTorqueImpulse(BTVector3FromGLMVec3(torque));
//		else  _btRigidBody->applyTorque(BTVector3FromGLMVec3(torque));
//		 *********/
//	}

	_dirtyMask = PHYSICS_BODY_DIRTY_MASK_ADD(_dirtyMask, PHYSICS_BODY_DIRTY_MASK::TORQUES);
}

void PhysicsBody::clearForces() {
	#warning FIX
	//if (_btRigidBody) _btRigidBody->clearForces();
}

void PhysicsBody::resetTransform() {
	#warning FIX
	//_dirtyMask = PHYSICS_BODY_DIRTY_MASK_ADD(_dirtyMask, PHYSICS_BODY_DIRTY_MASK::TRANSFORM);
	//proceedToTransform (const btTransform &newTrans)
}

/*********************************************************************************************
	Internal
 *********************************************************************************************/

void PhysicsBody::resting(bool resting) {
	_resting = resting;
#warning probably want to actually DO something with this...
}

void PhysicsBody::attachedToNode(Node* node) {
	AE_LOG_T("node: {:p}", (void*)node);

	_node = node;

	checkAutocreateShape(node);

	checkCreateModel();

//	if (auto world = physicalWorld()) {
//
////		_shape->update(simulator,
////					   node,
////					   *this,
////					   stats);
////
////		simulator.update(*this,
////						 node);
//
//		world->simulator()->create(*this);
//	}
//	else {
//		AE_LOG_I("No reachable PhysicalWorld.");
//	}
}

void PhysicsBody::detachedFromNode(Node* node) {
	AE_LOG_T("node: {:p}", (void*)node);

	if (auto world = physicalWorld()) {
		world->simulator()->remove(*this);
	}
	else {
		AE_LOG_E("Attempting to remove PhysicsBody with no PhysicalWorld.");
	}

	_node = nullptr;
}

//void PhysicsBody::nodeAttachedToParent(Node* parent) {
//
//}
//
//void PhysicsBody::nodeDetachedFromParent(Node* parent) {
//
//}
//
//void PhysicsBody::nodeAttachedToScene(Scene* scene) {
//
//}
//
//void PhysicsBody::nodeDetachedFromScene(Scene* scene) {
//
//}

void PhysicsBody::geometryAttachedToNode(Geometry* geometry) {
	AE_LOG_T("geometry: {:p}", (void*)geometry);

	checkAutocreateShape(geometry);
}

void PhysicsBody::geometryDetachedFromNode(Geometry* geometry) {
	AE_LOG_T("geometry: {:p}", (void*)geometry);
}

void PhysicsBody::physicalWorldReachable(PhysicalWorld* world) {
	AE_LOG_T("world: {:p}", (void*)world);

	if (_node->name().has_value() && _node->name() == "g duck") {
		AE_LOG_I("g duck!");
	}

	if (_shape) {
		_shape->physicalWorldReachable(world);
	}

	checkCreateModel();
//	if (!_resources) { // meh?

//	}
}

void PhysicsBody::physicalWorldUnreachable(PhysicalWorld* world) {
	AE_LOG_T("world: {:p}", (void*)world);

	if (_shape) {
		_shape->physicalWorldUnreachable(world);
	}
}

//void PhysicsBody::ancestorAttachedToParent(Node* ancestor,
//										   Node* parent) {
//
//}
//
//void PhysicsBody::ancestorDetachedFromParent(Node* ancestor,
//											 Node* parent) {
//
//}
//
//void PhysicsBody::ancestorAttachedToScene(Node* ancestor,
//										  Scene* scene) {
//
//}
//
//void PhysicsBody::ancestorDetachedFromScene(Node* ancestor,
//											Scene* scene) {
//
//}
//
//void PhysicsBody::physicalWorldAttachedToScene(PhysicalWorld* world,
//											   Scene* scene) {
//
//}
//
//void PhysicsBody::physicalWorldDetachedFromScene(PhysicalWorld* world,
//												 Scene* scene) {
//
//}

void PhysicsBody::worldTransformUpdated(const glm::mat4& transform) {
	//AE_LOG_I("transform: {}", utils::StringFromGLMMat4(transform));

	if (auto simulator = physicsSimulator()) {
		simulator->setWorldTransform(*this, transform);
	}
}

void PhysicsBody::modelCreated(PhysicsShape& shape) {
	AE_LOG_I("shape: {:p}", (void*)&shape);

#warning this may be redundant -- CHECK

	if (auto simulator = physicsSimulator()) {
		simulator->setShape(*this, shape);
	}
}

Node* PhysicsBody::node() const {
	return _node;
}


PhysicalWorld* PhysicsBody::physicalWorld() const {

	if (_node) {
		if (auto scene = _node->scene()) {
			if (auto physicalWorld = scene->physicalWorld()) {
				return physicalWorld.get();
			}
		}
	}
	return nullptr;
}

PhysicsSimulator* PhysicsBody::physicsSimulator() const {

	if (auto world = physicalWorld()) {
		return world->simulator();
	}

	return nullptr;
}

PhysicsBodyResources* PhysicsBody::resources() {
	return _resources.get();
}

//void PhysicsBody::update(PhysicsSimulator& simulator,
//						 Node& node,
//						 Stats& stats) {
//
//	_shape->update(simulator,
//				   node,
//				   *this,
//				   stats);
//
//	simulator.update(*this,
//					 node);
//}

void PhysicsBody::sync(PhysicsSimulator& simulator,
					   Node& node,
					   mat4& localTransform,
					   Stats& stats) {

//	_shape->sync(simulator,
//				   node,
//				   *this,
//				   stats);

	simulator.sync(*this,
				   localTransform);

	switch (type()) {
		case (PHYSICS_BODY_TYPE::DYNAMIC): ++stats.dynamicBodies; break;
		case (PHYSICS_BODY_TYPE::KINEMATIC): ++stats.kinematicBodies; break;
		case (PHYSICS_BODY_TYPE::STATIC): ++stats.staticBodies; break;
	}
}

PHYSICS_BODY_DIRTY_MASK PhysicsBody::dirtyMask() const {
	return _dirtyMask;
}

void PhysicsBody::dirtyMask(PHYSICS_BODY_DIRTY_MASK mask) {
	_dirtyMask = mask;
}

/*********************************************************************************************
	Private
 *********************************************************************************************/

void PhysicsBody::checkCreateModel() {
	AE_LOG_T("");

//	if (_resources) {
		if (auto simulator = physicsSimulator()) {
			simulator->create(*this);

//			for (auto& body : _bodies) {
//				body->modelCreated(*this);
//			}
		}
//	}
}

void PhysicsBody::checkAutocreateShape(Node* node) {
	if (!_shape) {
		if (auto geometry = node->geometry()) {
			// make a shape based on the geometry
			checkAutocreateShape(geometry.get());
		}
		else {
			// make a shape based on the node
			if (type() == PHYSICS_BODY_TYPE::STATIC) {
				AE_LOG_D("Autocreating {} PhysicsShape for Node {:p}...",
						 magic_enum::enum_name(PHYSICS_SHAPE_TYPE::CONCAVE_POLYHEDRON), (void*)node);
				shape(make_shared<PhysicsShape>(PHYSICS_SHAPE_TYPE::CONCAVE_POLYHEDRON, node));
//				_shape->sourceObject(node);
			}
			else {
				AE_LOG_D("Autocreating {} PhysicsShape for Node {:p}...",
						 magic_enum::enum_name(PHYSICS_SHAPE_TYPE::CONVEX_HULL), (void*)node);
				shape(make_shared<PhysicsShape>(PHYSICS_SHAPE_TYPE::CONVEX_HULL, node));
//				_shape->sourceObject(node);
			}
		}
	}
}

void PhysicsBody::checkAutocreateShape(Geometry* geometry) {

	if (!_shape) {
		if (type() == PHYSICS_BODY_TYPE::STATIC) {
			AE_LOG_D("Autocreating {} PhysicsShape for Geometry {:p}...",
					 magic_enum::enum_name(PHYSICS_SHAPE_TYPE::CONCAVE_POLYHEDRON), (void*)geometry);
			shape(make_shared<PhysicsShape>(PHYSICS_SHAPE_TYPE::CONCAVE_POLYHEDRON, geometry));
//			_shape->sourceObject(geometry);
		}
		else {
			AE_LOG_D("Autocreating {} PhysicsShape for Geometry {:p}...",
					 magic_enum::enum_name(PHYSICS_SHAPE_TYPE::CONVEX_HULL), (void*)geometry);
			shape(make_shared<PhysicsShape>(PHYSICS_SHAPE_TYPE::CONVEX_HULL, geometry));
//			_shape->sourceObject(geometry);
		}
	}
	else {
		AE_LOG_I("PhysicsBody already has a PhysicsShape.  Not auto-creating becuase of node geometry addition.");
	}
}
