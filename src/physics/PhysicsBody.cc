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
#include "physics/bullet/BulletBodyProxy.h"
#include "physics/bullet/BulletWorldProxy.h"
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
	_proxy = make_unique<BulletBodyProxy>(this);
}

PhysicsBody::PhysicsBody(PHYSICS_BODY_TYPE type, shared_ptr<PhysicsShape> shape):
		PhysicsBody(type) {

	this->shape(shape);
}

PhysicsBody::~PhysicsBody() {
	AE_LOG_D("Destroying PhysicsBody {:p}", static_cast<void*>(this));

	if (_shape) _shape->detachedFromBody(this);
	if (_proxy) _proxy->detachedFromBody(this);
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
			_proxy->shapeProxy(shape->proxy());
		}
		else {
			_proxy->shapeProxy(nullptr);
		}
	}
}

float PhysicsBody::mass() const {
	return _proxy->mass();
}

void PhysicsBody::mass(float mass) {
	_proxy->mass(mass);
}

vec3 PhysicsBody::momentOfInertia() const {
	return _proxy->momentOfInertia();
}

void PhysicsBody::momentOfInertia(vec3 moment) {
	_proxy->momentOfInertia(moment);
}

vec3 PhysicsBody::centerOfMass() const {
	return _proxy->centerOfMass();
}

void PhysicsBody::centerOfMass(const glm::vec3 offset) {
	_proxy->centerOfMass(offset);
}

float PhysicsBody::friction() const {
	return _proxy->friction();
}

void PhysicsBody::friction(float friction) {
	_proxy->friction(friction);
}

float PhysicsBody::rollingFriction() const {
	return _proxy->rollingFriction();
}

void PhysicsBody::rollingFriction(float friction) {
	_proxy->rollingFriction(friction);
}

float PhysicsBody::restitution() const {
	return _proxy->restitution();
}

void PhysicsBody::restitution(float restitution) {
	_proxy->restitution(restitution);
}

vec3 PhysicsBody::linearVelocity() const {
	return _proxy->linearVelocity();
}

void PhysicsBody::linearVelocity(vec3 velocity) {
	_proxy->linearVelocity(velocity);
}

vec3 PhysicsBody::angularVelocity() const {
	return _proxy->angularVelocity();
}

void PhysicsBody::angularVelocity(vec3 velocity) {
	_proxy->angularVelocity(velocity);
}

vec3 PhysicsBody::linearFactor() const {
	return _proxy->linearFactor();
}

void PhysicsBody::linearFactor(vec3 factor) {
	_proxy->linearFactor(factor);
}

vec3 PhysicsBody::angularFactor() const {
	return _proxy->angularFactor();
}

void PhysicsBody::angularFactor(vec3 factor) {
	_proxy->angularFactor(factor);
}

float PhysicsBody::linearDamping() const {
	return _proxy->linearDamping();
}

void PhysicsBody::linearDamping(float damping) {
	_proxy->linearDamping(damping);
}

float PhysicsBody::angularDamping() const {
	return _proxy->angularDamping();
}

void PhysicsBody::angularDamping(float damping) {
	_proxy->angularDamping(damping);
}

float PhysicsBody::linearSleepingThreshold() const {
	return _proxy->linearSleepingThreshold();
}

void PhysicsBody::linearSleepingThreshold(float threshold) {
	_proxy->linearSleepingThreshold(threshold);
}

float PhysicsBody::angularSleepingThreshold() const {
	return _proxy->angularSleepingThreshold();
}

void PhysicsBody::angularSleepingThreshold(float threshold) {
	_proxy->angularSleepingThreshold(threshold);
}

bool PhysicsBody::affectedByGravity() const {
	return _proxy->affectedByGravity();
}

void PhysicsBody::affectedByGravity(bool affectedByGravity) {
	_proxy->affectedByGravity(affectedByGravity);
}

bool PhysicsBody::allowsResting() const {
	return _proxy->allowsResting();
}

void PhysicsBody::allowsResting(bool allowsResting) {
	_proxy->allowsResting(allowsResting);
}

bool PhysicsBody::resting() const {
	return _proxy->resting();
}

