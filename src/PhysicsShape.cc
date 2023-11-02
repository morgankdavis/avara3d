//
//  PhysicsShape.cc
//	avara-engine
//
//  Created by Morgan Davis on 1/25/18.
//  Copyright © 2018 Morgan K Davis. All rights reserved.
//

#include "PhysicsShape.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <magic_enum.hpp>

#include "Box.h"
#include "Capsule.h"
#include "Cone.h"
#include "Cylinder.h"
#include "Geometry.h"
#include "GeometryElement.h"
#include "Logger.h"
#include "Node.h"
#include "PhysicsBody.h"
#include "Plane.h"
#include "Sphere.h"
#include "Utilities.h"


using namespace ae;
using namespace ae::utils;
using namespace glm;
using namespace std;


/*********************************************************************************************
	Public Static
 *********************************************************************************************/

shared_ptr<PhysicsShape> PhysicsShape::BoundingBoxShape() {
	return make_shared<PhysicsShape>(PHYSICS_SHAPE_TYPE::BOUNDING_BOX);
}

shared_ptr<PhysicsShape> PhysicsShape::ConvexHullShape() {
	return make_shared<PhysicsShape>(PHYSICS_SHAPE_TYPE::CONVEX_HULL);
}

shared_ptr<PhysicsShape> PhysicsShape::ConcavePolyhedronShape() {
	return make_shared<PhysicsShape>(PHYSICS_SHAPE_TYPE::CONCAVE_POLYHEDRON);
}

/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

PhysicsShape::PhysicsShape(PHYSICS_SHAPE_TYPE type):
		_sourceGeometry({}),
		_sourceNode({}),
		_type(type),
		_transforms(vector<mat4>()),
		_physicsBody({}),
		_dirtyBits(PHYSICS_SHAPE_DIRTY_BITS::ALL) {

	AE_LOG_D("Creating PhysicsShape type {}...",
			 magic_enum::enum_name(type));
}

PhysicsShape::PhysicsShape(PHYSICS_SHAPE_TYPE type, shared_ptr<Geometry> geometry):
		_sourceGeometry({}),
		_sourceNode({}),
		_type(type),
		_transforms(vector<mat4>()),
		_physicsBody({}),
		_dirtyBits(PHYSICS_SHAPE_DIRTY_BITS::ALL) {

	if (auto name = geometry->name()) {
		AE_LOG_D("Creating PhysicsShape type {} for source geometry: {}...",
				 magic_enum::enum_name(type), *name);
	}
	else {
		AE_LOG_D("Creating PhysicsShape type {} for source geometry: {:p}...",
				 magic_enum::enum_name(type), (void*)geometry.get());
	}

	sourceGeometry(geometry);
}

// construct a compound shape based on geometries under this node
PhysicsShape::PhysicsShape(PHYSICS_SHAPE_TYPE type, shared_ptr<Node> node):
		_sourceGeometry({}),
		_sourceNode({}),
		_type(type),
		_transforms(vector<mat4>()),
		_physicsBody({}),
		_dirtyBits(PHYSICS_SHAPE_DIRTY_BITS::ALL) {

	if (auto name = node->name()) {
		AE_LOG_D("Creating PhysicsShape type {} for source node: {}...",
				 magic_enum::enum_name(type), *name);
	}
	else {
		AE_LOG_D("Creating PhysicsShape type {} for source node: {:p}...",
				 magic_enum::enum_name(type), (void*)node.get());
	}

	sourceNode(node);
}

PhysicsShape::~PhysicsShape() {
	AE_LOG_D("Destroying PhysicsShape {:p}", (void*)this);
}

/*********************************************************************************************
	Public
 *********************************************************************************************/

weak_ptr<Geometry> PhysicsShape::sourceGeometry() const {
	return _sourceGeometry;
}

weak_ptr<Node> PhysicsShape::sourceNode() const {
	return _sourceNode;
}

PHYSICS_SHAPE_TYPE PhysicsShape::type() const {
	return _type;
}

void PhysicsShape::type(PHYSICS_SHAPE_TYPE type) {
	_type = type;
	_dirtyBits = PHYSICS_SHAPE_DIRTY_BITS_ADD(_dirtyBits, PHYSICS_SHAPE_DIRTY_BITS::MODEL);
}

/* ? */ vector<glm::mat4> PhysicsShape::transforms() const {
	return _transforms;
}

/*********************************************************************************************
	Internal
 *********************************************************************************************/

void PhysicsShape::sourceGeometry(std::weak_ptr<Geometry> geometry) {
	_sourceGeometry = geometry;
	
	_dirtyBits = PHYSICS_SHAPE_DIRTY_BITS_ADD(_dirtyBits, PHYSICS_SHAPE_DIRTY_BITS::MODEL);
}

void PhysicsShape::sourceNode(std::weak_ptr<Node> node) {
	_sourceNode = node;
	
	_dirtyBits = PHYSICS_SHAPE_DIRTY_BITS_ADD(_dirtyBits, PHYSICS_SHAPE_DIRTY_BITS::MODEL);
}

void PhysicsShape::attachedToBody(shared_ptr<PhysicsBody> body) {
	_physicsBody = body;
	_sourceNode = body->node();

	if (auto node = _sourceNode.lock()) {
		if (auto geometry = node->geometry()) {
			sourceGeometry(geometry);
		}
	}
}

//vector<shared_ptr<btCollisionShape>>& PhysicsShape::childShapes() {
//	return _childShapes;
//}

weak_ptr<PhysicsBody> PhysicsShape::physicsBody() const {
	return _physicsBody;
}

void PhysicsShape::physicsBody(shared_ptr<PhysicsBody> body) {
	_physicsBody = body;
	attachedToBody(body);
}

PHYSICS_SHAPE_DIRTY_BITS PhysicsShape::dirtyBits() const {
	return _dirtyBits;
}

void PhysicsShape::dirtyBits(PHYSICS_SHAPE_DIRTY_BITS bits) {
	_dirtyBits = bits;
}
