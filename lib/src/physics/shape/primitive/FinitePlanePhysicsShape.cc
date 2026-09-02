//
//  FinitePlanePhysicsShape.cc
//  avara3d
//
//  Created by Morgan Davis on 11/19/23.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#include "a3d/physics/shape/primitive/FinitePlanePhysicsShape.h"

#include <stdexcept>

namespace a3d {
// [Public Lifecycle Functions]

FinitePlanePhysicsShape::FinitePlanePhysicsShape(float width, float height):
    _width {width},
    _height {height} {}

// [Public Member Functions]

float FinitePlanePhysicsShape::width() const {
    return _width;
}

float FinitePlanePhysicsShape::height() const {
    return _height;
}

// [PhysicsShape Public Member Functions]

PhysicsShape::Type FinitePlanePhysicsShape::type() const {
    return PhysicsShape::Type::Primitive;
}

void FinitePlanePhysicsShape::type(PhysicsShape::Type type) {
    throw std::logic_error("Cannot set PhysicsShape::Type for FinitePlanePhysicsShape.");
}
} // namespace a3d