void PhysicsBody::resting(bool resting) {
	_proxy->resting(resting);
}

void PhysicsBody::applyForce(vec3 force, bool impulse) {

	if (impulse) {
		_proxy->applyCentralImpulse(force);
	}
	else {
		_proxy->applyCentralForce(force);
	}
}

void PhysicsBody::applyForce(vec3 force, vec3 location, bool impulse) {

	if (impulse) {
		_proxy->applyImpulse(force, location);
	}
	else {
		_proxy->applyForce(force, location);
	}
}

void PhysicsBody::applyTorque(vec3 torque, bool impulse) {

	if (impulse) {
		_proxy->applyTorqueImpulse(torque);
	}
	else {
		_proxy->applyTorque(torque);
	}
}

glm::vec3 PhysicsBody::totalForce() const {
	return _proxy->totalForce();
}

glm::vec3 PhysicsBody::totalTorque() const {
	return _proxy->totalTorque();
}

void PhysicsBody::clearForces() {
	_proxy->clearForces();
}

bool PhysicsBody::autocalculatesMomentOfInertia() const {
	return _proxy->autocalculatesMomentOfInertia();
}

void PhysicsBody::autocalculatesMomentOfInertia(bool autocalculate) {
	_proxy->autocalculatesMomentOfInertia(autocalculate);
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

	// PhysicalWorld::remove() handled in physicalWorldUnreachable()

	_node = nullptr; // ^^ physicalWorld() relies on old _node
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

	// if _world != nullptr it's the world we're currently in
	 if (_world) {
		_world->remove(*this);
	}
}

void PhysicsBody::addedToWorld(PhysicalWorld* world) {
	AE_LOG_D("world: {}", static_cast<void*>(world));

	_world = world;

	// set initial transform
	_proxy->worldTransform(node()->worldTransform());
}

void PhysicsBody::removedFromWorld(PhysicalWorld* world) {
	AE_LOG_D("world: {}", static_cast<void*>(world));

	_world = nullptr;
}

void PhysicsBody::shapeUpdated() {

	if (_proxy) {
		if (_shape) {
			auto existingShapeProxy = _proxy->shapeProxy();
			auto newShapeProxy = _shape->proxy();
			if (existingShapeProxy != newShapeProxy) {
				_proxy->shapeProxy(newShapeProxy);
			}
		}
		else {
			AE_LOG_E("No shape.");
		}
	}
	else {
		AE_LOG_E("No body model proxy.");
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

PhysicsBodyModelProxy* PhysicsBody::proxy() const {
	return _proxy.get();
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
			auto shapeType = PHYSICS_SHAPE_TYPE::CONCAVE_POLYHEDRON;
			if (type() == PHYSICS_BODY_TYPE::STATIC) {
				AE_LOG_D("Autocreating {} PhysicsShape for Node {:p}...",
						 magic_enum::enum_name(shapeType), static_cast<void*>(node));
				shape(make_shared<PhysicsShape>(shapeType, node));
			}
			else {
				auto shapeType = PHYSICS_SHAPE_TYPE::CONVEX_HULL;
				AE_LOG_D("Autocreating {} PhysicsShape for Node {:p}...",
						 magic_enum::enum_name(shapeType), static_cast<void*>(node));
				shape(make_shared<PhysicsShape>(shapeType, node));
			}
		}
	}
}

void PhysicsBody::checkAutocreateShape(Geometry* geometry) {

	if (!_shape) {
		if (type() == PHYSICS_BODY_TYPE::STATIC) {
			auto shapeType = PHYSICS_SHAPE_TYPE::CONCAVE_POLYHEDRON;
			AE_LOG_D("Autocreating {} PhysicsShape for Geometry {:p}...",
					 magic_enum::enum_name(shapeType), static_cast<void*>(geometry));
			shape(make_shared<PhysicsShape>(shapeType, geometry));
		}
		else {
			auto shapeType = PHYSICS_SHAPE_TYPE::CONVEX_HULL;
			AE_LOG_D("Autocreating {} PhysicsShape for Geometry {:p}...",
					 magic_enum::enum_name(shapeType), static_cast<void*>(geometry));
			shape(make_shared<PhysicsShape>(shapeType, geometry));
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
