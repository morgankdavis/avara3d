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
#include "physics/PhysicalWorld.h"
#include "physics/bullet/BulletBodyModel.h"
#include "physics/bullet/BulletWorldModel.h"
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
		_shape(nullptr),
		_node(nullptr),
		_world(nullptr) {

	// _node has to be initialized to nullptr before calling this
	_model = make_unique<BulletBodyModel>(this);
}

PhysicsBody::PhysicsBody(PHYSICS_BODY_TYPE type, shared_ptr<PhysicsShape> shape):
		PhysicsBody(type) {

	this->shape(shape);
}

PhysicsBody::~PhysicsBody() {
	AE_LOG_D("Destroying PhysicsBody {:p}", static_cast<void*>(this));
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
	}
}

shared_ptr<PhysicsShape> PhysicsBody::shape() const {
	return _shape;
}

void PhysicsBody::shape(shared_ptr<PhysicsShape> shape) {
	AE_LOG_T("shape: {:p}", static_cast<void*>(shape.get()));

	if (shape != _shape) {
		if (_shape) {
			_shape->detachedFromBody(this);
		}

		_shape = shape;

		if (shape) {
			shape->attachedToBody(this);
		}

		_model->shape(shape->model());
	}
}

float PhysicsBody::mass() const {
	return _model->mass();
}

void PhysicsBody::mass(float mass) {
	_model->mass(mass);
}

vec3 PhysicsBody::momentOfInertia() const {
	return _model->momentOfInertia();
}

void PhysicsBody::momentOfInertia(vec3 moment) {
	_model->momentOfInertia(moment);
}

vec3 PhysicsBody::centerOfMass() const {
	return _model->centerOfMass();
}

void PhysicsBody::centerOfMass(const glm::vec3 offset) {
	_model->centerOfMass(offset);
}

float PhysicsBody::friction() const {
	return _model->friction();
}

void PhysicsBody::friction(float friction) {
	_model->friction(friction);
}

float PhysicsBody::rollingFriction() const {
	return _model->rollingFriction();
}

void PhysicsBody::rollingFriction(float friction) {
	_model->rollingFriction(friction);
}

float PhysicsBody::restitution() const {
	return _model->restitution();
}

void PhysicsBody::restitution(float restitution) {
	_model->restitution(restitution);
}

vec3 PhysicsBody::linearVelocity() const {
	return _model->linearVelocity();
}

void PhysicsBody::linearVelocity(vec3 velocity) {
	_model->linearVelocity(velocity);
}

vec3 PhysicsBody::angularVelocity() const {
	return _model->angularVelocity();
}

void PhysicsBody::angularVelocity(vec3 velocity) {
	_model->angularVelocity(velocity);
}

vec3 PhysicsBody::linearFactor() const {
	return _model->linearFactor();
}

void PhysicsBody::linearFactor(vec3 factor) {
	_model->linearFactor(factor);
}

vec3 PhysicsBody::angularFactor() const {
	return _model->angularFactor();
}

void PhysicsBody::angularFactor(vec3 factor) {
	_model->angularFactor(factor);
}

float PhysicsBody::linearDamping() const {
	return _model->linearDamping();
}

void PhysicsBody::linearDamping(float damping) {
	_model->linearDamping(damping);
}

float PhysicsBody::angularDamping() const {
	return _model->angularDamping();
}

void PhysicsBody::angularDamping(float damping) {
	_model->angularDamping(damping);
}

float PhysicsBody::linearSleepingThreshold() const {
	return _model->linearSleepingThreshold();
}

void PhysicsBody::linearSleepingThreshold(float threshold) {
	_model->linearSleepingThreshold(threshold);
}

float PhysicsBody::angularSleepingThreshold() const {
	return _model->angularSleepingThreshold();
}

void PhysicsBody::angularSleepingThreshold(float threshold) {
	_model->angularSleepingThreshold(threshold);
}

bool PhysicsBody::affectedByGravity() const {
	return _model->affectedByGravity();
}

void PhysicsBody::affectedByGravity(bool affectedByGravity) {
	_model->affectedByGravity(affectedByGravity);
}

bool PhysicsBody::allowsResting() const {
	return _model->allowsResting();
}

void PhysicsBody::allowsResting(bool allowsResting) {
	_model->allowsResting(allowsResting);
}

bool PhysicsBody::resting() const {
	return _model->resting();
}

void PhysicsBody::resting(bool resting) {
	_model->resting(resting);
}

void PhysicsBody::applyForce(vec3 force, bool impulse) {

	if (impulse) {
		_model->applyCentralImpulse(force);
	}
	else {
		_model->applyCentralForce(force);
	}
}

void PhysicsBody::applyForce(vec3 force, vec3 location, bool impulse) {

	if (impulse) {
		_model->applyImpulse(force, location);
	}
	else {
		_model->applyForce(force, location);
	}
}

