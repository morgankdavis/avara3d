//
//  PhysicsBody.cc
//  avara3d
//
//  Created by Morgan Davis on 1/26/18.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#include "a3d/physics/PhysicsBody.h"

#include <cmath>
#include <stdexcept>

#include "a3d/log/Log.h"
#include "a3d/mesh/Mesh.h"
#include "a3d/mesh/primitive/Box.h"
#include "a3d/mesh/primitive/Capsule.h"
#include "a3d/mesh/primitive/Cone.h"
#include "a3d/mesh/primitive/Cylinder.h"
#include "a3d/mesh/primitive/Plane.h"
#include "a3d/mesh/primitive/Sphere.h"
#include "a3d/physics/shape/PhysicsShape.h"
#include "a3d/physics/PhysicsWorld.h"
#include "a3d/physics/backend/bullet/BulletBodyProxy.h"
#include "a3d/physics/backend/bullet/BulletWorldProxy.h"
#include "a3d/physics/shape/primitive/BoxPhysicsShape.h"
#include "a3d/physics/shape/primitive/CapsulePhysicsShape.h"
#include "a3d/physics/shape/primitive/ConePhysicsShape.h"
#include "a3d/physics/shape/primitive/CylinderPhysicsShape.h"
#include "a3d/physics/shape/primitive/PlanePhysicsShape.h"
#include "a3d/physics/shape/primitive/SpherePhysicsShape.h"
#include "a3d/scene/Node.h"
#include "a3d/util/Enum.h"

using namespace a3d;
using namespace a3d::math;
using namespace std;

/// Public Static Member Functions ///

unique_ptr<PhysicsBody> PhysicsBody::StaticBody() {
    return make_unique<PhysicsBody>(Type::Static);
}

unique_ptr<PhysicsBody> PhysicsBody::DynamicBody() {
    return make_unique<PhysicsBody>(Type::Dynamic);
}

unique_ptr<PhysicsBody> PhysicsBody::KinematicBody() {
    return make_unique<PhysicsBody>(Type::Kinematic);
}

/// Private Static Non-Member Prototypes ///

static shared_ptr<PhysicsShape> PhysicsShapeFromPrimitiveMesh(const shared_ptr<Mesh>& mesh);

/// Public Lifecycle Functions ///

PhysicsBody::PhysicsBody(Type type):
    _shape {},
    _node {},
    _world {} {

    // _node has to be initialized to nullptr before calling this
    _proxy = make_unique<BulletBodyProxy>(*this, type);
}

PhysicsBody::PhysicsBody(Type type, const shared_ptr<PhysicsShape>& shape):
    PhysicsBody {type} {

    this->shape(shape);
}

PhysicsBody::~PhysicsBody() {
    log::d()("Destroying PhysicsBody {:p}", static_cast<void*>(this));

    if (_world) {
        _world->remove(*this);
    }

    if (_shape) {
        _shape->detachedFromBody(*this);
    }
    if (_proxy) {
        _proxy->detachedFromBody(*this);
    }
}

/// Public Member Functions ///

PhysicsBody::Type PhysicsBody::type() const {
    return _proxy->type();
}

void PhysicsBody::type(Type type) {
    log::d()("type: {}", util::enums::enum_name(type));
    //_proxy->type(type);

    // ! TEMPORARY !
    if (type != this->type()) {
        throw logic_error("PhysicsBody type cannot be changed after creation.");
    }
}

const shared_ptr<PhysicsShape>& PhysicsBody::shape() const {
    return _shape;
}

