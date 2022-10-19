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
	Lifecycle
 *********************************************************************************************/

PhysicsShape::PhysicsShape(shared_ptr<Geometry> geometry, PHYSICS_SHAPE_TYPE type):
	_sourceGeometry({}),
	_sourceNode({}),
	_type(PHYSICS_SHAPE_TYPE::CONVEX_HULL),
	_transforms(vector<mat4>()),
	_physicsBody({}),
	_dirtyBits(PHYSICS_SHAPE_DIRTY_BITS::ALL) {
		
		_sourceGeometry = geometry;
		_type = type;
}

// construct a compound shape based on geometries under this node
PhysicsShape::PhysicsShape(shared_ptr<Node> node, PHYSICS_SHAPE_TYPE type):
	_sourceGeometry({}),
	_sourceNode({}),
	_type(PHYSICS_SHAPE_TYPE::CONVEX_HULL),
	_transforms(vector<mat4>()),
	_physicsBody({}),
	_dirtyBits(PHYSICS_SHAPE_DIRTY_BITS::ALL) {
		
		auto name = node->name();
		if (name) {
			AE_LOG_D("Creating PhysicsShape for source node: {}...", *name);
		}
		
		_sourceNode = node;
		_type = type;
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
