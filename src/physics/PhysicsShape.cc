//
//  PhysicsShape.cc
//	avara-engine
//
//  Created by Morgan Davis on 1/25/18.
//  Copyright © 2018 Morgan K Davis. All rights reserved.
//

#include "physics/PhysicsShape.h"

#include "glm/gtc/type_ptr.hpp"
#include "magic_enum.hpp"

#include "geometry/Geometry.h"
#include "diagnostic/logging/Logger.h"
#include "physics/PhysicsBody.h"
#include "scene/Node.h"


using namespace ae;
using namespace glm;
using namespace std;


/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

PhysicsShape::PhysicsShape(PHYSICS_SHAPE_TYPE type, shared_ptr<Geometry> geometry):
		_sourceObject(geometry),
		_type(type),
		_dirtyMask(PHYSICS_SHAPE_DIRTY_MASK::ALL) {

	if (auto name = geometry->name()) {
		AE_LOG_D("Creating PhysicsShape type {} for source geometry: {}...",
				 magic_enum::enum_name(type), *name);
	}
	else {
		AE_LOG_D("Creating PhysicsShape type {} for source geometry: {:p}...",
				 magic_enum::enum_name(type), (void*)geometry.get());
	}
}

// construct a compound shape based on geometries under this node
PhysicsShape::PhysicsShape(PHYSICS_SHAPE_TYPE type, shared_ptr<Node> node):
		_sourceObject(node),
		_type(type),
		_dirtyMask(PHYSICS_SHAPE_DIRTY_MASK::ALL) {

	if (auto name = node->name()) {
		AE_LOG_D("Creating PhysicsShape type {} for source node: {}...",
				 magic_enum::enum_name(type), *name);
	}
	else {
		AE_LOG_D("Creating PhysicsShape type {} for source node: {:p}...",
				 magic_enum::enum_name(type), (void*)node.get());
	}
}

PhysicsShape::~PhysicsShape() {
	AE_LOG_D("Destroying PhysicsShape {:p}", (void*)this);
}

/*********************************************************************************************
	Public
 *********************************************************************************************/

variant<weak_ptr<Geometry>, weak_ptr<Node>> PhysicsShape::sourceObject() const {
	return _sourceObject;
}

PHYSICS_SHAPE_TYPE PhysicsShape::type() const {
	return _type;
}

void PhysicsShape::type(PHYSICS_SHAPE_TYPE type) {
	_type = type;
	_dirtyMask = PHYSICS_SHAPE_DIRTY_MASK_ADD(_dirtyMask, PHYSICS_SHAPE_DIRTY_MASK::MODEL);
}

/*********************************************************************************************
	Internal
 *********************************************************************************************/

void PhysicsShape::sourceObject(variant<weak_ptr<Geometry>, weak_ptr<Node>> sourceObject) {
	_sourceObject = sourceObject;
	_dirtyMask = PHYSICS_SHAPE_DIRTY_MASK_ADD(_dirtyMask, PHYSICS_SHAPE_DIRTY_MASK::MODEL);
}

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

void  PhysicsShape::update(PhysicsSimulator& simulator,
						   Node& node,
						   PhysicsBody& body,
						   bool& updated,
						   FrameStats& stats) {

}

void  PhysicsShape::sync(PhysicsSimulator& simulator,
						 Node& node,
						 PhysicsBody& body,
						 mat4& transform,
						 FrameStats& stats) {

}

PHYSICS_SHAPE_DIRTY_MASK PhysicsShape::dirtyMask() const {
	return _dirtyMask;
}

void PhysicsShape::dirtyMask(PHYSICS_SHAPE_DIRTY_MASK mask) {
	_dirtyMask = mask;
}
