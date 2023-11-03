//
//  PhysicsShape.cc
//	avara-engine
//
//  Created by Morgan Davis on 1/25/18.
//  Copyright © 2018 Morgan K Davis. All rights reserved.
//

#include "PhysicsShape.h"

#include <glm/gtc/type_ptr.hpp>
#include <magic_enum.hpp>

#include "Geometry.h"
#include "Logger.h"
#include "Node.h"
#include "PhysicsBody.h"


using namespace ae;
using namespace glm;
using namespace std;


/*********************************************************************************************
	Public Static
 *********************************************************************************************/

//shared_ptr<PhysicsShape> PhysicsShape::BoundingBoxShape() {
//	return make_shared<PhysicsShape>(PHYSICS_SHAPE_TYPE::BOUNDING_BOX);
//}
//
//shared_ptr<PhysicsShape> PhysicsShape::ConvexHullShape() {
//	return make_shared<PhysicsShape>(PHYSICS_SHAPE_TYPE::CONVEX_HULL);
//}
//
//shared_ptr<PhysicsShape> PhysicsShape::ConcavePolyhedronShape() {
//	return make_shared<PhysicsShape>(PHYSICS_SHAPE_TYPE::CONCAVE_POLYHEDRON);
//}

/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

//PhysicsShape::PhysicsShape(PHYSICS_SHAPE_TYPE type):
////		_sourceGeometry({}),
////		_sourceNode({}),
//		_type(type),
//		_dirtyBits(PHYSICS_SHAPE_DIRTY_BITS::ALL) {
//
//	AE_LOG_D("Creating PhysicsShape type {}...",
//			 magic_enum::enum_name(type));
//}

PhysicsShape::PhysicsShape(PHYSICS_SHAPE_TYPE type, shared_ptr<Geometry> geometry):
//		_sourceGeometry({}),
//		_sourceNode({}),
		_sourceObject(geometry),
		_type(type),
		_dirtyBits(PHYSICS_SHAPE_DIRTY_BITS::ALL) {

	if (auto name = geometry->name()) {
		AE_LOG_D("Creating PhysicsShape type {} for source geometry: {}...",
				 magic_enum::enum_name(type), *name);
	}
	else {
		AE_LOG_D("Creating PhysicsShape type {} for source geometry: {:p}...",
				 magic_enum::enum_name(type), (void*)geometry.get());
	}

	//sourceGeometry(geometry);
}

// construct a compound shape based on geometries under this node
PhysicsShape::PhysicsShape(PHYSICS_SHAPE_TYPE type, shared_ptr<Node> node):
//		_sourceGeometry({}),
//		_sourceNode({}),
		_sourceObject(node),
		_type(type),
		_dirtyBits(PHYSICS_SHAPE_DIRTY_BITS::ALL) {

	if (auto name = node->name()) {
		AE_LOG_D("Creating PhysicsShape type {} for source node: {}...",
				 magic_enum::enum_name(type), *name);
	}
	else {
		AE_LOG_D("Creating PhysicsShape type {} for source node: {:p}...",
				 magic_enum::enum_name(type), (void*)node.get());
	}

	//sourceNode(node);
}

PhysicsShape::~PhysicsShape() {
	AE_LOG_D("Destroying PhysicsShape {:p}", (void*)this);
}

/*********************************************************************************************
	Public
 *********************************************************************************************/

//weak_ptr<Geometry> PhysicsShape::sourceGeometry() const {
//	return _sourceGeometry;
//}
//
//weak_ptr<Node> PhysicsShape::sourceNode() const {
//	return _sourceNode;
//}

//weak_ptr<std::any> PhysicsShape::sourceObject() const {
//	return _sourceObject;
//}

variant<weak_ptr<Geometry>, weak_ptr<Node>> PhysicsShape::sourceObject() const {
	return _sourceObject;
}

PHYSICS_SHAPE_TYPE PhysicsShape::type() const {
	return _type;
}

void PhysicsShape::type(PHYSICS_SHAPE_TYPE type) {
	_type = type;
	_dirtyBits = PHYSICS_SHAPE_DIRTY_BITS_ADD(_dirtyBits, PHYSICS_SHAPE_DIRTY_BITS::MODEL);
}

/*********************************************************************************************
	Internal
 *********************************************************************************************/

void PhysicsShape::sourceObject(variant<weak_ptr<Geometry>, weak_ptr<Node>> sourceObject) {
	_sourceObject = sourceObject;
}

//void PhysicsShape::sourceGeometry(std::weak_ptr<Geometry> geometry) {
//	_sourceGeometry = geometry;
//
//	_dirtyBits = PHYSICS_SHAPE_DIRTY_BITS_ADD(_dirtyBits, PHYSICS_SHAPE_DIRTY_BITS::MODEL);
//}
//
//void PhysicsShape::sourceNode(std::weak_ptr<Node> node) {
//	_sourceNode = node;
//
//	_dirtyBits = PHYSICS_SHAPE_DIRTY_BITS_ADD(_dirtyBits, PHYSICS_SHAPE_DIRTY_BITS::MODEL);
//}

//void PhysicsShape::attachedToBody(shared_ptr<PhysicsBody> body) {
//
//	_sourceNode = body->node();
//
//	if (auto node = _sourceNode.lock()) {
//		if (auto geometry = node->geometry()) {
//			sourceGeometry(geometry);
//		}
//	}
//}

PHYSICS_SHAPE_DIRTY_BITS PhysicsShape::dirtyBits() const {
	return _dirtyBits;
}

void PhysicsShape::dirtyBits(PHYSICS_SHAPE_DIRTY_BITS bits) {
	_dirtyBits = bits;
}