void PhysicsBody::shape(const shared_ptr<PhysicsShape>& shape) {
    log::t()("shape: {:p}", static_cast<void*>(shape.get()));

    if (shape == _shape) {
        return;
    }

    if (shape && !shape->supportsBodyType(type())) {
        throw logic_error("PhysicsShape is incompatible with this PhysicsBody type.");
    }

    if (_world) {
        _world->remove(*this);
    }

    _proxy->shapeProxy(nullptr);

    if (_shape) {
        _shape->detachedFromBody(*this);
    }

    _shape = shape;

    if (_shape) {
        _shape->attachedToBody(*this);

        if (_proxy->shapeProxy() != _shape->proxy()) {
            _proxy->shapeProxy(_shape->proxy());
        }

        if (!_node.expired()) {
            checkAddToWorld();
        }
    }
}

float PhysicsBody::mass() const {
    return _proxy->mass();
}

void PhysicsBody::mass(float mass) {

    if (type() != Type::Dynamic) {
        throw logic_error("Mass may only be changed on dynamic PhysicsBody objects.");
    }

    if (!math::is_finite(mass) || mass <= 0.0f) {
        throw invalid_argument("PhysicsBody mass must be positive and finite.");
    }

    _proxy->mass(mass);
}

vec3 PhysicsBody::momentOfInertia() const {
    return _proxy->momentOfInertia();
}

void PhysicsBody::momentOfInertia(const vec3& moment) {
    _proxy->momentOfInertia(moment);
}

vec3 PhysicsBody::centerOfMass() const {
    return _proxy->centerOfMass();
}

void PhysicsBody::centerOfMass(const vec3& centerOfMass) {

    if (type() != Type::Dynamic) {
        throw logic_error("Center of mass may only be changed on dynamic PhysicsBody objects.");
    }

    if (!math::is_finite(centerOfMass.x) || !math::is_finite(centerOfMass.y)
        || !math::is_finite(centerOfMass.z)) {

        throw invalid_argument("PhysicsBody center of mass must be finite.");
    }

    if (_world) {
        _world->remove(*this);
    }

    _proxy->autocalculatesCenterOfMass(false);
    _proxy->centerOfMass(centerOfMass);

    if (!_node.expired()) {
        checkAddToWorld();
    }
}

float PhysicsBody::friction() const {
    return _proxy->friction();
}

void PhysicsBody::friction(float friction) {
    _proxy->friction(friction);
}

float PhysicsBody::rollingFriction() const {
    return _proxy->rollingFriction();
}

void PhysicsBody::rollingFriction(float friction) {
    _proxy->rollingFriction(friction);
}

float PhysicsBody::restitution() const {
    return _proxy->restitution();
}

void PhysicsBody::restitution(float restitution) {
    _proxy->restitution(restitution);
}

vec3 PhysicsBody::linearVelocity() const {
    return _proxy->linearVelocity();
}

void PhysicsBody::linearVelocity(const vec3& velocity) {
    _proxy->linearVelocity(velocity);
}

vec3 PhysicsBody::angularVelocity() const {
    return _proxy->angularVelocity();
}

void PhysicsBody::angularVelocity(const vec3& velocity) {
    _proxy->angularVelocity(velocity);
}

vec3 PhysicsBody::linearFactor() const {
    return _proxy->linearFactor();
}

void PhysicsBody::linearFactor(const vec3& factor) {
    _proxy->linearFactor(factor);
}

vec3 PhysicsBody::angularFactor() const {
    return _proxy->angularFactor();
}

void PhysicsBody::angularFactor(const vec3& factor) {
    _proxy->angularFactor(factor);
}

float PhysicsBody::linearDamping() const {
    return _proxy->linearDamping();
}

void PhysicsBody::linearDamping(float damping) {
    _proxy->linearDamping(damping);
}

float PhysicsBody::angularDamping() const {
    return _proxy->angularDamping();
}

void PhysicsBody::angularDamping(float damping) {
    _proxy->angularDamping(damping);
}

float PhysicsBody::linearSleepingThreshold() const {
    return _proxy->linearSleepingThreshold();
}

void PhysicsBody::linearSleepingThreshold(float threshold) {
    _proxy->linearSleepingThreshold(threshold);
}

