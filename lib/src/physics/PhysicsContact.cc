//
//  PhysicsContact.cc
//  avara3d
//
//  Created by Morgan Davis on 1/25/18.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#include "a3d/physics/PhysicsContact.h"

#include <utility>

using namespace a3d;
using namespace a3d::math;
using namespace std;

/// Public Member Functions ///

weak_ptr<Node> PhysicsContact::nodeA() const {
    return _nodeA;
}

weak_ptr<Node> PhysicsContact::nodeB() const {
    return _nodeB;
}

const vec3& PhysicsContact::contactPoint() const {
    return _contactPoint;
}

const vec3& PhysicsContact::contactNormal() const {
    return _contactNormal;
}

float PhysicsContact::collisionImpulse() const {
    return _collisionImpulse;
}

float PhysicsContact::penetrationDistance() const {
    return _penetrationDistance;
}

float PhysicsContact::sweepTestFraction() const {
    return _sweepTestFraction;
}

/// Internal Lifecycle Functions ///

PhysicsContact::PhysicsContact(weak_ptr<Node> nodeA,
                               weak_ptr<Node> nodeB,
                               const vec3&    contactPoint,
                               const vec3&    contactNormal,
                               float          collisionImpulse,
                               float          penetrationDistance,
                               float          sweepTestFraction):
    _nodeA {std::move(nodeA)},
    _nodeB {std::move(nodeB)},
    _contactPoint {contactPoint},
    _contactNormal {contactNormal},
    _collisionImpulse {collisionImpulse},
    _penetrationDistance {penetrationDistance},
    _sweepTestFraction {sweepTestFraction} {}
