//
//  CylinderPhysicsShape.cc
//  avara3d
//
//  Created by Morgan Davis on 11/19/23.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#include "a3d/physics/shape_primitive/CylinderPhysicsShape.h"

#include "a3d/diagnostic/exception/Exception.h"


using namespace a3d;


/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

CylinderPhysicsShape::CylinderPhysicsShape(float radius, float height):
		_radius{radius},
		_height{height} {}

CylinderPhysicsShape::~CylinderPhysicsShape() {}

/*********************************************************************************************
	Public
 *********************************************************************************************/

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

/*********************************************************************************************
	PhysicsShape
 *********************************************************************************************/

PhysicsShapeType CylinderPhysicsShape::type() const {
	throw Exception("PHYSICS_SHAPE_TYPE does not apply to CylinderPhysicsShape.");
}

void CylinderPhysicsShape::type(PhysicsShapeType type) {
	throw Exception("PHYSICS_SHAPE_TYPE does not apply to CylinderPhysicsShape.");
}
