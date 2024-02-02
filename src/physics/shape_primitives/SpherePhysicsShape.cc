//
// Created by mkd on 11/19/23.
//

#include "ae/physics/shape_primitives/SpherePhysicsShape.h"

#include "ae/diagnostic/exceptions/Exception.h"


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

PHYSICS_SHAPE_TYPE SpherePhysicsShape::type() const {
	throw Exception("PHYSICS_SHAPE_TYPE does not apply to SpherePhysicsShape.");
}

void SpherePhysicsShape::type(PHYSICS_SHAPE_TYPE type) {
	throw Exception("PHYSICS_SHAPE_TYPE does not apply to SpherePhysicsShape.");
}
