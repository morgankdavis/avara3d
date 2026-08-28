//
//  SpherePhysicsShape.cc
//  avara3d
//
//  Created by Morgan Davis on 11/19/23.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#include "a3d/physics/shape/primitive/SpherePhysicsShape.h"

#include <stdexcept>

using namespace a3d;

// [Public Lifecycle Functions]

SpherePhysicsShape::SpherePhysicsShape(float radius):
    _radius {radius} {}

// [Public Member Functions]

float SpherePhysicsShape::radius() const {
    return _radius;
}

// [PhysicsShape Public Member Functions]

PhysicsShape::Type SpherePhysicsShape::type() const {
    return PhysicsShape::Type::Primitive;
}

void SpherePhysicsShape::type(PhysicsShape::Type type) {
    throw std::logic_error("Cannot set PhysicsShape::Type for SpherePhysicsShape.");
}

// [PhysicsShape Internal Member Functions]

bool SpherePhysicsShape::supportsMargin() const {
    return false;
}
