//
//  PhysicsBodyProxy.h
//  avara3d
//
//  Created by Morgan Davis on 11/13/23.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_PHYSICS_PROXY_PHYSICSBODYPROXY_H
#define AVARA3D_PHYSICS_PROXY_PHYSICSBODYPROXY_H

#include "a3d/physics/PhysicsBody.h"

namespace a3d {
class PhysicsBody;
class PhysicsShapeProxy;

class PhysicsBodyProxy {

public:
    // [Internal Lifecycle Functions]

    explicit PhysicsBodyProxy(PhysicsBody& body, PhysicsBody::Type type);

    PhysicsBodyProxy(const PhysicsBodyProxy&)            = delete;
    PhysicsBodyProxy& operator=(const PhysicsBodyProxy&) = delete;

    PhysicsBodyProxy(PhysicsBodyProxy&&)            = delete;
    PhysicsBodyProxy& operator=(PhysicsBodyProxy&&) = delete;

    virtual ~PhysicsBodyProxy();

    // [Internal Member Functions]

    virtual PhysicsBody::Type  type() const                 = 0;
    virtual void               type(PhysicsBody::Type type) = 0;

    virtual PhysicsShapeProxy* shapeProxy() const                   = 0;
    virtual void               shapeProxy(PhysicsShapeProxy* proxy) = 0;

    virtual float              mass() const     = 0;
    virtual void               mass(float mass) = 0;

    virtual math::vec3         momentOfInertia() const                   = 0;
    virtual void               momentOfInertia(const math::vec3& moment) = 0;

    virtual math::vec3         centerOfMass() const                   = 0;
    virtual void               centerOfMass(const math::vec3& offset) = 0;

    virtual float              friction() const         = 0;
    virtual void               friction(float friction) = 0;

    virtual float              rollingFriction() const         = 0;
    virtual void               rollingFriction(float friction) = 0;

    virtual float              spinningFriction() const         = 0;
    virtual void               spinningFriction(float friction) = 0;

    virtual float              restitution() const            = 0;
    virtual void               restitution(float restitution) = 0;

    virtual math::vec3         linearVelocity() const                     = 0;
    virtual void               linearVelocity(const math::vec3& velocity) = 0;

    virtual math::vec3         angularVelocity() const                     = 0;
    virtual void               angularVelocity(const math::vec3& velocity) = 0;

    virtual math::vec3         linearFactor() const                   = 0;
    virtual void               linearFactor(const math::vec3& factor) = 0;

    virtual math::vec3         angularFactor() const                   = 0;
    virtual void               angularFactor(const math::vec3& factor) = 0;

    virtual float              linearDamping() const        = 0;
    virtual void               linearDamping(float damping) = 0;

    virtual float              angularDamping() const        = 0;
    virtual void               angularDamping(float damping) = 0;

    virtual float              linearSleepingThreshold() const          = 0;
    virtual void               linearSleepingThreshold(float threshold) = 0;

    virtual float              angularSleepingThreshold() const          = 0;
    virtual void               angularSleepingThreshold(float threshold) = 0;

    virtual void               applyForce(const math::vec3& force, const math::vec3& worldPosition) = 0;
    virtual void               applyCentralForce(const math::vec3& force)                           = 0;

    virtual void               applyImpulse(const math::vec3& impulse, const math::vec3& worldPosition) = 0;
    virtual void               applyCentralImpulse(const math::vec3& impulse)                           = 0;

    virtual void               applyTorque(const math::vec3& torque)        = 0;
    virtual void               applyTorqueImpulse(const math::vec3& torque) = 0;

    virtual math::vec3         totalForce() const  = 0;
    virtual math::vec3         totalTorque() const = 0;

    virtual void               ccdEnabled(bool enabled) = 0;
    virtual bool               ccdEnabled() const       = 0;

    virtual void               ccdMotionThreshold(float distance) = 0;
    virtual float              ccdMotionThreshold() const         = 0;

    virtual void               ccdSweptSphereRadius(float radius) = 0;
    virtual float              ccdSweptSphereRadius() const       = 0;

    virtual bool               affectedByGravity() const                 = 0;
    virtual void               affectedByGravity(bool affectedByGravity) = 0;

    virtual bool               allowsResting() const             = 0;
    virtual void               allowsResting(bool allowsResting) = 0;

    virtual bool               resting() const       = 0;
    virtual void               resting(bool resting) = 0;

    virtual void               clearForces() = 0;

    virtual void               worldTransform(const math::mat4& worldTransform) = 0;

    virtual bool               autocalculatesCenterOfMass() const;
    virtual void               autocalculatesCenterOfMass(bool autocalculate);

    virtual PhysicsBody::CenterOfMassCalculation centerOfMassCalculation() const;
    virtual void centerOfMassCalculation(PhysicsBody::CenterOfMassCalculation calculation);

    virtual bool autocalculatesMomentOfInertia() const;
    virtual void autocalculatesMomentOfInertia(bool autocalculate);

    void         attachedToBody(PhysicsBody& body);
    void         detachedFromBody(PhysicsBody& body);

protected:
    // [Protected Member Variables]

    PhysicsBody*                         _body;
    PhysicsShapeProxy*                   _shapeProxy;
    math::vec3                           _centerOfMass;
    bool                                 _autocalculatesCenterOfMass;
    PhysicsBody::CenterOfMassCalculation _centerOfMassCalculation;
    bool                                 _autocalculatesMomentOfInertia;
};
} // namespace a3d

#endif // AVARA3D_PHYSICS_PROXY_PHYSICSBODYPROXY_H
