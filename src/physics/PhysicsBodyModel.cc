//
// Created by mkd on 11/13/23.
//

#include "physics/PhysicsBodyModel.h"


using namespace ae;


/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

PhysicsBodyModel::PhysicsBodyModel(PhysicsBody* body):
		_body(body),
		_shapeModel(nullptr),
		_autocalculatesMomentOfInertia(true) { }

PhysicsBodyModel::~PhysicsBodyModel() { }

/*********************************************************************************************
	Internal
 *********************************************************************************************/

bool PhysicsBodyModel::autocalculatesMomentOfInertia() const {
	return _autocalculatesMomentOfInertia;
}

void PhysicsBodyModel::autocalculatesMomentOfInertia(bool autocalculate) {
	_autocalculatesMomentOfInertia = autocalculate;
}
