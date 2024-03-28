//
// Created by mkd on 11/19/23.
//

#include "a3d/physics/shape_primitive/CapsulePhysicsShape.h"

#include "a3d/diagnostic/exception/Exception.h"
#include "a3d/physics/proxy/PhysicsShapeProxy.h"


using namespace a3d;


/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

CapsulePhysicsShape::CapsulePhysicsShape(float radius, float height):
		PhysicsShape{},
		_radius{radius},
		_height{height} {

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
