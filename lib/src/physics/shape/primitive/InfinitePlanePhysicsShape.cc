//
//  InfinitePlanePhysicsShape.cc
//  avara3d
//
//  Created by Morgan Davis on 8/8/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#include "a3d/physics/shape/primitive/InfinitePlanePhysicsShape.h"

#include <stdexcept>

#include "a3d/physics/PhysicsBody.h"

using namespace a3d;

// [Public Lifecycle Functions]

InfinitePlanePhysicsShape::InfinitePlanePhysicsShape() = default;

// [PhysicsShape Public Member Functions]

PhysicsShape::Type InfinitePlanePhysicsShape::type() const {
    return PhysicsShape::Type::Primitive;
}

void InfinitePlanePhysicsShape::type(PhysicsShape::Type type) {
    throw std::logic_error("Cannot set PhysicsShape::Type for InfinitePlanePhysicsShape.");
}

// [PhysicsShape Internal Member Functions]

bool InfinitePlanePhysicsShape::supportsBodyType(PhysicsBody::Type type) const {
    return type == PhysicsBody::Type::Static;
}

bool InfinitePlanePhysicsShape::supportsMargin() const {
    return false;
}