void PhysicsBody::applyTorque(vec3 torque, bool impulse) {

	if (impulse) {
		_model->applyTorqueImpulse(torque);
	}
	else {
		_model->applyTorque(torque);
	}
}

glm::vec3 PhysicsBody::totalForce() const {
	return _model->totalForce();
}

glm::vec3 PhysicsBody::totalTorque() const {
	return _model->totalTorque();
}

void PhysicsBody::clearForces() {
	_model->clearForces();
}

bool PhysicsBody::autocalculatesMomentOfInertia() const {
	return _model->autocalculatesMomentOfInertia();
}

void PhysicsBody::autocalculatesMomentOfInertia(bool autocalculate) {
	_model->autocalculatesMomentOfInertia(autocalculate);
}

/*********************************************************************************************
	Internal
 *********************************************************************************************/

void PhysicsBody::attachedToNode(Node* node) {
	AE_LOG_T("node: {:p}", static_cast<void*>(node));

	_node = node;

	checkAutocreateShape(node);

	checkAddToWorld();
}

void PhysicsBody::detachedFromNode(Node* node) {
	AE_LOG_T("node: {:p}", static_cast<void*>(node));

	if (auto world = physicalWorld()) {
		world->remove(*this);
	}
	else {
		AE_LOG_E("Attempting to remove PhysicsBody with no PhysicalWorld.");
	}

	_node = nullptr;
}

void PhysicsBody::geometryAttachedToNode(Geometry* geometry) {
	AE_LOG_T("geometry: {:p}", static_cast<void*>(geometry));

	checkAutocreateShape(geometry);
}

void PhysicsBody::geometryDetachedFromNode(Geometry* geometry) {
	AE_LOG_T("geometry: {:p}", static_cast<void*>(geometry));
}

void PhysicsBody::physicalWorldReachable(PhysicalWorld* world) {
	AE_LOG_T("world: {:p}", static_cast<void*>(world));

	if (_node->name().has_value() && _node->name() == "g duck") {
		AE_LOG_I("g duck!");
	}

	if (_shape) {
		_shape->physicalWorldReachable(world);
	}

	checkAddToWorld();
}

void PhysicsBody::physicalWorldUnreachable(PhysicalWorld* world) {
	AE_LOG_T("world: {:p}", static_cast<void*>(world));

	if (_shape) {
		_shape->physicalWorldUnreachable(world);
	}
}

void PhysicsBody::addedToWorld(PhysicalWorld* world) {
	AE_LOG_D("world: {}", static_cast<void*>(world));

	_world = world;

	// set initial transform
	_model->worldTransform(node()->worldTransform());
}

void PhysicsBody::removedFromWorld(PhysicalWorld* world) {
	AE_LOG_D("world: {}", static_cast<void*>(world));

	_world = nullptr;
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

PhysicsBodyModel* PhysicsBody::model() const {
	return _model.get();
}

/*********************************************************************************************
	Private
 *********************************************************************************************/

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
						 magic_enum::enum_name(PHYSICS_SHAPE_TYPE::CONCAVE_POLYHEDRON), static_cast<void*>(node));
				shape(make_shared<PhysicsShape>(PHYSICS_SHAPE_TYPE::CONCAVE_POLYHEDRON, node));
			}
			else {
				AE_LOG_D("Autocreating {} PhysicsShape for Node {:p}...",
						 magic_enum::enum_name(PHYSICS_SHAPE_TYPE::CONVEX_HULL), static_cast<void*>(node));
				shape(make_shared<PhysicsShape>(PHYSICS_SHAPE_TYPE::CONVEX_HULL, node));
			}
		}
	}
}

void PhysicsBody::checkAutocreateShape(Geometry* geometry) {

	if (!_shape) {
		if (type() == PHYSICS_BODY_TYPE::STATIC) {
			AE_LOG_D("Autocreating {} PhysicsShape for Geometry {:p}...",
					 magic_enum::enum_name(PHYSICS_SHAPE_TYPE::CONCAVE_POLYHEDRON), static_cast<void*>(geometry));
			shape(make_shared<PhysicsShape>(PHYSICS_SHAPE_TYPE::CONCAVE_POLYHEDRON, geometry));
		}
		else {
			AE_LOG_D("Autocreating {} PhysicsShape for Geometry {:p}...",
					 magic_enum::enum_name(PHYSICS_SHAPE_TYPE::CONVEX_HULL), static_cast<void*>(geometry));
			shape(make_shared<PhysicsShape>(PHYSICS_SHAPE_TYPE::CONVEX_HULL, geometry));
		}
	}
	else {
		AE_LOG_I("PhysicsBody already has a PhysicsShape.  Not auto-creating because of node geometry addition.");
	}
}

void PhysicsBody::checkAddToWorld() {

	if (!_world) {
		if (auto world = physicalWorld()) {
			world->add(*this);
		}
	}
}
