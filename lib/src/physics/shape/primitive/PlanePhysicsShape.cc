//
//  PlanePhysicsShape.cc
//  avara3d
//
//  Created by Morgan Davis on 11/19/23.
//  Copyright © 2023-2026 Morgan K Davis. All rights reserved.
//

#include "a3d/physics/shape/primitive/PlanePhysicsShape.h"

#include <stdexcept>

using namespace a3d;

/// Public Lifecycle Functions ///

PlanePhysicsShape::PlanePhysicsShape(float width, float height):
    _width {width},
    _height {height} {}

/// Public Member Functions ///

float PlanePhysicsShape::width() const {
    return _width;
}

void PlanePhysicsShape::width(float width) {
    _width = width;
}

float PlanePhysicsShape::height() const {
    return _height;
}

void PlanePhysicsShape::height(float height) {
    _height = height;
}

/// PhysicsShape Public Member Functions ///

PhysicsShape::Type PlanePhysicsShape::type() const {
    return PhysicsShape::Type::Primitive;
}

void PlanePhysicsShape::type(PhysicsShape::Type type) {
    throw std::logic_error("Cannot set PhysicsShape::Type for PlanePhysicsShape.");
}
