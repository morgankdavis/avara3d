//
//  PhysicsBody.h
//  avara3d
//
//  Created by Morgan Davis on 1/26/18.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_PHYSICS_PHYSICSBODY_H
#define AVARA3D_PHYSICS_PHYSICSBODY_H

#include <memory>

#include "a3d/Math.h"

namespace a3d {

    class Mesh;
    class Node;
    class PhysicsWorld;
    class PhysicsBodyProxy;
    class PhysicsShapeProxy;
    class PhysicsShape;
    class Scene;

    /**
     * @brief Rigid-body simulation state attached to a Node.
     *
     * A Node owns its PhysicsBody, while the body retains its PhysicsShape with
     * shared ownership so a shape may be reused by multiple bodies. Dynamic bodies
     * are driven by simulation, kinematic bodies are moved explicitly through their
     * Node transform, and static bodies represent fixed collision geometry.
     *
     * When a body without an explicit shape is attached to a Node, A3D attempts to
     * derive a suitable collision shape from the node's mesh or hierarchy.
     */
    class PhysicsBody {

    public:
        // [Public Types]

        /** @brief Determines how a rigid body participates in simulation. */
        enum class Type : uint8_t {
            Static,   ///< Fixed collision body that does not respond dynamically to forces.
            Dynamic,  ///< Simulation-driven body affected by forces, gravity, and collisions.
            Kinematic ///< Explicitly moved body that participates in collisions without dynamic motion.
        };

        /** @brief Selects the method used to automatically determine a dynamic body's center of mass. */
        enum class CenterOfMassCalculation : uint8_t {
            BoundsCenter, ///< Uses the center of the collision shape's local bounds.
            // VolumeCentroid
        };

        // [Public Static Member Functions]

        /** @brief Creates a shape-less static body. */
        static std::unique_ptr<PhysicsBody> StaticBody();

        /**
         * @brief Creates a static body using @p shape.
         *
         * @throws std::logic_error if @p shape is incompatible with a static body, its
         * source has expired, or valid collision geometry cannot be created from it.
         * @throws std::invalid_argument if @p shape is derived from a Mesh with no elements.
         */
        static std::unique_ptr<PhysicsBody> StaticBody(const std::shared_ptr<PhysicsShape>& shape);

        /** @brief Creates a shape-less dynamic body. */
        static std::unique_ptr<PhysicsBody> DynamicBody();

        /**
         * @brief Creates a dynamic body using @p shape.
         *
         * @throws std::logic_error if @p shape is incompatible with a dynamic body, its
         * source has expired, or valid collision geometry cannot be created from it.
         * @throws std::invalid_argument if @p shape is derived from a Mesh with no elements.
         */
        static std::unique_ptr<PhysicsBody> DynamicBody(const std::shared_ptr<PhysicsShape>& shape);

        /** @brief Creates a shape-less kinematic body. */
        static std::unique_ptr<PhysicsBody> KinematicBody();

        /**
         * @brief Creates a kinematic body using @p shape.
         *
         * @throws std::logic_error if @p shape is incompatible with a kinematic body, its
         * source has expired, or valid collision geometry cannot be created from it.
         * @throws std::invalid_argument if @p shape is derived from a Mesh with no elements.
         */
        static std::unique_ptr<PhysicsBody> KinematicBody(const std::shared_ptr<PhysicsShape>& shape);

        // [Public Lifecycle Functions]

        /** @brief Creates a shape-less body of @p type. */
        explicit PhysicsBody(Type type);

        /**
         * @brief Creates a body of @p type using @p shape.
         *
         * @throws std::logic_error if @p shape is incompatible with @p type, its source
         * has expired, or valid collision geometry cannot be created from it.
         * @throws std::invalid_argument if @p shape is derived from a Mesh with no elements.
         */
        PhysicsBody(Type type, const std::shared_ptr<PhysicsShape>& shape);

        PhysicsBody(const PhysicsBody&)            = delete;
        PhysicsBody& operator=(const PhysicsBody&) = delete;

        PhysicsBody(PhysicsBody&&)            = delete;
        PhysicsBody& operator=(PhysicsBody&&) = delete;

        ~PhysicsBody();

        // [Public Member Functions]

        /** @brief Returns the body type. */
        Type                                 type() const;

        /**
         * @brief Validates the requested body type.
         *
         * Changing a body's type after construction is not currently supported.
         *
         * @throws std::logic_error if @p type differs from the current type.
         */
        void                                 type(Type type);

        /** @brief Returns the collision shape retained by this body, or nullptr if no shape is assigned. */
        const std::shared_ptr<PhysicsShape>& shape() const;

