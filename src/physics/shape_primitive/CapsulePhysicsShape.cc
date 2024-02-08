//
// Created by mkd on 11/19/23.
//

#include "ae/physics/shape_primitive/CapsulePhysicsShape.h"

#include "ae/diagnostic/exception/Exception.h"
#include "ae/physics/proxy/PhysicsShapeProxy.h"


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

PhysicsShapeType CapsulePhysicsShape::type() const {
	throw Exception("PHYSICS_SHAPE_TYPE does not apply to CapsulePhysicsShape.");
}

void CapsulePhysicsShape::type(PhysicsShapeType type) {
	throw Exception("PHYSICS_SHAPE_TYPE does not apply to CapsulePhysicsShape.");
}
