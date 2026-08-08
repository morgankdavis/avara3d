//
//  BulletBodyProxy.cc
//  avara3d
//
//  Created by Morgan Davis on 10/29/23.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#include "a3d/physics/backend/bullet/BulletBodyProxy.h"

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

using namespace a3d;
using namespace a3d::math;
using namespace std;

namespace {

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

} // namespace

/// Internal Lifecycle Functions ///

BulletBodyProxy::BulletBodyProxy(PhysicsBody& body, PhysicsBody::Type type):
    PhysicsBodyProxy {body, type},
    _btBody {},
        /*_btMotionState(nullptr)*/
    _motionState {},
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
    rigidBodyInfo.m_rollingFriction = 0.05; // don't roll forever
    rigidBodyInfo.m_spinningFriction = 0.05; // don't spin forever
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

/// PhysicsBodyModelProxy Internal Member Functions ///

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

    if (proxy) {
        // front is either the only btCollisionShape or a btCompound shape with child shapes at index 1+

        if (auto btShape = dynamic_cast<BulletShapeProxy*>(proxy)->btShapes().front().get()) {

            _btBody->setCollisionShape(btShape);

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

            if (this->type() == PhysicsBody::Type::Dynamic && _autocalculatesMomentOfInertia) {
                calculateMomentOfIntertia();
            }
        }
        else {
            log::e()("Could not get shape resources.");
            _btBody->setCollisionShape(nullptr);
            _shapeProxy = nullptr;
        }
    }
    else {
        _btBody->setCollisionShape(nullptr);
        _shapeProxy = nullptr;
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
    return A3DVec3FromBTVector3(_btBody->getCenterOfMassPosition());
}

void BulletBodyProxy::centerOfMass(const vec3& offset) {
    _btBody->setCenterOfMassTransform(BTTransformFromA3DMat4(translate(mat4(1.0), offset)));
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
}

float BulletBodyProxy::restitution() const {
    return _btBody->getRestitution();
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
    const btVector3 relativePosition = BTVector3FromA3DVec3(worldPosition) - _btBody->getCenterOfMassPosition();
    _btBody->applyForce(BTVector3FromA3DVec3(force), relativePosition);
}

void BulletBodyProxy::applyCentralForce(const vec3& force) {
    _btBody->applyCentralForce(BTVector3FromA3DVec3(force));
}

void BulletBodyProxy::applyImpulse(const vec3& impulse, const vec3& worldPosition) {
    const btVector3 relativePosition = BTVector3FromA3DVec3(worldPosition) - _btBody->getCenterOfMassPosition();
    _btBody->applyImpulse(BTVector3FromA3DVec3(impulse), relativePosition);
}

void BulletBodyProxy::applyCentralImpulse(const vec3& impulse) {
    _btBody->applyCentralImpulse(BTVector3FromA3DVec3(impulse));
}

void BulletBodyProxy::applyTorque(const vec3& torque) {
    _btBody->applyTorque(BTVector3FromA3DVec3(torque));
}

void BulletBodyProxy::applyTorqueImpulse(const vec3& torque) {
    _btBody->applyTorqueImpulse(BTVector3FromA3DVec3(torque));
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
    // *** test this ***
    auto gravity = _btBody->getGravity();
    return (gravity.x() != 0) || (gravity.y() != 0) || (gravity.z() != 0);
}

vec3 BulletBodyProxy::totalForce() const {
    return A3DVec3FromBTVector3(_btBody->getTotalForce());
}

vec3 BulletBodyProxy::totalTorque() const {
    return A3DVec3FromBTVector3(_btBody->getTotalTorque());
}

void BulletBodyProxy::affectedByGravity(bool affectedByGravity) {

    // *** test this ***
    _btBody->setGravity(affectedByGravity ? btVector3 {1.0, 1.0, 1.0} : btVector3 {0, 0, 0});
}

bool BulletBodyProxy::allowsResting() const {
    return _btBody->getActivationState() != DISABLE_DEACTIVATION;
}

void BulletBodyProxy::allowsResting(bool allowsResting) {

    if (type() == PhysicsBody::Type::Kinematic && allowsResting) {
        log::e()("Cannot enable resting for kinematic bodies.");
        return;
    }

    _btBody->setActivationState(allowsResting ? ACTIVE_TAG : DISABLE_DEACTIVATION);
    _btBody->activate(true);
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

//void BulletBodyProxy::worldTransform(const mat4& transform) {
//	_btBody->setWorldTransform(BTTransformFromA3DMat4(transform));
//}

void BulletBodyProxy::worldTransform(const mat4& transform) {

    btTransform btTransform = BTTransformFromA3DMat4(transform);

    // Keep BOTH the rigid body and its motion state consistent.
    // (Bullet uses these differently depending on type and interpolation.)
    if (auto* ms = _btBody->getMotionState()) {
        ms->setWorldTransform(btTransform);
    }

    _btBody->setWorldTransform(btTransform);
    _btBody->setInterpolationWorldTransform(btTransform);

    // If you're externally driving this body (kinematic/static), make sure it's awake.
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

void BulletBodyProxy::clearForces() {
    _btBody->clearForces();
}

/// Internal Member Functions ///

btRigidBody* BulletBodyProxy::btBody() {
    return _btBody.get();
}

/// Private Member Functions ///

void BulletBodyProxy::calculateMomentOfIntertia() {

    if (auto btShapeModel = dynamic_cast<BulletShapeProxy*>(_shapeProxy)) {
        if (auto btShape = btShapeModel->btShapes().front().get()) {
            btVector3 localInertia;
            auto      mass = _body->mass();
            btShape->calculateLocalInertia(mass, localInertia);
            SetMassPropsPreservingType(*_btBody, type(), mass, localInertia);
        }
        else {
            log::w()("Missing btCollisionShape.");
        }
    }
    //	else {
    //		log::w()("Missing PhysicsShapeModelProxy.");
    //	}
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
