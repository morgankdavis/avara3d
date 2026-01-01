//
//  PlanePhysicsShape.cc
//  avara3d
//
//  Created by Morgan Davis on 11/19/23.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#include "a3d/physics/shape_primitive/PlanePhysicsShape.h"

#include "a3d/exception/Exception.h"

using namespace a3d;

/// Public Lifecycle Functions ///

PlanePhysicsShape::PlanePhysicsShape(float width, float height):
		_width{width},
		_height{height} {}

PlanePhysicsShape::~PlanePhysicsShape() {}

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

PhysicsShapeType PlanePhysicsShape::type() const {
	return PhysicsShapeType::Primitive;
}

void PlanePhysicsShape::type(PhysicsShapeType type) {
	throw Exception("Cannot set PhysicsShapeType for PlanePhysicsShape.");
}
