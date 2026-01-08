//
//  CapsulePhysicsShape.cc
//  avara3d
//
//  Created by Morgan Davis on 11/19/23.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#include "a3d/physics/shape/primitive/CapsulePhysicsShape.h"

#include "a3d/physics/proxy/PhysicsShapeProxy.h"

using namespace a3d;

/// Public Lifecycle Functions ///

CapsulePhysicsShape::CapsulePhysicsShape(float radius, float height):
		PhysicsShape{},
		_radius{radius},
		_height{height} {

	//checkCreateProxy();
}

CapsulePhysicsShape::~CapsulePhysicsShape() {}

/// Public Member Functions ///

float CapsulePhysicsShape::radius() const {
	return _radius;
}

void CapsulePhysicsShape::radius(float radius) {
	_radius = radius;

//	_proxy = nullptr;
//	checkCreateProxy();
}

float CapsulePhysicsShape::height() const {
	return _height;
}

void CapsulePhysicsShape::height(float height) {
	_height = height;

//	_proxy = nullptr;
//	checkCreateProxy();
}

/// PhysicsShape Public Member Functions ///

PhysicsShape::Type CapsulePhysicsShape::type() const {
	return PhysicsShape::Type::Primitive;
}

void CapsulePhysicsShape::type(PhysicsShape::Type type) {
	throw std::logic_error("Cannot set PhysicsShape::Type for CapsulePhysicsShape.");
}
