//
//  SpherePhysicsShape.cc
//  avara3d
//
//  Created by Morgan Davis on 11/19/23.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#include "a3d/physics/shape/primitive/SpherePhysicsShape.h"

#include <stdexcept>

using namespace a3d;

/// Public Lifecycle Functions ///

SpherePhysicsShape::SpherePhysicsShape(float radius):
		_radius{radius} {}

SpherePhysicsShape::~SpherePhysicsShape() {}

/// Public Member Functions ///

float SpherePhysicsShape::radius() const {
	return _radius;
}

void SpherePhysicsShape::radius(float radius) {
	_radius = radius;
}

/// PhysicsShape Public Member Functions ///

PhysicsShape::Type SpherePhysicsShape::type() const {
	return PhysicsShape::Type::Primitive;
}

void SpherePhysicsShape::type(PhysicsShape::Type type) {
	throw std::logic_error("Cannot set PhysicsShape::Type for SpherePhysicsShape.");
}