float PhysicsBody::angularSleepingThreshold() const {
    return _proxy->angularSleepingThreshold();
}

void PhysicsBody::angularSleepingThreshold(float threshold) {
    _proxy->angularSleepingThreshold(threshold);
}

void PhysicsBody::applyForce(const vec3& force, bool impulse) {

    if (type() != PhysicsBody::Type::Dynamic) {
        throw logic_error("Force may only be applied to dynamic PhysicsBody objects.");
    }

    if (impulse) {
        _proxy->applyCentralImpulse(force);
    }
    else {
        _proxy->applyCentralForce(force);
    }
}

void PhysicsBody::applyForce(const vec3& force, const vec3& worldPosition, bool impulse) {

    if (type() != PhysicsBody::Type::Dynamic) {
        throw logic_error("Force may only be applied to dynamic PhysicsBody objects.");
    }

    if (impulse) {
        _proxy->applyImpulse(force, worldPosition);
    }
    else {
        _proxy->applyForce(force, worldPosition);
    }
}

void PhysicsBody::applyTorque(const vec3& torque, bool impulse) {

    if (type() != PhysicsBody::Type::Dynamic) {
        throw logic_error("Torque may only be applied to dynamic PhysicsBody objects.");
    }

    if (impulse) {
        _proxy->applyTorqueImpulse(torque);
    }
    else {
        _proxy->applyTorque(torque);
    }
}

vec3 PhysicsBody::totalForce() const {
    return _proxy->totalForce();
}

vec3 PhysicsBody::totalTorque() const {
    return _proxy->totalTorque();
}

void PhysicsBody::clearForces() {
    _proxy->clearForces();
}

void PhysicsBody::ccdEnabled(bool enabled) {
    _proxy->ccdEnabled(enabled);
}

bool PhysicsBody::ccdEnabled() const {
    return _proxy->ccdEnabled();
}

void PhysicsBody::ccdMotionThreshold(float distance) {
    _proxy->ccdMotionThreshold(distance);
}

float PhysicsBody::ccdMotionThreshold() const {
    return _proxy->ccdMotionThreshold();
}

void PhysicsBody::ccdSweptSphereRadius(float radius) {
    _proxy->ccdSweptSphereRadius(radius);
}

float PhysicsBody::ccdSweptSphereRadius() const {
    return _proxy->ccdSweptSphereRadius();
}

bool PhysicsBody::affectedByGravity() const {
    return _proxy->affectedByGravity();
}

void PhysicsBody::affectedByGravity(bool affectedByGravity) {
    _proxy->affectedByGravity(affectedByGravity);
}

bool PhysicsBody::allowsResting() const {
    return _proxy->allowsResting();
}

void PhysicsBody::allowsResting(bool allowsResting) {
    _proxy->allowsResting(allowsResting);
}

bool PhysicsBody::resting() const {
    return _proxy->resting();
}

void PhysicsBody::resting(bool resting) {
    _proxy->resting(resting);
}

bool PhysicsBody::autocalculatesCenterOfMass() const {
    return _proxy->autocalculatesCenterOfMass();
}

void PhysicsBody::autocalculatesCenterOfMass(bool autocalculate) {

    if (type() != Type::Dynamic && autocalculate) {
        throw logic_error(
            "Center of mass may only be automatically calculated for dynamic PhysicsBody objects.");
    }

    if (_proxy->autocalculatesCenterOfMass() == autocalculate) {
        return;
    }

    // Disabling automatic calculation does not change the current COM.
    if (!autocalculate) {
        _proxy->autocalculatesCenterOfMass(false);
        return;
    }

    if (_world) {
        _world->remove(*this);
    }

    _proxy->autocalculatesCenterOfMass(true);

    if (!_node.expired()) {
        checkAddToWorld();
    }
}

bool PhysicsBody::autocalculatesMomentOfInertia() const {
    return _proxy->autocalculatesMomentOfInertia();
}

