//
//  PhysicsShape.cc
//  avara3d
//
//  Created by Morgan Davis on 1/25/18.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#include "a3d/physics/shape/PhysicsShape.h"

#include <stdexcept>

#include "a3d/mesh/Mesh.h"
#include "a3d/log/Log.h"
#include "a3d/physics/PhysicsBody.h"
#include "a3d/physics/PhysicsWorld.h"
#include "a3d/physics/backend/bullet/BulletShapeProxy.h"
#include "a3d/scene/Node.h"
#include "a3d/util/Enum.h"

using namespace a3d;
using namespace std;

/// Public Lifecycle Functions ///

PhysicsShape::PhysicsShape(Type type, const shared_ptr<Mesh>& mesh):
    _type {type},
    _proxy {},
    _source {mesh},
    _bodies {} {

    if (auto name = mesh->name()) {
        log::d()("Creating PhysicsShape type {} for source mesh: {}...", util::enums::enum_name(type), *name);
    }
    else {
        log::d()("Creating PhysicsShape type {} for source mesh: {:p}...", util::enums::enum_name(type),
                 static_cast<void*>(mesh.get()));
    }
}

// construct a compound shape based on meshes under this node
PhysicsShape::PhysicsShape(Type type, const shared_ptr<Node>& node):
    _type {type},
    _proxy {},
    _source {node},
    _bodies {} {

    if (auto name = node->name()) {
        log::d()("Creating PhysicsShape type {} for source node: {}...", util::enums::enum_name(type), *name);
    }
    else {
        log::d()("Creating PhysicsShape type {} for source node: {:p}...", util::enums::enum_name(type),
                 static_cast<void*>(node.get()));
    }
}

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
    log::t()("type: {}", util::enums::enum_name(type));

    // ! TEMPORARY !
    if (type != _type) {
        throw logic_error("PhysicsShape type cannot be changed after creation.");
    }
}

/// Internal Member Functions ///

bool PhysicsShape::supportsBodyType(PhysicsBody::Type) const {
    return true;
}

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

void PhysicsShape::physicsWorldReachable(PhysicsWorld& world) {
    log::t()("world: {:p}", static_cast<void*>(&world));

    checkCreateProxy();
}

void PhysicsShape::physicsWorldUnreachable(PhysicsWorld& world) {
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

/// Protected Lifecycle ///

PhysicsShape::PhysicsShape():
    _type {Type::Primitive},
    _proxy {},
    _source {},
    _bodies {} {}
