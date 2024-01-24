//
// Created by mkd on 11/19/23.
//

#include "physics/shape_primitives/CapsulePhysicsShape.h"

#include "diagnostic/Exception.h"
#include "physics/model_proxy/PhysicsShapeModelProxy.h"


using namespace ae;


/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

CapsulePhysicsShape::CapsulePhysicsShape(float radius, float height):
		PhysicsShape(),
		_radius(radius),
		_height(height) {

	//checkCreateProxy();
}

CapsulePhysicsShape::~CapsulePhysicsShape() {}

/*********************************************************************************************
	Public
 *********************************************************************************************/

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

/*********************************************************************************************
	PhysicsShape
 *********************************************************************************************/

PHYSICS_SHAPE_TYPE CapsulePhysicsShape::type() const {
	throw Exception("PHYSICS_SHAPE_TYPE does not apply to CapsulePhysicsShape.");
}

void CapsulePhysicsShape::type(PHYSICS_SHAPE_TYPE type) {
	throw Exception("PHYSICS_SHAPE_TYPE does not apply to CapsulePhysicsShape.");
}
