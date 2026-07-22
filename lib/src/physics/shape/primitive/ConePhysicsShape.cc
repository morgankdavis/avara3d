//
//  ConePhysicsShape.cc
//  avara3d
//
//  Created by Morgan Davis on 11/19/23.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#include "a3d/physics/shape/primitive/ConePhysicsShape.h"

#include <stdexcept>

using namespace a3d;

/// Public Lifecycle Functions ///

ConePhysicsShape::ConePhysicsShape(float radius, float height):
		_radius{radius},
		_height{height} {}

ConePhysicsShape::~ConePhysicsShape() {}

/// Public Member Functions ///

float ConePhysicsShape::radius() const {
	return _radius;
}

void ConePhysicsShape::radius(float radius) {
	_radius = radius;
}

float ConePhysicsShape::height() const {
	return _height;
}

void ConePhysicsShape::height(float height) {
	_height = height;
}

/// PhysicsShape Public Member Functions ///

PhysicsShape::Type ConePhysicsShape::type() const {
	return PhysicsShape::Type::Primitive;
}

void ConePhysicsShape::type(PhysicsShape::Type type) {
	throw std::logic_error("Cannot set PhysicsShape::Type for ConePhysicsShape.");
}
