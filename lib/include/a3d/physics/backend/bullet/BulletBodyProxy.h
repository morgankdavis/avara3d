//
//  BulletBodyProxy.h
//  avara3d
//
//  Created by Morgan Davis on 10/29/23.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_PHYSICS_BACKEND_BULLET_BULLETBODYPROXY_H
#define AVARA3D_PHYSICS_BACKEND_BULLET_BULLETBODYPROXY_H

#include <memory>

#include "a3d/physics/proxy/PhysicsBodyProxy.h"

class btCompoundShape;
class btRigidBody;

namespace a3d {

    class BulletMotionState;

    class BulletBodyProxy : public PhysicsBodyProxy {

    public:
        /// Internal Lifecycle Functions ///

        explicit BulletBodyProxy(PhysicsBody& body, PhysicsBody::Type type);
        ~BulletBodyProxy() override;

        /// PhysicsBodyModelProxy Internal Member Functions ///

        PhysicsBody::Type  type() const override;
        void               type(PhysicsBody::Type type) override;

        PhysicsShapeProxy* shapeProxy() const override;
        void               shapeProxy(PhysicsShapeProxy* proxy) override;

        float              mass() const override;
        void               mass(float mass) override;

        math::vec3         momentOfInertia() const override;
        void               momentOfInertia(const math::vec3& moment) override;

        math::vec3         centerOfMass() const override;
        void               centerOfMass(const math::vec3& offset) override;

        float              friction() const override;
        void               friction(float friction) override;

        float              rollingFriction() const override;
        void               rollingFriction(float friction) override;

        float              restitution() const override;
        void               restitution(float restitution) override;

        math::vec3         linearVelocity() const override;
        void               linearVelocity(const math::vec3& velocity) override;

        math::vec3         angularVelocity() const override;
        void               angularVelocity(const math::vec3& velocity) override;

        math::vec3         linearFactor() const override;
        void               linearFactor(const math::vec3& factor) override;

        math::vec3         angularFactor() const override;
        void               angularFactor(const math::vec3& factor) override;

        float              linearDamping() const override;
        void               linearDamping(float damping) override;

        float              angularDamping() const override;
        void               angularDamping(float damping) override;

        float              linearSleepingThreshold() const override;
        void               linearSleepingThreshold(float threshold) override;

        float              angularSleepingThreshold() const override;
        void               angularSleepingThreshold(float threshold) override;

        void               applyForce(const math::vec3& force, const math::vec3& worldPosition) override;
        void               applyCentralForce(const math::vec3& force) override;

        void               applyImpulse(const math::vec3& impulse, const math::vec3& worldPosition) override;
        void               applyCentralImpulse(const math::vec3& impulse) override;

        void               applyTorque(const math::vec3& torque) override;
        void               applyTorqueImpulse(const math::vec3& torque) override;

        math::vec3         totalForce() const override;
        math::vec3         totalTorque() const override;

        void               ccdEnabled(bool enabled) override;
        bool               ccdEnabled() const override;

        void               ccdMotionThreshold(float distance) override;
        float              ccdMotionThreshold() const override;

        void               ccdSweptSphereRadius(float radius) override;
        float              ccdSweptSphereRadius() const override;

        bool               affectedByGravity() const override;
        void               affectedByGravity(bool affectedByGravity) override;

        bool               allowsResting() const override;
        void               allowsResting(bool allowsResting) override;

        bool               resting() const override;
        void               resting(bool resting) override;

        void               worldTransform(const math::mat4& transform) override;

        void               autocalculatesCenterOfMass(bool autocalculate) override;

        void               centerOfMassCalculation(PhysicsBody::CenterOfMassCalculation calculation) override;

        void               clearForces() override;

        /// Internal Member Functions ///

        btRigidBody*       btBody();

    private:
        ///  Private Member Functions ///

        void                               calculateCenterOfMass();
        void                               rebuildCenterOfMassOffsetShape();
        void                               calculateMomentOfInertia();
        void                               syncCcdSettings();

        /// Private Member Variables ///

        std::unique_ptr<BulletMotionState> _motionState;
        std::unique_ptr<btCompoundShape>   _centerOfMassOffsetShape;
        std::unique_ptr<btRigidBody>       _btBody;
        bool                               _ccdEnabled;
        float                              _ccdMotionThreshold;
        float                              _ccdSweptSphereRadius;
    };

}

#endif //AVARA3D_PHYSICS_BACKEND_BULLET_BULLETBODYPROXY_H