        /**
         * @brief Replaces the collision shape retained by this body; nullptr removes it.
         *
         * @throws std::logic_error if @p shape does not support this body's type, its
         * source has expired, or valid collision geometry cannot be created from it.
         * @throws std::invalid_argument if @p shape is derived from a Mesh with no elements.
         */
        void                                 shape(const std::shared_ptr<PhysicsShape>& shape);

        /** @brief Returns the body mass. */
        float                                mass() const;

        /**
         * @brief Sets the mass of a dynamic body.
         *
         * @throws std::logic_error if the body is not dynamic.
         * @throws std::invalid_argument if @p mass is less than or equal to zero.
         */
        void                                 mass(float mass);

        /** @brief Returns the body's local principal-axis moment of inertia. */
        math::vec3                           momentOfInertia() const;

        /** @brief Sets the local principal-axis moment of inertia when automatic calculation is disabled. */
        void                                 momentOfInertia(const math::vec3& moment);

        /** @brief Returns the center-of-mass offset from the owning node's origin in local coordinates. */
        math::vec3                           centerOfMass() const;

        /**
         * @brief Sets a dynamic body's local center-of-mass offset and disables automatic calculation.
         *
         * @throws std::logic_error if the body is not dynamic.
         */
        void                                 centerOfMass(const math::vec3& offset);

        /** @brief Returns the sliding-friction coefficient. */
        float                                friction() const;

        /** @brief Sets the sliding-friction coefficient. */
        void                                 friction(float friction);

        /** @brief Returns the rolling-friction coefficient. */
        float                                rollingFriction() const;

        /** @brief Sets the rolling-friction coefficient. */
        void                                 rollingFriction(float friction);

        /** @brief Returns the spinning-friction coefficient. */
        float                                spinningFriction() const;

        /** @brief Sets the spinning-friction coefficient. */
        void                                 spinningFriction(float friction);

        /** @brief Returns the collision restitution coefficient. */
        float                                restitution() const;

        /** @brief Sets the collision restitution coefficient. */
        void                                 restitution(float restitution);

        /** @brief Returns the body linear velocity in world coordinates. */
        math::vec3                           linearVelocity() const;

        /** @brief Sets the body linear velocity in world coordinates. */
        void                                 linearVelocity(const math::vec3& velocity);

        /** @brief Returns the body angular velocity in radians per second. */
        math::vec3                           angularVelocity() const;

        /** @brief Sets the body angular velocity in radians per second. */
        void                                 angularVelocity(const math::vec3& velocity);

        /** @brief Returns the per-axis factor applied to linear motion. */
        math::vec3                           linearFactor() const;

        /** @brief Sets the per-axis factor applied to linear motion. */
        void                                 linearFactor(const math::vec3& factor);

        /** @brief Returns the per-axis factor applied to angular motion. */
        math::vec3                           angularFactor() const;

        /** @brief Sets the per-axis factor applied to angular motion. */
        void                                 angularFactor(const math::vec3& factor);

        /** @brief Returns the linear damping coefficient. */
        float                                linearDamping() const;

        /** @brief Sets the linear damping coefficient. */
        void                                 linearDamping(float damping);

        /** @brief Returns the angular damping coefficient. */
        float                                angularDamping() const;

        /** @brief Sets the angular damping coefficient. */
        void                                 angularDamping(float damping);

        /** @brief Returns the linear-speed threshold used when deciding whether the body may rest. */
        float                                linearSleepingThreshold() const;

        /** @brief Sets the linear-speed threshold used when deciding whether the body may rest. */
        void                                 linearSleepingThreshold(float threshold);

        /** @brief Returns the angular-speed threshold used when deciding whether the body may rest. */
        float                                angularSleepingThreshold() const;

        /** @brief Sets the angular-speed threshold used when deciding whether the body may rest. */
        void                                 angularSleepingThreshold(float threshold);

        /**
         * @brief Applies a central force or impulse to a dynamic body.
         *
         * @param force world-space force vector, or impulse vector when @p impulse is true.
         * @param impulse when true, applies @p force as an instantaneous impulse instead of a continuous force.
         * @throws std::logic_error if the body is not dynamic.
         */
        void                                 applyForce(const math::vec3& force, bool impulse);

        /**
         * @brief Applies a force or impulse at a world-space position on a dynamic body.
         *
         * @param force world-space force vector, or impulse vector when @p impulse is true.
         * @param location world-space application position.
         * @param impulse when true, applies @p force as an instantaneous impulse instead of a continuous force.
         * @throws std::logic_error if the body is not dynamic.
         */
        void                    applyForce(const math::vec3& force, const math::vec3& location, bool impulse);

        /**
         * @brief Applies torque or a torque impulse to a dynamic body.
         *
         * @param torque torque vector, or angular impulse when @p impulse is true.
         * @param impulse when true, applies @p torque as an instantaneous angular impulse.
         * @throws std::logic_error if the body is not dynamic.
         */
        void                    applyTorque(const math::vec3& torque, bool impulse);

