//
//  SpherePhysicsShape.cc
//  avara3d
//
//  Created by Morgan Davis on 11/19/23.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#include "a3d/physics/shape_primitive/SpherePhysicsShape.h"

#include "a3d/diagnostic/exception/Exception.h"


using namespace a3d;


/*********************************************************************************************
	Public Lifecycle
 *********************************************************************************************/

SpherePhysicsShape::SpherePhysicsShape(float radius):
		_radius{radius} {}

SpherePhysicsShape::~SpherePhysicsShape() {}

/*********************************************************************************************
	Public Members
 *********************************************************************************************/

float SpherePhysicsShape::radius() const {
	return _radius;
}

void SpherePhysicsShape::radius(float radius) {
	_radius = radius;
}

/*********************************************************************************************
	PhysicsShape Public Members
 *********************************************************************************************/

PhysicsShapeType SpherePhysicsShape::type() const {
	throw Exception("PHYSICS_SHAPE_TYPE does not apply to SpherePhysicsShape.");
}

void SpherePhysicsShape::type(PhysicsShapeType type) {
	throw Exception("PHYSICS_SHAPE_TYPE does not apply to SpherePhysicsShape.");
}
