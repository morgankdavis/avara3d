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
#include "physics/PhysicsShapeResources.h"
#include "physics/PhysicsSimulator.h"
#include "physics/PhysicalWorld.h"
#include "physics/bullet/BulletShapeResources.h"
//#include "physics/shape_primitives/BoxPhysicsShape.h"
//#include "physics/shape_primitives/CapsulePhysicsShape.h"
//#include "physics/shape_primitives/ConePhysicsShape.h"
//#include "physics/shape_primitives/CylinderPhysicsShape.h"
//#include "physics/shape_primitives/PlanePhysicsShape.h"
//#include "physics/shape_primitives/SpherePhysicsShape.h"
#include "scene/Node.h"


using namespace ae;
using namespace glm;
using namespace std;


/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

//PhysicsShape::PhysicsShape(PHYSICS_SHAPE_TYPE type):
//		_sourceObject(monostate{}),
//		_type(type),
//		_resources(make_shared<BulletShapeResources>()),
//		_dirtyMask(PHYSICS_SHAPE_DIRTY_MASK::ALL) { }

PhysicsShape::PhysicsShape(PHYSICS_SHAPE_TYPE type, Geometry* geometry):
		_sourceObject(geometry),
		_bodies({}),
		_type(type),
//		_resources(make_shared<BulletShapeResources>()),
		_dirtyMask(PHYSICS_SHAPE_DIRTY_MASK::ALL) {

	if (auto name = geometry->name()) {
		AE_LOG_D("Creating PhysicsShape type {} for source geometry: {}...",
				 magic_enum::enum_name(type), *name);
	}
	else {
		AE_LOG_D("Creating PhysicsShape type {} for source geometry: {:p}...",
				 magic_enum::enum_name(type), (void*)geometry);
	}
}

// construct a compound shape based on geometries under this node
PhysicsShape::PhysicsShape(PHYSICS_SHAPE_TYPE type, Node* node):
		_sourceObject(node),
		_bodies({}),
		_type(type),
//		_resources(make_shared<BulletShapeResources>()),
		_dirtyMask(PHYSICS_SHAPE_DIRTY_MASK::ALL) {

	if (auto name = node->name()) {
		AE_LOG_D("Creating PhysicsShape type {} for source node: {}...",
				 magic_enum::enum_name(type), *name);
	}
	else {
		AE_LOG_D("Creating PhysicsShape type {} for source node: {:p}...",
				 magic_enum::enum_name(type), (void*)node);
	}
}

PhysicsShape::PhysicsShape():
		_sourceObject(monostate{}),
		_bodies({}),
//		_resources(make_shared<BulletShapeResources>()),
		_dirtyMask(PHYSICS_SHAPE_DIRTY_MASK::ALL) { }

PhysicsShape::~PhysicsShape() {
	AE_LOG_D("Destroying PhysicsShape {:p}", (void*)this);
}

/*********************************************************************************************
	Public
 *********************************************************************************************/

//variant<weak_ptr<Geometry>, weak_ptr<Node>, monostate> PhysicsShape::sourceObject() const {
//	return _sourceObject;
//}

variant<Geometry*, Node*, monostate> PhysicsShape::sourceObject() const {
	return _sourceObject;
}

PHYSICS_SHAPE_TYPE PhysicsShape::type() const {
	return _type;
}

void PhysicsShape::type(PHYSICS_SHAPE_TYPE type) {
	AE_LOG_T("type: {}", magic_enum::enum_name(type));

	if (type != _type) {

		_type = type;
		_resources = nullptr;
		_dirtyMask = PHYSICS_SHAPE_DIRTY_MASK_ADD(_dirtyMask, PHYSICS_SHAPE_DIRTY_MASK::MODEL);

		checkCreateModel();
	}
}

/*********************************************************************************************
	Internal
 *********************************************************************************************/

void PhysicsShape::attachedToBody(PhysicsBody* body) {
	AE_LOG_T("body: {:p}", (void*)body);

	if (!_bodies.count(body)) {
		_bodies.insert(body);

		checkCreateModel();
	}
}

