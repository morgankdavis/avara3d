//
//  ConePhysicsShape.cc
//  avara3d
//
//  Created by Morgan Davis on 11/19/23.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#include "a3d/physics/shape_primitive/ConePhysicsShape.h"

#include "a3d/diagnostic/exception/Exception.h"


using namespace a3d;


/*********************************************************************************************
	Public Lifecycle
 *********************************************************************************************/

ConePhysicsShape::ConePhysicsShape(float radius, float height):
		_radius{radius},
		_height{height} {}

ConePhysicsShape::~ConePhysicsShape() {}

/*********************************************************************************************
	Public Members
 *********************************************************************************************/

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

/*********************************************************************************************
	PhysicsShape Public Members
 *********************************************************************************************/

PhysicsShapeType ConePhysicsShape::type() const {
	return PhysicsShapeType::Primitive;
}

void ConePhysicsShape::type(PhysicsShapeType type) {
	throw Exception("Cannot set PhysicsShapeType for ConePhysicsShape.");
}