void PhysicsBody::autocalculatesMomentOfInertia(bool autocalculate) {
    _proxy->autocalculatesMomentOfInertia(autocalculate);
}

/// Internal Member Functions ///

void PhysicsBody::attachedToNode(const shared_ptr<Node>& node) {
    log::t()("node: {:p}", static_cast<void*>(node.get()));

    _node = node;

    checkAutocreateShape(node);

    checkAddToWorld();
}

void PhysicsBody::detachedFromNode(const std::shared_ptr<Node>& node) {
    log::t()("node: {:p}", static_cast<void*>(node.get()));

    // PhysicsWorld::remove() handled in physicsWorldUnreachable()

    _node = {}; // ^^ physicsWorld() relies on old _node
}

void PhysicsBody::meshAttachedToNode(const shared_ptr<Mesh>& mesh) {
    log::t()("mesh: {:p}", static_cast<void*>(mesh.get()));

    checkAutocreateShape(mesh);
}

void PhysicsBody::meshDetachedFromNode(const shared_ptr<Mesh>& mesh) {
    log::t()("mesh: {:p}", static_cast<void*>(mesh.get()));
}

void PhysicsBody::physicsWorldReachable(PhysicsWorld& world) {
    log::t()("world: {:p}", static_cast<void*>(&world));

    if (_shape) {
        _shape->physicsWorldReachable(world);
    }

    checkAddToWorld();
}

void PhysicsBody::physicsWorldUnreachable(PhysicsWorld& world) {
    log::t()("world: {:p}", static_cast<void*>(&world));

    if (_shape) {
        _shape->physicsWorldUnreachable(world);
    }

    // if _world != nullptr it's the world we're currently in
    if (_world) {
        _world->remove(*this);
    }
}

void PhysicsBody::addedToWorld(PhysicsWorld& world) {
    log::d()("world: {}", static_cast<void*>(&world));

    _world = &world;
}

void PhysicsBody::removedFromWorld(PhysicsWorld& world) {
    log::d()("world: {}", static_cast<void*>(&world));

    _world = nullptr;
}

void PhysicsBody::shapeWillUpdate() {

    if (_world) {
        _world->remove(*this);
    }

    if (_proxy) {
        _proxy->shapeProxy(nullptr);
    }
}

void PhysicsBody::shapeDidUpdate() {

    if (!_proxy) {
        log::e()("No body proxy.");
        return;
    }

    if (!_shape) {
        log::e()("No shape.");
        _proxy->shapeProxy(nullptr);
        return;
    }

    if (!_shape->proxy()) {
        log::e()("PhysicsShape has no proxy.");
        _proxy->shapeProxy(nullptr);
        return;
    }

    _proxy->shapeProxy(_shape->proxy());

    if (!_node.expired()) {
        checkAddToWorld();
    }
}

void PhysicsBody::syncTransformFromNode() {

    if (auto node = _node.lock()) {

        const mat4 worldTransform =
            translate(mat4(1.0f), node->worldPosition()) * mat4_cast(node->worldOrientation());

        _proxy->worldTransform(worldTransform);
    }
}

weak_ptr<Node> PhysicsBody::node() const {
    return _node;
}

PhysicsWorld* PhysicsBody::physicsWorld() const {

    if (auto node = _node.lock()) {
        if (auto scene = node->scene()) {
            if (auto physicsWorld = scene->physicsWorld(); physicsWorld) {
                return physicsWorld;
            }
        }
    }
    else {
        log::e()("_node is gone.");
        // TODO: throw?
    }
    return nullptr;
}

PhysicsBodyProxy* PhysicsBody::proxy() const {
    return _proxy.get();
}

/// Private Member Functions ///