        /** @brief Returns the accumulated force for the current simulation step. */
        math::vec3              totalForce() const;

        /** @brief Returns the accumulated torque for the current simulation step. */
        math::vec3              totalTorque() const;

        /** @brief Clears accumulated forces and torques without changing the body's velocities. */
        void                    clearForces();

        /** @brief Enables or disables continuous collision detection for this body. */
        void                    ccdEnabled(bool enabled);

        /** @brief Returns whether continuous collision detection is enabled. */
        bool                    ccdEnabled() const;

        /** @brief Sets the motion-distance threshold used by continuous collision detection; negative values become zero. */
        void                    ccdMotionThreshold(float distance);

        /** @brief Returns the configured continuous-collision motion threshold. */
        float                   ccdMotionThreshold() const;

        /** @brief Sets the swept-sphere radius used by continuous collision detection; negative values become zero. */
        void                    ccdSweptSphereRadius(float radius);

        /** @brief Returns the configured continuous-collision swept-sphere radius. */
        float                   ccdSweptSphereRadius() const;

        /** @brief Returns whether this body is affected by its PhysicsWorld gravity. */
        bool                    affectedByGravity() const;

        /** @brief Sets whether this body is affected by its PhysicsWorld gravity. */
        void                    affectedByGravity(bool affectedByGravity);

        /** @brief Returns whether the body is allowed to enter the resting state automatically. */
        bool                    allowsResting() const;

        /**
         * @brief Sets whether the body may enter the resting state automatically.
         *
         * Kinematic bodies cannot enable resting.
         *
         * @throws std::logic_error if @p allowsResting is true for a kinematic body.
         */
        void                    allowsResting(bool allowsResting);

        /** @brief Returns whether the body is currently resting. */
        bool                    resting() const;

        /** @brief Forces the body into or out of the resting state. */
        void                    resting(bool resting);

        /** @brief Returns whether the center of mass is calculated automatically. */
        bool                    autocalculatesCenterOfMass() const;

        /**
         * @brief Enables or disables automatic center-of-mass calculation.
         *
         * Enabling recomputes the center of mass. Disabling preserves the current
         * offset for subsequent manual adjustment.
         *
         * @throws std::logic_error if automatic calculation is enabled for a non-dynamic body.
         */
        void                    autocalculatesCenterOfMass(bool autocalculate);

        /** @brief Returns the method used for automatic center-of-mass calculation. */
        CenterOfMassCalculation centerOfMassCalculation() const;

        /**
         * @brief Sets the method used for automatic center-of-mass calculation.
         *
         * If automatic calculation is enabled, the center of mass is recomputed.
         *
         * @throws std::logic_error if the body is not dynamic.
         */
        void                    centerOfMassCalculation(CenterOfMassCalculation calculation);

        /** @brief Returns whether the moment of inertia is calculated automatically from mass and collision shape. */
        bool                    autocalculatesMomentOfInertia() const;

        /** @brief Enables or disables automatic moment-of-inertia calculation. */
        void                    autocalculatesMomentOfInertia(bool autocalculate);

        // [Internal Member Functions]

        void                    attachedToNode(const std::shared_ptr<Node>& node);
        void                    detachedFromNode(const std::shared_ptr<Node>& node);

        void                    meshAttachedToNode(const std::shared_ptr<Mesh>& mesh); // owning node's mesh
        void                    meshDetachedFromNode(const std::shared_ptr<Mesh>& mesh);

        void                    physicsWorldReachable(PhysicsWorld& world);
        void                    physicsWorldUnreachable(PhysicsWorld& world);

        void                    addedToWorld(PhysicsWorld& world);
        void                    removedFromWorld(PhysicsWorld& world);

        void                    shapeWillUpdate();
        void                    shapeDidUpdate();

        void                    syncTransformFromNode();

        std::weak_ptr<Node>     node() const;

        // the scene's world, if it exists.  not the same as _world.
        PhysicsWorld*           physicsWorld() const;

        PhysicsBodyProxy*       proxy() const;

    private:
        // [Private Member Functions]

        void                              checkAutocreateShape(const std::shared_ptr<Node>& node);
        void                              checkAutocreateShape(const std::shared_ptr<Mesh>& mesh);

        void                              checkAddToWorld();

        // [Private Member Variables]

        std::shared_ptr<PhysicsShape>     _shape;
        std::unique_ptr<PhysicsBodyProxy> _proxy;
        std::weak_ptr<Node>               _node;
        // either a pointer to the world we are currently in or null.
        PhysicsWorld*                     _world;
    };

}

#endif // AVARA3D_PHYSICS_PHYSICSBODY_H
