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
	Lifecycle
 *********************************************************************************************/

ConePhysicsShape::ConePhysicsShape(float radius, float height):
		_radius{radius},
		_height{height} {}

ConePhysicsShape::~ConePhysicsShape() {}

/*********************************************************************************************
	Public
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
	PhysicsShape
 *********************************************************************************************/

PhysicsShapeType ConePhysicsShape::type() const {
	throw Exception("PHYSICS_SHAPE_TYPE does not apply to ConePhysicsShape.");
}

void ConePhysicsShape::type(PhysicsShapeType type) {
	throw Exception("PHYSICS_SHAPE_TYPE does not apply to ConePhysicsShape.");
}