void PhysicsBody::checkAutocreateShape(const shared_ptr<Node>& node) {

    if (!_shape) {
//		if (auto sNode = node.lock()) {
        if (auto mesh = node->mesh()) {
                // make a shape based on the mesh
            checkAutocreateShape(mesh);
        }
        else {
                // make a shape based on the node
            auto shapeType = PhysicsShape::Type::ConcavePolyhedron;
            if (type() == Type::Static) {
                log::d()("Autocreating {} PhysicsShape for Node {:p}...", util::enums::enum_name(shapeType),
                         static_cast<void*>(node.get()));
                shape(make_shared<PhysicsShape>(shapeType, node));
            }
            else {
                auto shapeType = PhysicsShape::Type::ConvexHull;
                log::d()("Autocreating {} PhysicsShape for Node {:p}...", util::enums::enum_name(shapeType),
                         static_cast<void*>(node.get()));
                shape(make_shared<PhysicsShape>(shapeType, node));
            }
        }
//		}
//		else {
//			throw std::bad_weak_ptr();
//		}
    }
}

void PhysicsBody::checkAutocreateShape(const shared_ptr<Mesh>& mesh) {

    if (_shape) {
        log::i()("PhysicsBody already has a PhysicsShape.  Not auto-creating because of node mesh addition.");
        return;
    }

    if (auto primitiveShape = PhysicsShapeFromPrimitiveMesh(mesh)) {
        shape(primitiveShape);
        return;
    }

    if (type() == Type::Static) {

        auto shapeType = PhysicsShape::Type::ConcavePolyhedron;
        log::d()("Autocreating {} PhysicsShape for Mesh {:p}...", util::enums::enum_name(shapeType),
                 static_cast<void*>(mesh.get()));
        shape(make_shared<PhysicsShape>(shapeType, mesh));
    }
    else {

        auto shapeType = PhysicsShape::Type::ConvexHull;
        log::d()("Autocreating {} PhysicsShape for Mesh {:p}...", util::enums::enum_name(shapeType),
                 static_cast<void*>(mesh.get()));
        shape(make_shared<PhysicsShape>(shapeType, mesh));
    }
}

void PhysicsBody::checkAddToWorld() {

    if (!_world) {
        if (auto world = physicsWorld()) {
            world->add(*this);
        }
    }
}

/// Private Static Non-Member Functions ///

static shared_ptr<PhysicsShape> PhysicsShapeFromPrimitiveMesh(const shared_ptr<Mesh>& mesh) {

    if (!mesh || mesh->elements().size() != 1) {
        return nullptr;
    }

    auto* element = mesh->elements().front().get();

    if (auto box = dynamic_cast<Box*>(element)) {
        log::d()("Creating BoxPhysicsShape based on Box MeshElement.");
        return make_shared<BoxPhysicsShape>(box->width(), box->height(), box->length());
    }

    if (auto capsule = dynamic_cast<Capsule*>(element)) {
        log::d()("Creating CapsulePhysicsShape based on Capsule MeshElement.");
        return make_shared<CapsulePhysicsShape>(capsule->radius(), capsule->height());
    }

    if (auto cone = dynamic_cast<Cone*>(element)) {
        log::d()("Creating ConePhysicsShape based on Cone MeshElement.");
        return make_shared<ConePhysicsShape>(cone->radius(), cone->height());
    }

    if (auto cylinder = dynamic_cast<Cylinder*>(element)) {
        log::d()("Creating CylinderPhysicsShape based on Cylinder MeshElement.");
        return make_shared<CylinderPhysicsShape>(cylinder->radius(), cylinder->height());
    }

    if (auto plane = dynamic_cast<Plane*>(element)) {
        log::d()("Creating PlanePhysicsShape based on Plane MeshElement.");
        return make_shared<PlanePhysicsShape>(plane->width(), plane->height());
    }

    if (auto sphere = dynamic_cast<Sphere*>(element)) {
        log::d()("Creating SpherePhysicsShape based on Sphere MeshElement.");
        return make_shared<SpherePhysicsShape>(sphere->radius());
    }

    return nullptr;
}
