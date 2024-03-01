//
//  PhysicsShape.cc
//	avara-engine
//
//  Created by Morgan Davis on 1/25/18.
//  Copyright © 2018 Morgan K Davis. All rights reserved.
//

#include "ae/physics/PhysicsShape.h"


#include "magic_enum.hpp"

#include "ae/mesh/Mesh.h"
#include "ae/diagnostic/logging/Logger.h"
#include "ae/physics/PhysicsBody.h"
#include "ae/physics/proxy/PhysicsShapeProxy.h"
#include "ae/physics/PhysicalWorld.h"
#include "ae/physics/bullet/BulletShapeProxy.h"
#include "ae/scene/Node.h"


using namespace ae;
using namespace glm;
using namespace std;


/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

PhysicsShape::PhysicsShape(PhysicsShapeType type, Mesh* geometry):
		_sourceObject(geometry),
		_bodies({}),
		_type(type),
		_proxy(nullptr)
		/*_model(make_unique<BulletShapeProxy>(this))*/ {

	if (auto name = geometry->name()) {
		AE_LOG_D("Creating PhysicsShape type {} for source mesh: {}...",
				 magic_enum::enum_name(type), *name);
	}
	else {
		AE_LOG_D("Creating PhysicsShape type {} for source mesh: {:p}...",
				 magic_enum::enum_name(type), static_cast<void*>(geometry));
	}
}

// construct a compound shape based on geometries under this node
PhysicsShape::PhysicsShape(PhysicsShapeType type, Node* node):
		_sourceObject(node),
		_bodies({}),
		_type(type),
		_proxy(nullptr)
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
		_bodies({}),
		_proxy(nullptr)
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

PhysicsShape::SourceObject PhysicsShape::sourceObject() const {
	return _sourceObject;
}

PhysicsShapeType PhysicsShape::type() const {
	return _type;
}

void PhysicsShape::type(PhysicsShapeType type) {
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

void PhysicsShape::sourceObject(SourceObject sourceObject) {

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

PhysicsShapeProxy* PhysicsShape::proxy() const {
	return _proxy.get();
}
