//
//  BoxPhysicsShape.cc
//  avara3d
//
//  Created by Morgan Davis on 11/19/23.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#include "a3d/physics/shape/primitive/BoxPhysicsShape.h"

#include <stdexcept>

namespace a3d {

// [Public Lifecycle Functions]

BoxPhysicsShape::BoxPhysicsShape(float width, float height, float length):
    _width {width},
    _height {height},
    _length {length} {}

// [Public Member Functions]

float BoxPhysicsShape::width() const {
    return _width;
}

float BoxPhysicsShape::height() const {
    return _height;
}

float BoxPhysicsShape::length() const {
    return _length;
}

// [PhysicsShape Public Member Functions]

PhysicsShape::Type BoxPhysicsShape::type() const {
    return PhysicsShape::Type::Primitive;
}

void BoxPhysicsShape::type(PhysicsShape::Type type) {
    throw std::logic_error("Cannot set PhysicsShape::Type for BoxPhysicsShape.");
}

} // namespace a3d
