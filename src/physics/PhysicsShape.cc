//
//  PhysicsShape.cc
//  avara3d
//
//  Created by Morgan Davis on 1/25/18.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#include "a3d/physics/PhysicsShape.h"

#include "magic_enum/magic_enum.hpp"

#include "a3d/mesh/Mesh.h"
#include "a3d/diagnostic/log/Log.h"
#include "a3d/physics/PhysicsBody.h"
#include "a3d/physics/proxy/PhysicsShapeProxy.h"
#include "a3d/physics/PhysicalWorld.h"
#include "a3d/physics/bullet/BulletShapeProxy.h"
#include "a3d/scene/Node.h"

using namespace a3d;
using namespace std;

/// Public Lifecycle Functions ///

PhysicsShape::PhysicsShape(PhysicsShapeType type, const shared_ptr<Mesh>& mesh):
		_source{mesh},
		_bodies{},
		_type{type},
		_proxy{}
		/*_model(make_unique<BulletShapeProxy>(this))*/ {

	if (auto name = mesh->name()) {
		A3D_LOG_D("Creating PhysicsShape type {} for source mesh: {}...",
				  magic_enum::enum_name(type), *name);
	}
	else {
		A3D_LOG_D("Creating PhysicsShape type {} for source mesh: {:p}...",
				  magic_enum::enum_name(type), static_cast<void *>(mesh.get()));
	}
}

// construct a compound shape based on meshes under this node
PhysicsShape::PhysicsShape(PhysicsShapeType type, const shared_ptr<Node>& node):
		_source{node},
		_bodies{},
		_type{type},
		_proxy{}
		/*_model(make_unique<BulletShapeProxy>(this))*/ {

	if (auto name = node->name()) {
		A3D_LOG_D("Creating PhysicsShape type {} for source node: {}...",
				  magic_enum::enum_name(type), *name);
	}
	else {
		A3D_LOG_D("Creating PhysicsShape type {} for source node: {:p}...",
				  magic_enum::enum_name(type), static_cast<void *>(node.get()));
	}
}

PhysicsShape::PhysicsShape():
		_source{},
		_bodies{},
		_proxy{} { }

PhysicsShape::~PhysicsShape() {
	A3D_LOG_D("Destroying PhysicsShape {:p}", static_cast<void*>(this));
}

/// Public Member Functions ///

PhysicsShape::Source PhysicsShape::source() const {
	return _source;
}

PhysicsShapeType PhysicsShape::type() const {
	return _type;
}

void PhysicsShape::type(PhysicsShapeType type) {
	A3D_LOG_T("type: {}", magic_enum::enum_name(type));

	_type = type;
	_proxy = nullptr;

	checkCreateProxy();
}

/// Internal Member Functions ///

void PhysicsShape::attachedToBody(PhysicsBody& body) {
	A3D_LOG_T("body: {:p}", static_cast<void*>(&body));

	if (!_bodies.count(&body)) {
		_bodies.insert(&body);

		checkCreateProxy();
	}
}

void PhysicsShape::detachedFromBody(PhysicsBody& body) {
	A3D_LOG_T("body: {:p}", static_cast<void*>(&body));

	_bodies.erase(&body);
}

void PhysicsShape::physicalWorldReachable(PhysicalWorld& world) {
	A3D_LOG_T("world: {:p}", static_cast<void*>(&world));

	checkCreateProxy();
}

void PhysicsShape::physicalWorldUnreachable(PhysicalWorld& world) {
	A3D_LOG_T("world: {:p}", static_cast<void*>(&world));
}

void PhysicsShape::source(const Source& sourceObject) {

	_source = sourceObject;
}

void PhysicsShape::checkCreateProxy() {
	A3D_LOG_T("");

	if (!_proxy) {
		_proxy = make_unique<BulletShapeProxy>(*this);

		for (auto body : _bodies) {
			body->shapeUpdated();
		}
	}
}

const unordered_set<PhysicsBody*>& PhysicsShape::bodies() const {
	return _bodies;
}

PhysicsShapeProxy* PhysicsShape::proxy() const {
	return _proxy.get();
}
