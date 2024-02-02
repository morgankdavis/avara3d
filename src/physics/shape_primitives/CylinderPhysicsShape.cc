//
// Created by mkd on 11/19/23.
//

#include "ae/physics/shape_primitives/CylinderPhysicsShape.h"

#include "ae/diagnostic/exceptions/Exception.h"


using namespace ae;


/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

CylinderPhysicsShape::CylinderPhysicsShape(float radius, float height):
		_radius(radius),
		_height(height) {}

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

PHYSICS_SHAPE_TYPE CylinderPhysicsShape::type() const {
	throw Exception("PHYSICS_SHAPE_TYPE does not apply to CylinderPhysicsShape.");
}

void CylinderPhysicsShape::type(PHYSICS_SHAPE_TYPE type) {
	throw Exception("PHYSICS_SHAPE_TYPE does not apply to CylinderPhysicsShape.");
}
