//
//  BulletBodyProxy.cc
//  avara3d
//
//  Created by Morgan Davis on 10/29/23.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#include "a3d/physics/backend/bullet/BulletBodyProxy.h"

#include <stdexcept>

#include <bullet/btBulletDynamicsCommon.h>

#include "a3d/log/Log.h"
#include "a3d/mesh/ConvexDecomposer.h"
#include "a3d/mesh/Mesh.h"
#include "a3d/physics/PhysicsBody.h"
#include "a3d/physics/shape/PhysicsShape.h"
#include "a3d/physics/PhysicsWorld.h"
#include "a3d/physics/backend/bullet/BulletMotionState.h"
#include "a3d/physics/backend/bullet/BulletShapeProxy.h"
#include "a3d/physics/backend/bullet/BulletUtilities.h"
#include "a3d/physics/backend/bullet/BulletWorldProxy.h"
#include "a3d/physics/proxy/PhysicsBodyProxy.h"
#include "a3d/physics/proxy/PhysicsShapeProxy.h"
#include "a3d/scene/Node.h"
#include "a3d/util/Flow.h"

using namespace a3d::math;
using namespace std;

namespace a3d {
namespace {

    // [Private Non-Member Prototypes]

    int  CollisionFlagsForBodyType(int flags, PhysicsBody::Type type);
    void SetMassPropsPreservingType(btRigidBody&      body,
                                    PhysicsBody::Type type,
                                    btScalar          mass,
                                    const btVector3&  inertia);
    void ForceActivationForBodyType(btRigidBody& body, PhysicsBody::Type type);
    void ActivateDynamicBody(btRigidBody& body);

} // namespace

// [Internal Lifecycle Functions]

BulletBodyProxy::BulletBodyProxy(PhysicsBody& body, PhysicsBody::Type type):
    PhysicsBodyProxy {body, type},
    _motionState {},
    _centerOfMassOffsetShape {},
    _btBody {},
    _ccdEnabled {false},
    _ccdMotionThreshold {0.0f},
    _ccdSweptSphereRadius {0.0f} {

    log::d()("body: {:p}", static_cast<void*>(&body));

    // make a "shell" of a body and modify its properties as they are set
    // https://pybullet.org/Bullet/phpBB3/viewtopic.php?p=43923&sid=187e552b028cd64fe2e831df414d382a#p43923

//	_btMotionState = make_shared<btDefaultMotionState>(btTransform::getIdentity());
//	_motionState = make_shared<MotionState>(body, btTransform::getIdentity());
    _motionState = make_unique<BulletMotionState>(body);

    // it seems as though adding a body to the world with mass=0 forever casts it
    // as a static body. adding it, setting it to 0, the setting it to something
    // different seems to work fine, though.
    btRigidBody::btRigidBodyConstructionInfo rigidBodyInfo((/*body.type()*/ type == PhysicsBody::Type::Static
                                                                ? 0.0f
                                                                : 1.0f), // important!
                                                           _motionState.get(), nullptr);
    rigidBodyInfo.m_friction = 0.5;
    rigidBodyInfo.m_rollingFriction = 0.0;
    rigidBodyInfo.m_spinningFriction = 0.0;
    rigidBodyInfo.m_restitution = 0.0;
    rigidBodyInfo.m_linearSleepingThreshold = 1.0;
    rigidBodyInfo.m_angularSleepingThreshold = 1.0;

    _btBody = make_unique<btRigidBody>(rigidBodyInfo);

    BulletBodyProxy::type(type);

    // just checking.
//	if (_btBody->getActivationState() == DISABLE_DEACTIVATION
//		|| _btBody->getActivationState() == DISABLE_SIMULATION) {
//		log::e()("activationState={}", _btBody->getActivationState());
//	}

    _btBody->setUserPointer(static_cast<void*>(&body));
}

BulletBodyProxy::~BulletBodyProxy() {
    log::d()("Destroying BulletBodyProxy {:p}", static_cast<void*>(this));

#ifdef A3D_DEBUG
    if (_btBody && _btBody->isInWorld()) {
        log::e()("btRigidBody still in world!");
        btAssert(false);
    }
#endif
}

// [PhysicsBodyModelProxy Internal Member Functions]

PhysicsBody::Type BulletBodyProxy::type() const {

    const int flags = _btBody->getCollisionFlags();

    if (flags & btCollisionObject::CF_KINEMATIC_OBJECT) {
        return PhysicsBody::Type::Kinematic;
    }
    if (flags & btCollisionObject::CF_STATIC_OBJECT) {
        return PhysicsBody::Type::Static;
    }
    return PhysicsBody::Type::Dynamic; // default when neither static nor kinematic
}

void BulletBodyProxy::type(PhysicsBody::Type type) {

    switch (type) {
        case PhysicsBody::Type::Static:
            SetMassPropsPreservingType(*_btBody, type, 0.0f, btVector3(0, 0, 0));
            break;

        case PhysicsBody::Type::Dynamic:
            _btBody->setCollisionFlags(CollisionFlagsForBodyType(_btBody->getCollisionFlags(), type));
            // mass/inertia handled elsewhere after shape is set
            break;

        case PhysicsBody::Type::Kinematic:
            SetMassPropsPreservingType(*_btBody, type, 0.0f, btVector3(0, 0, 0));
            break;
    }

    ForceActivationForBodyType(*_btBody, type);
}

PhysicsShapeProxy* BulletBodyProxy::shapeProxy() const {
    return _shapeProxy;
}

void BulletBodyProxy::shapeProxy(PhysicsShapeProxy* proxy) {
    log::t()("proxy: {:p}", static_cast<void*>(proxy));

    if (!proxy) {
        _btBody->setCollisionShape(nullptr);
        _centerOfMassOffsetShape.reset();
        _shapeProxy = nullptr;

        syncRollingFrictionAnisotropy();

        if (_autocalculatesCenterOfMass) {
            _centerOfMass = vec3 {0.0f};
        }

        return;
    }

    auto* bulletProxy = dynamic_cast<BulletShapeProxy*>(proxy);

    if (!bulletProxy) {
        throw logic_error("BulletBodyProxy requires a BulletShapeProxy.");
    }

    if (bulletProxy->btShapes().empty() || !bulletProxy->btShapes().front()) {
        throw logic_error("BulletShapeProxy has no root Bullet collision shape.");
    }

    const auto bodyType = _body->type();

    switch (bodyType) {
        case PhysicsBody::Type::Static:
        case PhysicsBody::Type::Kinematic:
            SetMassPropsPreservingType(*_btBody, bodyType, 0.0f, btVector3(0, 0, 0));
            ForceActivationForBodyType(*_btBody, bodyType);
            break;

        case PhysicsBody::Type::Dynamic:
            break;
    }

    _shapeProxy = proxy;

    if (bodyType == PhysicsBody::Type::Dynamic && _autocalculatesCenterOfMass) {
        calculateCenterOfMass();
    }

    rebuildCenterOfMassOffsetShape();
    syncRollingFrictionAnisotropy();

    if (bodyType == PhysicsBody::Type::Dynamic && _autocalculatesMomentOfInertia) {
        calculateMomentOfInertia();
    }
}

float BulletBodyProxy::mass() const {

    auto invMass = _btBody->getInvMass();
    return (invMass != 0 ? 1.0f / _btBody->getInvMass() : 0);
}

void BulletBodyProxy::mass(float mass) {

    const auto bodyType = type();
    btVector3  inertia(0, 0, 0);

    if (bodyType == PhysicsBody::Type::Dynamic && mass > 0.0f && _autocalculatesMomentOfInertia) {
        if (auto* shape = _btBody->getCollisionShape()) {
            shape->calculateLocalInertia(mass, inertia);
        }
    }
    else {
        inertia = _btBody->getLocalInertia();
    }

    SetMassPropsPreservingType(*_btBody, bodyType, mass, inertia);
    if (bodyType == PhysicsBody::Type::Kinematic) {
        ForceActivationForBodyType(*_btBody, bodyType);
    }
    else {
        _btBody->activate(true);
    }
}

vec3 BulletBodyProxy::momentOfInertia() const {
    return A3DVec3FromBTVector3(_btBody->getLocalInertia());
}

void BulletBodyProxy::momentOfInertia(const vec3& moment) {

    if (!_autocalculatesMomentOfInertia) {
        const auto bodyType = type();
        SetMassPropsPreservingType(*_btBody, bodyType, mass(), BTVector3FromA3DVec3(moment));
        if (bodyType == PhysicsBody::Type::Kinematic) {
            ForceActivationForBodyType(*_btBody, bodyType);
        }
    }
    else {
        log::w()("Ignoring moment of inertia: autocalculatesMomentOfInertia to to true.");
    }
}

vec3 BulletBodyProxy::centerOfMass() const {
    return _centerOfMass;
}

void BulletBodyProxy::centerOfMass(const vec3& centerOfMass) {

    if (_centerOfMass.x == centerOfMass.x && _centerOfMass.y == centerOfMass.y
        && _centerOfMass.z == centerOfMass.z) {

        return;
    }

    _centerOfMass = centerOfMass;

    rebuildCenterOfMassOffsetShape();

    if (type() == PhysicsBody::Type::Dynamic && _autocalculatesMomentOfInertia) {
        calculateMomentOfInertia();
    }
}

float BulletBodyProxy::friction() const {
    return _btBody->getFriction();
}

void BulletBodyProxy::friction(float friction) {
    _btBody->setFriction(friction);
}

float BulletBodyProxy::rollingFriction() const {
    return _btBody->getRollingFriction();
}

void BulletBodyProxy::rollingFriction(float friction) {
    _btBody->setRollingFriction(friction);
    syncRollingFrictionAnisotropy();
}

float BulletBodyProxy::restitution() const {
    return _btBody->getRestitution();
}

float BulletBodyProxy::spinningFriction() const {
    return _btBody->getSpinningFriction();
}

void BulletBodyProxy::spinningFriction(float friction) {
    _btBody->setSpinningFriction(friction);
}

void BulletBodyProxy::restitution(float restitution) {
    _btBody->setRestitution(restitution);
}

vec3 BulletBodyProxy::linearVelocity() const {
    return A3DVec3FromBTVector3(_btBody->getLinearVelocity());
}

void BulletBodyProxy::linearVelocity(const vec3& velocity) {
    _btBody->setLinearVelocity(BTVector3FromA3DVec3(velocity));
}

vec3 BulletBodyProxy::angularVelocity() const {
    return A3DVec3FromBTVector3(_btBody->getAngularVelocity());
}

void BulletBodyProxy::angularVelocity(const vec3& velocity) {
    _btBody->setAngularVelocity(BTVector3FromA3DVec3(velocity));
}

vec3 BulletBodyProxy::linearFactor() const {
    return A3DVec3FromBTVector3(_btBody->getLinearFactor());
}

void BulletBodyProxy::linearFactor(const vec3& factor) {
    _btBody->setLinearFactor(BTVector3FromA3DVec3(factor));
}

vec3 BulletBodyProxy::angularFactor() const {
    return A3DVec3FromBTVector3(_btBody->getAngularFactor());
}

void BulletBodyProxy::angularFactor(const vec3& factor) {
    _btBody->setAngularFactor(BTVector3FromA3DVec3(factor));
}

float BulletBodyProxy::linearDamping() const {
    return _btBody->getLinearDamping();
}

void BulletBodyProxy::linearDamping(float damping) {
    _btBody->setDamping(damping, _btBody->getAngularDamping());
}

float BulletBodyProxy::angularDamping() const {
    return _btBody->getAngularDamping();
}

void BulletBodyProxy::angularDamping(float damping) {
    _btBody->setDamping(_btBody->getLinearDamping(), damping);
}

float BulletBodyProxy::linearSleepingThreshold() const {
    return _btBody->getLinearSleepingThreshold();
}

void BulletBodyProxy::linearSleepingThreshold(float threshold) {
    _btBody->setSleepingThresholds(threshold, _btBody->getAngularSleepingThreshold());
}

float BulletBodyProxy::angularSleepingThreshold() const {
    return _btBody->getAngularSleepingThreshold();
}

void BulletBodyProxy::angularSleepingThreshold(float threshold) {
    _btBody->setSleepingThresholds(_btBody->getLinearSleepingThreshold(), threshold);
}

void BulletBodyProxy::applyForce(const vec3& force, const vec3& worldPosition) {

    ActivateDynamicBody(*_btBody);
    const btVector3 relativePosition = BTVector3FromA3DVec3(worldPosition) - _btBody->getCenterOfMassPosition();
    _btBody->applyForce(BTVector3FromA3DVec3(force), relativePosition);
}

void BulletBodyProxy::applyCentralForce(const vec3& force) {

    ActivateDynamicBody(*_btBody);
    _btBody->applyCentralForce(BTVector3FromA3DVec3(force));
}

void BulletBodyProxy::applyImpulse(const vec3& impulse, const vec3& worldPosition) {

    ActivateDynamicBody(*_btBody);
    const btVector3 relativePosition = BTVector3FromA3DVec3(worldPosition) - _btBody->getCenterOfMassPosition();
    _btBody->applyImpulse(BTVector3FromA3DVec3(impulse), relativePosition);
}

void BulletBodyProxy::applyCentralImpulse(const vec3& impulse) {

    ActivateDynamicBody(*_btBody);
    _btBody->applyCentralImpulse(BTVector3FromA3DVec3(impulse));
}

void BulletBodyProxy::applyTorque(const vec3& torque) {

    ActivateDynamicBody(*_btBody);
    _btBody->applyTorque(BTVector3FromA3DVec3(torque));
}

void BulletBodyProxy::applyTorqueImpulse(const vec3& torque) {

    ActivateDynamicBody(*_btBody);
    _btBody->applyTorqueImpulse(BTVector3FromA3DVec3(torque));
}

vec3 BulletBodyProxy::totalForce() const {
    return A3DVec3FromBTVector3(_btBody->getTotalForce());
}

vec3 BulletBodyProxy::totalTorque() const {
    return A3DVec3FromBTVector3(_btBody->getTotalTorque());
}

void BulletBodyProxy::ccdEnabled(bool enabled) {

    _ccdEnabled = enabled;
    syncCcdSettings();
}

bool BulletBodyProxy::ccdEnabled() const {

    return _ccdEnabled;
}

void BulletBodyProxy::ccdMotionThreshold(float distance) {

    if (distance < 0.0f) {
        distance = 0.0f;
    }

    _ccdMotionThreshold = distance;
    syncCcdSettings();
}

float BulletBodyProxy::ccdMotionThreshold() const {

    return _ccdMotionThreshold;
}

void BulletBodyProxy::ccdSweptSphereRadius(float radius) {

    if (radius < 0.0f) {
        radius = 0.0f;
    }

    _ccdSweptSphereRadius = radius;
    syncCcdSettings();
}

float BulletBodyProxy::ccdSweptSphereRadius() const {

    return _ccdSweptSphereRadius;
}

bool BulletBodyProxy::affectedByGravity() const {
    return !(_btBody->getFlags() & BT_DISABLE_WORLD_GRAVITY);
}

void BulletBodyProxy::affectedByGravity(bool affectedByGravity) {

    auto flags = _btBody->getFlags();

    if (affectedByGravity) {

        _btBody->setFlags(flags & ~BT_DISABLE_WORLD_GRAVITY);

        // if the body is already in a world, restore that world's current
        // gravity immediately. otherwise Bullet will assign it when added.
        if (_btBody->isInWorld()) {
            if (auto world = _body->physicsWorld()) {
                _btBody->setGravity(BTVector3FromA3DVec3(world->gravity()));
            }
        }

        ActivateDynamicBody(*_btBody);
    }
    else {

        _btBody->setFlags(flags | BT_DISABLE_WORLD_GRAVITY);
        _btBody->setGravity(btVector3 {0, 0, 0});
    }
}

bool BulletBodyProxy::allowsResting() const {
    return _btBody->getActivationState() != DISABLE_DEACTIVATION;
}

void BulletBodyProxy::allowsResting(bool allowsResting) {

    if (type() == PhysicsBody::Type::Kinematic && allowsResting) {
        throw logic_error("Cannot enable resting for kinematic bodies.");
    }

    if (allowsResting) {
        _btBody->forceActivationState(ACTIVE_TAG);
        _btBody->activate(true);
    }
    else {
        _btBody->forceActivationState(DISABLE_DEACTIVATION);
    }
}

bool BulletBodyProxy::resting() const {
    return _btBody->getActivationState() == ISLAND_SLEEPING;
}

void BulletBodyProxy::resting(bool resting) {
    _btBody->setActivationState(resting ? ISLAND_SLEEPING : ACTIVE_TAG);
    if (!resting) {
        _btBody->activate(true);
    }
}

void BulletBodyProxy::worldTransform(const mat4& transform) {

    btTransform btTransform = _motionState->centerOfMassWorldTransform(transform);

    // if (auto* ms = _btBody->getMotionState()) {
    //     ms->setWorldTransform(btTransform);
    // }

    _btBody->proceedToTransform(btTransform);
    _btBody->activate(true);

    // Broadphase update: critical for kinematic/static teleports.
    // Without this, the AABB in the broadphase can lag, and collisions "randomly" miss.
    if (PhysicsBodyProxy::_body && PhysicsBodyProxy::_body->physicsWorld()) {
        if (auto btWorldProxy =
                dynamic_cast<BulletWorldProxy*>(PhysicsBodyProxy::_body->physicsWorld()->proxy())) {
            auto btWorld = btWorldProxy->btWorld();
            // !!! THIS NEEDS TO LOCK _btMutex IN BulletWorldProxy !!!
            // AND: if (_btBody->isInWorld() && _btBody->getBroadphaseHandle()) {
            if (_btBody->isInWorld() && _btBody->getBroadphaseHandle()) {
                btWorld->updateSingleAabb(_btBody.get());
            }

            // OPTIONAL but recommended if you "teleport" large distances:
            // clears stale overlapping pairs that can persist after big jumps.
            if (auto* proxy = _btBody->getBroadphaseHandle()) {
                // !!! THIS NEEDS TO LOCK _btMutex IN BulletWorldProxy !!!
                // ALSO: kills performance for normal kinemetic movement --
                // only do for large steps:
                /*
				 * btTransform prev = _btBody->getWorldTransform();
					btVector3 dp = btTransform.getOrigin() - prev.getOrigin();
					const bool teleported = dp.length2() > (2.0f * 2.0f); // e.g. >2m jump, tune

					// ...
					btWorld->updateSingleAabb(_btBody.get());

					if (teleported) {
						if (auto* proxy = _btBody->getBroadphaseHandle()) {
							btWorld->getBroadphase()->getOverlappingPairCache()
								  ->cleanProxyFromPairs(proxy, btWorld->getDispatcher());
						}
					}
				 */

                // ^^ so, disabling this FOR now since no teleporting:
                //				btWorld->getBroadphase()->getOverlappingPairCache()
                //						->cleanProxyFromPairs(proxy, btWorld->getDispatcher());
            }
        }
    }
}

void BulletBodyProxy::autocalculatesCenterOfMass(bool autocalculate) {

    if (_autocalculatesCenterOfMass == autocalculate) {
        return;
    }

    PhysicsBodyProxy::autocalculatesCenterOfMass(autocalculate);

    if (!autocalculate) {
        return;
    }

    calculateCenterOfMass();
    rebuildCenterOfMassOffsetShape();

    if (type() == PhysicsBody::Type::Dynamic && _autocalculatesMomentOfInertia) {
        calculateMomentOfInertia();
    }
}

void BulletBodyProxy::centerOfMassCalculation(PhysicsBody::CenterOfMassCalculation calculation) {

    if (_centerOfMassCalculation == calculation) {
        return;
    }

    PhysicsBodyProxy::centerOfMassCalculation(calculation);

    if (!_autocalculatesCenterOfMass) {
        return;
    }

    calculateCenterOfMass();
    rebuildCenterOfMassOffsetShape();

    if (type() == PhysicsBody::Type::Dynamic && _autocalculatesMomentOfInertia) {
        calculateMomentOfInertia();
    }
}

void BulletBodyProxy::clearForces() {
    _btBody->clearForces();
}

// [Internal Member Functions]

btRigidBody* BulletBodyProxy::btBody() {
    return _btBody.get();
}

// [Private Member Functions]

void BulletBodyProxy::calculateCenterOfMass() {

    _centerOfMass = vec3 {0.0f};

    auto* shapeProxy = dynamic_cast<BulletShapeProxy*>(_shapeProxy);
    if (!shapeProxy || shapeProxy->btShapes().empty()) {
        return;
    }

    auto* rootShape = shapeProxy->btShapes().front().get();
    if (!rootShape) {
        return;
    }

    switch (_centerOfMassCalculation) {

        case PhysicsBody::CenterOfMassCalculation::BoundsCenter: {

            btTransform identity;
            identity.setIdentity();

            btVector3 aabbMin;
            btVector3 aabbMax;

            rootShape->getAabb(identity, aabbMin, aabbMax);

            const vec3 centerOfMass = A3DVec3FromBTVector3((aabbMin + aabbMax) * btScalar(0.5));

            _centerOfMass = centerOfMass;
            break;
        }
    }
}

void BulletBodyProxy::rebuildCenterOfMassOffsetShape() {

    // the rigid body may currently point at _centerOfMassOffsetShape, so detach it
    // before destroying/replacing the wrapper.
    _btBody->setCollisionShape(nullptr);
    _centerOfMassOffsetShape.reset();

    auto* shapeProxy = dynamic_cast<BulletShapeProxy*>(_shapeProxy);
    if (!shapeProxy || shapeProxy->btShapes().empty()) {
        return;
    }

    auto* rootShape = shapeProxy->btShapes().front().get();
    if (!rootShape) {
        return;
    }

    const bool centered = _centerOfMass.x == 0.0f && _centerOfMass.y == 0.0f && _centerOfMass.z == 0.0f;

    if (centered) {
        _btBody->setCollisionShape(rootShape);
        return;
    }

    _centerOfMassOffsetShape = make_unique<btCompoundShape>(false);

    btTransform childTransform;
    childTransform.setIdentity();
    childTransform.setOrigin(-BTVector3FromA3DVec3(_centerOfMass));

    _centerOfMassOffsetShape->addChildShape(childTransform, rootShape);

    _btBody->setCollisionShape(_centerOfMassOffsetShape.get());
}

void BulletBodyProxy::calculateMomentOfInertia() {

    auto* shape = _btBody->getCollisionShape();
    if (!shape) {
        log::w()("Missing btCollisionShape.");
        return;
    }

    const auto mass = _body->mass();

    btVector3 localInertia;
    shape->calculateLocalInertia(mass, localInertia);

    SetMassPropsPreservingType(*_btBody, type(), mass, localInertia);
}

void BulletBodyProxy::syncCcdSettings() {

    if (!_btBody) {
        return;
    }

    if (_ccdEnabled) {
        _btBody->setCcdMotionThreshold(static_cast<btScalar>(_ccdMotionThreshold));
        _btBody->setCcdSweptSphereRadius(static_cast<btScalar>(_ccdSweptSphereRadius));
    }
    else {
        _btBody->setCcdMotionThreshold(btScalar(0.0));
        _btBody->setCcdSweptSphereRadius(btScalar(0.0));
    }
}

void BulletBodyProxy::syncRollingFrictionAnisotropy() {

    btVector3 direction {1.0f, 1.0f, 1.0f};

    if (_btBody->getRollingFriction() > 0.0f) {

        if (auto* bulletProxy = dynamic_cast<BulletShapeProxy*>(_shapeProxy);
            bulletProxy && !bulletProxy->btShapes().empty()) {

            if (auto* rootShape = bulletProxy->btShapes().front().get()) {
                direction = rootShape->getAnisotropicRollingFrictionDirection();
            }
        }
    }

    _btBody->setAnisotropicFriction(direction, btCollisionObject::CF_ANISOTROPIC_ROLLING_FRICTION);
}

namespace {

