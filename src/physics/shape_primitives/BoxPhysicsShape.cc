//
// Created by mkd on 11/19/23.
//

#include "physics/shape_primitives/BoxPhysicsShape.h"

#include "diagnostic/Exception.h"


using namespace ae;


/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

BoxPhysicsShape::BoxPhysicsShape(float width, float height, float length):
	_width(width),
	_height(height),
	_length(length) {}

BoxPhysicsShape::~BoxPhysicsShape() {}

/*********************************************************************************************
	Public
 *********************************************************************************************/

float BoxPhysicsShape::width() const {
	return _width;
}

void BoxPhysicsShape::width(float width) {
	_width = width;
}

float BoxPhysicsShape::height() const {
	return _height;
}

void BoxPhysicsShape::height(float height) {
	_height = height;
}

float BoxPhysicsShape::length() const {
	return _length;
}

void BoxPhysicsShape::length(float length) {
	_length = length;
}

/*********************************************************************************************
	PhysicsShape
 *********************************************************************************************/

PHYSICS_SHAPE_TYPE BoxPhysicsShape::type() const {
	throw Exception("PHYSICS_SHAPE_TYPE does not apply to BoxPhysicsShape.");
}

void BoxPhysicsShape::type(PHYSICS_SHAPE_TYPE type) {
	throw Exception("PHYSICS_SHAPE_TYPE does not apply to BoxPhysicsShape.");
}