void PhysicsShape::detachedFromBody(PhysicsBody* body) {
	AE_LOG_T("body: {:p}", (void*)body);

	_bodies.erase(body);
}

void PhysicsShape::physicalWorldReachable(PhysicalWorld* world) {
	AE_LOG_T("world: {:p}", (void*)world);

	checkCreateModel();
}

void PhysicsShape::physicalWorldUnreachable(PhysicalWorld* world) {
	AE_LOG_T("world: {:p}", (void*)world);
}

void PhysicsShape::sourceObject(variant<
		Geometry*,
		Node*,
		monostate> sourceObject) {

	_sourceObject = sourceObject;
	_dirtyMask = PHYSICS_SHAPE_DIRTY_MASK_ADD(_dirtyMask, PHYSICS_SHAPE_DIRTY_MASK::MODEL);
}

unordered_set<PhysicsBody*> PhysicsShape::bodies() const {
	return _bodies;
}

PhysicsSimulator* PhysicsShape::physicsSimulator() const {

	for (auto& body : _bodies) {
		if (auto world = body->physicalWorld()) {
			return world->simulator();
		}
	}
	return nullptr;
}

//PhysicalWorld* PhysicsShape::physicalWorld() const {
//
//	if (_body) {
//		if (_node) {
//			auto scene = _node->scene();
//			if (scene) {
//				auto physicalWorld = scene->physicalWorld();
//				if (physicalWorld) {
//					return physicalWorld.get();
//				}
//			}
//		}
//	}
//	return nullptr;
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


PhysicsShapeResources* PhysicsShape::resources() const {
	return _resources.get();
}

void PhysicsShape::resources(std::shared_ptr<PhysicsShapeResources> resources) {
	_resources = resources;
}

//void PhysicsShape::update(PhysicsSimulator& simulator,
//						  Node& node,
//						  PhysicsBody& body,
//						  Stats& stats) {
//
//	bool updated;
//	simulator.update(*this,
//					 body.type(),
//					 updated);
//
//	if (updated) {
//		// when PhysicsBody calls PhysicsSimulator::update() the simulator needs
//		// to know that the underlying model's shape has changed
//		auto dirtyMask = body.dirtyMask();
//		PHYSICS_BODY_DIRTY_MASK_ADD(dirtyMask, PHYSICS_BODY_DIRTY_MASK::SHAPE);
//		body.dirtyMask(dirtyMask);
//	}
//}
//
//void PhysicsShape::sync(PhysicsSimulator& simulator,
//						Node& node,
//						PhysicsBody& body,
//						Stats& stats) {
//
//	simulator.sync(*this,
//				   body.type());
//
//	// this is probably slow
//	if (dynamic_cast<BoxPhysicsShape*>(this)
//		|| dynamic_cast<CapsulePhysicsShape*>(this)
//		|| dynamic_cast<ConePhysicsShape*>(this)
//		|| dynamic_cast<CylinderPhysicsShape*>(this)
//		|| dynamic_cast<PlanePhysicsShape*>(this)
//		|| dynamic_cast<SpherePhysicsShape*>(this)) {
//
//		// TODO: PUT IN SUBCLASS
//	}
//	else {
//		switch (type()) {
//			case (PHYSICS_SHAPE_TYPE::BOUNDING_BOX):
//				++stats.boundingBoxShapes;
//				break;
//			case (PHYSICS_SHAPE_TYPE::CONVEX_HULL):
//				++stats.convexHullShapes;
//				break;
//			case (PHYSICS_SHAPE_TYPE::CONCAVE_POLYHEDRON):
//				++stats.concavePolyhedronShapes;
//				break;
//		}
//	}
//}

void PhysicsShape::checkCreateModel() {
	AE_LOG_T("");

	if (!_resources) {
		if (auto simulator = physicsSimulator()) {
			simulator->create(*this);

			for (auto& body : _bodies) {
				body->modelCreated(*this);
			}
		}
	}
}

PHYSICS_SHAPE_DIRTY_MASK PhysicsShape::dirtyMask() const {
	return _dirtyMask;
}

void PhysicsShape::dirtyMask(PHYSICS_SHAPE_DIRTY_MASK mask) {
	_dirtyMask = mask;
}
