//
// Created by mkd on 11/19/23.
//

#include "a3d/physics/shape_primitive/SpherePhysicsShape.h"

#include "a3d/diagnostic/exception/Exception.h"


using namespace a3d;


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
