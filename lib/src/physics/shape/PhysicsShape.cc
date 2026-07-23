//
//  PhysicsShape.cc
//  avara3d
//
//  Created by Morgan Davis on 1/25/18.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#include "a3d/physics/shape/PhysicsShape.h"

#include <magic_enum/magic_enum.hpp>

#include "a3d/mesh/Mesh.h"
#include "a3d/log/Log.h"
#include "a3d/physics/PhysicsBody.h"
#include "a3d/physics/PhysicsWorld.h"
#include "a3d/physics/backend/bullet/BulletShapeProxy.h"
#include "a3d/scene/Node.h"

using namespace a3d;
using namespace std;

/// Public Lifecycle Functions ///

PhysicsShape::PhysicsShape(Type type, const shared_ptr<Mesh>& mesh):
		_source{mesh},
		_bodies{},
		_type{type},
		_proxy{}
		/*_model(make_unique<BulletShapeProxy>(this))*/ {

	if (auto name = mesh->name()) {
		log::d()("Creating PhysicsShape type {} for source mesh: {}...",
				  magic_enum::enum_name(type), *name);
	}
	else {
		log::d()("Creating PhysicsShape type {} for source mesh: {:p}...",
				  magic_enum::enum_name(type), static_cast<void *>(mesh.get()));
	}
}

// construct a compound shape based on meshes under this node
PhysicsShape::PhysicsShape(Type type, const shared_ptr<Node>& node):
		_source{node},
		_bodies{},
		_type{type},
		_proxy{}
		/*_model(make_unique<BulletShapeProxy>(this))*/ {

	if (auto name = node->name()) {
		log::d()("Creating PhysicsShape type {} for source node: {}...",
				  magic_enum::enum_name(type), *name);
	}
	else {
		log::d()("Creating PhysicsShape type {} for source node: {:p}...",
				  magic_enum::enum_name(type), static_cast<void *>(node.get()));
	}
}

PhysicsShape::PhysicsShape():
		_type{Type::Primitive},
		_source{},
		_bodies{},
		_proxy{} {}

PhysicsShape::~PhysicsShape() {
	log::d()("Destroying PhysicsShape {:p}", static_cast<void*>(this));
}

/// Public Member Functions ///

PhysicsShape::Source PhysicsShape::source() const {
	return _source;
}

PhysicsShape::Type PhysicsShape::type() const {
	return _type;
}

void PhysicsShape::type(Type type) {
	log::t()("type: {}", magic_enum::enum_name(type));

	if (_type == type) {
		return;
	}

	for (auto* body : _bodies) {
		body->shapeWillUpdate();
	}

	const auto previousType = _type;
	_type = type;

	try {
		std::unique_ptr<PhysicsShapeProxy> replacementProxy;

		if (!_bodies.empty()) {
			replacementProxy = make_unique<BulletShapeProxy>(*this);
		}

		_proxy = std::move(replacementProxy);
	}
	catch (...) {
		_type = previousType;

		for (auto* body : _bodies) {
			body->shapeDidUpdate();
		}

		throw;
	}

	for (auto* body : _bodies) {
		body->shapeDidUpdate();
	}
}

/// Internal Member Functions ///

void PhysicsShape::attachedToBody(PhysicsBody& body) {
	log::t()("body: {:p}", static_cast<void*>(&body));

	if (!_bodies.count(&body)) {
		_bodies.insert(&body);

		checkCreateProxy();
	}
}

void PhysicsShape::detachedFromBody(PhysicsBody& body) {
	log::t()("body: {:p}", static_cast<void*>(&body));

	_bodies.erase(&body);
}

void PhysicsShape::physicalWorldReachable(PhysicsWorld& world) {
	log::t()("world: {:p}", static_cast<void*>(&world));

	checkCreateProxy();
}

void PhysicsShape::physicalWorldUnreachable(PhysicsWorld& world) {
	log::t()("world: {:p}", static_cast<void*>(&world));
}

void PhysicsShape::source(const Source& sourceObject) {

	_source = sourceObject;
}

void PhysicsShape::checkCreateProxy() {
	log::t();

	if (!_proxy) {
		_proxy = make_unique<BulletShapeProxy>(*this);

		for (auto body : _bodies) {
			body->shapeDidUpdate();
		}
	}
}

const unordered_set<PhysicsBody*>& PhysicsShape::bodies() const {
	return _bodies;
}

PhysicsShapeProxy* PhysicsShape::proxy() const {
	return _proxy.get();
}