    // [Private Non-Member Functions]

    int CollisionFlagsForBodyType(int flags, PhysicsBody::Type type) {
        flags &= ~(btCollisionObject::CF_STATIC_OBJECT | btCollisionObject::CF_KINEMATIC_OBJECT);

        switch (type) {
            case PhysicsBody::Type::Static:
                return flags | btCollisionObject::CF_STATIC_OBJECT;

            case PhysicsBody::Type::Kinematic:
                return flags | btCollisionObject::CF_KINEMATIC_OBJECT;

            case PhysicsBody::Type::Dynamic:
                return flags;
        }

        return flags;
    }

    void SetMassPropsPreservingType(btRigidBody&      body,
                                    PhysicsBody::Type type,
                                    btScalar          mass,
                                    const btVector3&  inertia) {
        body.setMassProps(mass, inertia);
        body.setCollisionFlags(CollisionFlagsForBodyType(body.getCollisionFlags(), type));
        body.updateInertiaTensor();
    }

    void ForceActivationForBodyType(btRigidBody& body, PhysicsBody::Type type) {
        switch (type) {
            case PhysicsBody::Type::Static:
            case PhysicsBody::Type::Dynamic:
                body.forceActivationState(ACTIVE_TAG);
                break;

            case PhysicsBody::Type::Kinematic:
                body.forceActivationState(DISABLE_DEACTIVATION);
                break;
        }
    }

    void ActivateDynamicBody(btRigidBody& body) {

        if (!body.isStaticOrKinematicObject()) {
            body.activate(true);
        }
    }

} // namespace
} // namespace a3d
