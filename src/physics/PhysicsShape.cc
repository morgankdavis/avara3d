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
#include "physics/model_proxy/PhysicsShapeModelProxy.h"
#include "physics/PhysicalWorld.h"
#include "physics/bullet/BulletShapeProxy.h"
#include "scene/Node.h"


using namespace ae;
using namespace glm;
using namespace std;


/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

PhysicsShape::PhysicsShape(PHYSICS_SHAPE_TYPE type, Geometry* geometry):
		_sourceObject(geometry),
		_bodies({}),
		_type(type)
		/*_model(make_unique<BulletShapeProxy>(this))*/ {

	if (auto name = geometry->name()) {
		AE_LOG_D("Creating PhysicsShape type {} for source geometry: {}...",
				 magic_enum::enum_name(type), *name);
	}
	else {
		AE_LOG_D("Creating PhysicsShape type {} for source geometry: {:p}...",
				 magic_enum::enum_name(type), static_cast<void*>(geometry));
	}
}

// construct a compound shape based on geometries under this node
PhysicsShape::PhysicsShape(PHYSICS_SHAPE_TYPE type, Node* node):
		_sourceObject(node),
		_bodies({}),
		_type(type)
		/*_model(make_unique<BulletShapeProxy>(this))*/ {

	if (auto name = node->name()) {
		AE_LOG_D("Creating PhysicsShape type {} for source node: {}...",
				 magic_enum::enum_name(type), *name);
	}
	else {
		AE_LOG_D("Creating PhysicsShape type {} for source node: {:p}...",
				 magic_enum::enum_name(type),static_cast<void*>(node));
	}
}

PhysicsShape::PhysicsShape():
		_sourceObject(monostate{}),
		_bodies({})
		/*_model(make_unique<BulletShapeProxy>(this))*/ { }

PhysicsShape::~PhysicsShape() {
	AE_LOG_D("Destroying PhysicsShape {:p}", static_cast<void*>(this));

//	for (auto& body : _bodies) {
//
//	}
}

/*********************************************************************************************
	Public
 *********************************************************************************************/

variant<Geometry*, Node*, monostate> PhysicsShape::sourceObject() const {
	return _sourceObject;
}

PHYSICS_SHAPE_TYPE PhysicsShape::type() const {
	return _type;
}

void PhysicsShape::type(PHYSICS_SHAPE_TYPE type) {
	AE_LOG_T("type: {}", magic_enum::enum_name(type));

	_type = type;
	_proxy = nullptr;

	checkCreateProxy();

//	for (auto body : _bodies) {
//		body->shapeUpdated();
//	}
}

/*********************************************************************************************
	Internal
 *********************************************************************************************/

void PhysicsShape::attachedToBody(PhysicsBody* body) {
	AE_LOG_T("body: {:p}", static_cast<void*>(body));

	if (!_bodies.count(body)) {
		_bodies.insert(body);

		checkCreateProxy();
	}
}

void PhysicsShape::detachedFromBody(PhysicsBody* body) {
	AE_LOG_T("body: {:p}", static_cast<void*>(body));

	_bodies.erase(body);
}

void PhysicsShape::physicalWorldReachable(PhysicalWorld* world) {
	AE_LOG_T("world: {:p}", static_cast<void*>(world));

	checkCreateProxy();
}

void PhysicsShape::physicalWorldUnreachable(PhysicalWorld* world) {
	AE_LOG_T("world: {:p}", static_cast<void*>(world));
}

void PhysicsShape::sourceObject(variant<
		Geometry*,
		Node*,
		monostate> sourceObject) {

	_sourceObject = sourceObject;
}

void PhysicsShape::checkCreateProxy() {
	AE_LOG_T("");

	if (!_proxy) {
		_proxy = make_unique<BulletShapeProxy>(this);

		for (auto body : _bodies) {
			body->shapeUpdated();
		}
	}
}


unordered_set<PhysicsBody*> PhysicsShape::bodies() const {
	return _bodies;
}

PhysicsShapeModelProxy* PhysicsShape::proxy() const {
	return _proxy.get();
}
