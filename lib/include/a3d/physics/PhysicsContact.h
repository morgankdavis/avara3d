//
//  PhysicsContact.h
//  avara3d
//
//  Created by Morgan Davis on 1/25/18.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_PHYSICS_PHYSICSCONTACT_H
#define AVARA3D_PHYSICS_PHYSICSCONTACT_H

#include <memory>

#include "a3d/Math.h"

namespace a3d {
class Node;

/**
 * @brief Describes a contact between two physics bodies.
 *
 * The participating nodes are retained weakly. When a body pair has multiple
 * underlying contact points, PhysicsContact represents the point with the
 * greatest collision impulse, using deepest penetration as a tie-breaker.
 * Contact persistence may keep a logical contact active across very small
 * separations used to maintain stable simulation contact.
 */
class PhysicsContact {

public:
    // [Public Member Functions]

    /** @brief Returns the first contact node as a weak reference. */
    std::weak_ptr<Node> nodeA() const;

    /** @brief Returns the second contact node as a weak reference. */
    std::weak_ptr<Node> nodeB() const;

    /** @brief Returns the world-space midpoint between the two body contact points. */
    const math::vec3&   contactPoint() const;

    /** @brief Returns the world-space contact normal pointing from nodeB() toward nodeA(). */
    const math::vec3&   contactNormal() const;

    /** @brief Returns the collision impulse associated with the represented contact point. */
    float               collisionImpulse() const;

    /** @brief Returns the penetration depth, where zero indicates no penetration and positive values indicate overlap. */
    float               penetrationDistance() const;

    /** @brief Returns the sweep-test fraction associated with this contact; ordinary contact results use zero. */
    float               sweepTestFraction() const;

    // [Internal Lifecycle Functions]

    PhysicsContact(std::weak_ptr<Node> nodeA,
                   std::weak_ptr<Node> nodeB,
                   const math::vec3&   contactPoint,
                   const math::vec3&   contactNormal,
                   float               collisionImpulse,
                   float               penetrationDistance,
                   float               sweepTestFraction);

private:
    // [Private Member Variables]

    std::weak_ptr<Node> _nodeA;
    std::weak_ptr<Node> _nodeB;
    math::vec3          _contactPoint;
    math::vec3          _contactNormal;
    float               _collisionImpulse;
    float               _penetrationDistance;
    float               _sweepTestFraction;
};
} // namespace a3d

#endif // AVARA3D_PHYSICS_PHYSICSCONTACT_H
