//
// Created by mkd on 11/19/23.
//

#include "ae/physics/shape_primitive/SpherePhysicsShape.h"

#include "ae/diagnostic/exception/Exception.h"


using namespace ae;


/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

SpherePhysicsShape::SpherePhysicsShape(float radius):
		_radius(radius) {}

SpherePhysicsShape::~SpherePhysicsShape() {}

/*********************************************************************************************
	Public
 *********************************************************************************************/

float SpherePhysicsShape::radius() const {
	return _radius;
}

void SpherePhysicsShape::radius(float radius) {
	_radius = radius;
}

/*********************************************************************************************
	PhysicsShape
 *********************************************************************************************/

PhysicsShapeType SpherePhysicsShape::type() const {
	throw Exception("PHYSICS_SHAPE_TYPE does not apply to SpherePhysicsShape.");
}

void SpherePhysicsShape::type(PhysicsShapeType type) {
	throw Exception("PHYSICS_SHAPE_TYPE does not apply to SpherePhysicsShape.");
}
