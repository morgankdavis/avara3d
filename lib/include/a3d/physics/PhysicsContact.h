//
//  PhysicsContact.h
//  avara3d
//
//  Created by Morgan Davis on 1/25/18.
//  Copyright © 2018-2026 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_PHYSICS_PHYSICSCONTACT_H
#define AVARA3D_PHYSICS_PHYSICSCONTACT_H

#include <memory>

#include "a3d/Math.h"

namespace a3d {

    class Node;

    // TODO: Doxygen:
    // - If a body pair has multiple underlying contact points, PhysicsContact represents
    //   the point with the greatest applied impulse, using deepest penetration as a tie-breaker.
    // - penetrationDistance() is 0 when there is no penetration and positive for penetration.
    // - Contact lifetime follows the physics backend's persistent contact semantics, so a contact may
    //   remain active across very small separations used to maintain stable contact persistence.
    // - contactNormal() points from nodeB() toward nodeA().
    // - contactPoint() is the midpoint between the two underlying body contact points.

    class PhysicsContact {

    public:
        /// Public Member Functions ///

        std::weak_ptr<Node> nodeA() const;
        std::weak_ptr<Node> nodeB() const;
        const math::vec3&   contactPoint() const;
        const math::vec3&   contactNormal() const;
        float               collisionImpulse() const;
        float               penetrationDistance() const;
        float               sweepTestFraction() const;

        /// Internal Lifecycle Functions ///

        PhysicsContact(std::weak_ptr<Node> nodeA,
                       std::weak_ptr<Node> nodeB,
                       const math::vec3&   contactPoint,
                       const math::vec3&   contactNormal,
                       float               collisionImpulse,
                       float               penetrationDistance,
                       float               sweepTestFraction);

    private:
        /// Private Member Variables ///

        std::weak_ptr<Node> _nodeA;
        std::weak_ptr<Node> _nodeB;
        math::vec3          _contactPoint;
        math::vec3          _contactNormal;
        float               _collisionImpulse;
        float               _penetrationDistance;
        float               _sweepTestFraction;
    };

}

#endif /* AVARA3D_PHYSICS_PHYSICSCONTACT_H */
