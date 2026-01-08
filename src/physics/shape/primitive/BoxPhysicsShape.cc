//
//  BoxPhysicsShape.cc
//  avara3d
//
//  Created by Morgan Davis on 11/19/23.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#include "a3d/physics/shape/primitive/BoxPhysicsShape.h"

#include "a3d/exception/Exception.h"

using namespace a3d;

/// Public Lifecycle Functions ///

BoxPhysicsShape::BoxPhysicsShape(float width, float height, float length):
	_width{width},
	_height{height},
	_length{length} {}

BoxPhysicsShape::~BoxPhysicsShape() {}

/// Public Member Functions ///

float BoxPhysicsShape::width() const {
	return _width;
}

void BoxPhysicsShape::width(float width) {
	_width = width;
}

float BoxPhysicsShape::height() const {
	return _height;
}

void BoxPhysicsShape::height(float height) {
	_height = height;
}

float BoxPhysicsShape::length() const {
	return _length;
}

void BoxPhysicsShape::length(float length) {
	_length = length;
}

/// PhysicsShape Public Member Functions ///

PhysicsShape::Type BoxPhysicsShape::type() const {
	return PhysicsShape::Type::Primitive;
}

void BoxPhysicsShape::type(PhysicsShape::Type type) {
	throw Exception("Cannot set PhysicsShape::Type for BoxPhysicsShape.");
}
