//
//  PhysicsBody.cc
//	avara3d
//
//  Created by Morgan Davis on 1/26/18.
//  Copyright © 2018 Morgan K Davis. All rights reserved.
//

#include "a3d/physics/PhysicsBody.h"

#include "magic_enum.hpp"

#include "a3d/diagnostic/logging/Logger.h"
#include "a3d/physics/PhysicsShape.h"
#include "a3d/physics/PhysicalWorld.h"
#include "a3d/physics/bullet/BulletBodyProxy.h"
#include "a3d/physics/bullet/BulletWorldProxy.h"
#include "a3d/scene/Node.h"
#include "a3d/scene/Scene.h"


using namespace a3d;
using namespace glm;
using namespace std;


/*********************************************************************************************
	Public Static
 *********************************************************************************************/

//shared_ptr<PhysicsBody> PhysicsBody::StaticBody() {
//	return make_shared<PhysicsBody>(PhysicsBodyType::Static);
//}
//
//shared_ptr<PhysicsBody> PhysicsBody::DynamicBody() {
//	return make_shared<PhysicsBody>(PhysicsBodyType::Dynamic);
//}
//
//shared_ptr<PhysicsBody> PhysicsBody::KinematicBody() {
//	return make_shared<PhysicsBody>(PhysicsBodyType::Kinematic);
//}

unique_ptr<PhysicsBody> PhysicsBody::StaticBody() {
	return make_unique<PhysicsBody>(PhysicsBodyType::Static);
}

unique_ptr<PhysicsBody> PhysicsBody::DynamicBody() {
	return make_unique<PhysicsBody>(PhysicsBodyType::Dynamic);
}

unique_ptr<PhysicsBody> PhysicsBody::KinematicBody() {
	return make_unique<PhysicsBody>(PhysicsBodyType::Kinematic);
}

/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

PhysicsBody::PhysicsBody(PhysicsBodyType type):
		_type{type},
		_shape{},
		_node{},
		_world{} {

	// _node has to be initialized to nullptr before calling this
	_proxy = make_unique<BulletBodyProxy>(*this);
}

PhysicsBody::PhysicsBody(PhysicsBodyType type, const shared_ptr<PhysicsShape>& shape):
		PhysicsBody(type) {

	this->shape(shape);
}

PhysicsBody::~PhysicsBody() {
	A3D_LOG_D("Destroying PhysicsBody {:p}", static_cast<void*>(this));

	if (_shape) _shape->detachedFromBody(*this);
	if (_proxy) _proxy->detachedFromBody(*this);
}

/*********************************************************************************************
	Public
 *********************************************************************************************/

PhysicsBodyType PhysicsBody::type() const {
	return _type;
}

void PhysicsBody::type(PhysicsBodyType type) {
	A3D_LOG_T("type: {}", magic_enum::enum_name(type));

	if (type != _type) {
		_type = type;
	}
}

const shared_ptr<PhysicsShape>& PhysicsBody::shape() const {
	return _shape;
}

