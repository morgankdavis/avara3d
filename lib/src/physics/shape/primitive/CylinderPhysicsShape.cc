//
//  CylinderPhysicsShape.cc
//  avara3d
//
//  Created by Morgan Davis on 11/19/23.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#include "a3d/physics/shape/primitive/CylinderPhysicsShape.h"

#include <stdexcept>

using namespace a3d;

/// Public Lifecycle Functions ///

CylinderPhysicsShape::CylinderPhysicsShape(float radius, float height):
    _radius {radius},
    _height {height} {}

/// Public Member Functions ///

float CylinderPhysicsShape::radius() const {
    return _radius;
}

void CylinderPhysicsShape::radius(float radius) {
    _radius = radius;
}

float CylinderPhysicsShape::height() const {
    return _height;
}

void CylinderPhysicsShape::height(float height) {
    _height = height;
}

/// PhysicsShape Public Member Functions ///

PhysicsShape::Type CylinderPhysicsShape::type() const {
    return PhysicsShape::Type::Primitive;
}

void CylinderPhysicsShape::type(PhysicsShape::Type type) {
    throw std::logic_error("Cannot set PhysicsShape::Type for CylinderPhysicsShape.");
}