void PhysicsBody::shape(const shared_ptr<PhysicsShape>& shape) {
	A3D_LOG_T("shape: {:p}", static_cast<void*>(shape.get()));

	if (shape != _shape) {

		if (_shape) {
			_shape->detachedFromBody(*this);
		}

		_shape = shape;

		if (shape) {
			shape->attachedToBody(*this);
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

void PhysicsBody::momentOfInertia(const vec3& moment) {
	_proxy->momentOfInertia(moment);
}

vec3 PhysicsBody::centerOfMass() const {
	return _proxy->centerOfMass();
}

void PhysicsBody::centerOfMass(const glm::vec3& offset) {
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

void PhysicsBody::linearVelocity(const vec3 velocity) {
	_proxy->linearVelocity(velocity);
}

vec3 PhysicsBody::angularVelocity() const {
	return _proxy->angularVelocity();
}

void PhysicsBody::angularVelocity(const vec3& velocity) {
	_proxy->angularVelocity(velocity);
}

vec3 PhysicsBody::linearFactor() const {
	return _proxy->linearFactor();
}

void PhysicsBody::linearFactor(const vec3& factor) {
	_proxy->linearFactor(factor);
}

vec3 PhysicsBody::angularFactor() const {
	return _proxy->angularFactor();
}

void PhysicsBody::angularFactor(const vec3& factor) {
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

void PhysicsBody::applyForce(const vec3& force, bool impulse) {

	if (impulse) {
		_proxy->applyCentralImpulse(force);
	}
	else {
		_proxy->applyCentralForce(force);
	}
}

void PhysicsBody::applyForce(const vec3& force, const vec3& location, bool impulse) {

	if (impulse) {
		_proxy->applyImpulse(force, location);
	}
	else {
		_proxy->applyForce(force, location);
	}
}

void PhysicsBody::applyTorque(const vec3& torque, bool impulse) {

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

void PhysicsBody::attachedToNode(const shared_ptr<Node>& node) {
	A3D_LOG_T("node: {:p}", static_cast<void*>(node.get()));

	_node = node;

	checkAutocreateShape(node);

	checkAddToWorld();
}

void PhysicsBody::detachedFromNode(const std::shared_ptr<Node>& node) {
	A3D_LOG_T("node: {:p}", static_cast<void*>(node.get()));

	// PhysicalWorld::remove() handled in physicalWorldUnreachable()

	_node = {}; // ^^ physicalWorld() relies on old _node
}

void PhysicsBody::meshAttachedToNode(const shared_ptr<Mesh>& mesh) {
	A3D_LOG_T("mesh: {:p}", static_cast<void*>(mesh.get()));

	checkAutocreateShape(mesh);
}

void PhysicsBody::meshDetachedFromNode(const shared_ptr<Mesh>& mesh) {
	A3D_LOG_T("mesh: {:p}", static_cast<void*>(mesh.get()));
}

void PhysicsBody::physicalWorldReachable(PhysicalWorld& world) {
	A3D_LOG_T("world: {:p}", static_cast<void*>(&world));

	if (_shape) {
		_shape->physicalWorldReachable(world);
	}

	checkAddToWorld();
}

void PhysicsBody::physicalWorldUnreachable(PhysicalWorld& world) {
	A3D_LOG_T("world: {:p}", static_cast<void*>(&world));

	if (_shape) {
		_shape->physicalWorldUnreachable(world);
	}

	// if _world != nullptr it's the world we're currently in
	 if (_world) {
		_world->remove(*this);
	}
}

void PhysicsBody::addedToWorld(PhysicalWorld& world) {
	A3D_LOG_D("world: {}", static_cast<void*>(&world));

	_world = &world;

	if (auto node = _node.lock()) {
		// set initial transform
		_proxy->worldTransform(node->worldTransform());
	}
	else {
		A3D_LOG_E("_node is null.");
	}
}

void PhysicsBody::removedFromWorld(PhysicalWorld& world) {
	A3D_LOG_D("world: {}", static_cast<void*>(&world));

	_world = nullptr;
}

void PhysicsBody::shapeUpdated() {

	if (_proxy) {
		if (_shape) {
				_proxy->shapeProxy(_shape->proxy());
		}
		else {
			A3D_LOG_E("No shape.");
		}
	}
	else {
		A3D_LOG_E("No body model proxy.");
	}
}

weak_ptr<Node> PhysicsBody::node() const {
	return _node;
}

PhysicalWorld* PhysicsBody::physicalWorld() const {

	if (auto node = _node.lock()) {
		if (auto scene = node->scene()) {
			if (auto physicalWorld = scene->physicalWorld(); physicalWorld) {
				return physicalWorld;
			}
		}
	}
	else {
		A3D_LOG_E("_node is null.");
	}
	return nullptr;
}

PhysicsBodyProxy* PhysicsBody::proxy() const {
	return _proxy.get();
}

/*********************************************************************************************
	Private
 *********************************************************************************************/

void PhysicsBody::checkAutocreateShape(const shared_ptr<Node>& node) {

	if (!_shape) {
//		if (auto sNode = node.lock()) {
			if (auto mesh = node->mesh()) {
				// make a shape based on the mesh
				checkAutocreateShape(mesh);
			}
			else {
				// make a shape based on the node
				auto shapeType = PhysicsShapeType::ConcavePolyhedron;
				if (type() == PhysicsBodyType::Static) {
					A3D_LOG_D("Autocreating {} PhysicsShape for Node {:p}...",
							  magic_enum::enum_name(shapeType), static_cast<void *>(node.get()));
					shape(make_shared<PhysicsShape>(shapeType, node));
				}
				else {
					auto shapeType = PhysicsShapeType::ConvexHull;
					A3D_LOG_D("Autocreating {} PhysicsShape for Node {:p}...",
							  magic_enum::enum_name(shapeType), static_cast<void *>(node.get()));
					shape(make_shared<PhysicsShape>(shapeType, node));
				}
			}
//		}
//		else {
//			throw std::bad_weak_ptr();
//		}
	}
}

void PhysicsBody::checkAutocreateShape(const shared_ptr<Mesh>& mesh) {

	if (!_shape) {
//		if (auto sMesh = mesh.lock()) {
			if (type() == PhysicsBodyType::Static) {
				auto shapeType = PhysicsShapeType::ConcavePolyhedron;
				A3D_LOG_D("Autocreating {} PhysicsShape for Mesh {:p}...",
						  magic_enum::enum_name(shapeType), static_cast<void *>(mesh.get()));
				shape(make_shared<PhysicsShape>(shapeType, mesh));
			}
			else {
				auto shapeType = PhysicsShapeType::ConvexHull;
				A3D_LOG_D("Autocreating {} PhysicsShape for Mesh {:p}...",
						  magic_enum::enum_name(shapeType), static_cast<void *>(mesh.get()));
				shape(make_shared<PhysicsShape>(shapeType, mesh));
			}
//		}
//		else {
//			throw std::bad_weak_ptr();
//		}
	}
	else {
		A3D_LOG_I("PhysicsBody already has a PhysicsShape.  Not auto-creating because of node mesh addition.");
	}
}

void PhysicsBody::checkAddToWorld() {

	if (!_world) {
		if (auto world = physicalWorld()) {
			world->add(*this);
		}
	}
}
