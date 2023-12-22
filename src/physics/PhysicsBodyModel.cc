//
// Created by mkd on 11/13/23.
//

#include "physics/PhysicsBodyModel.h"
#include "diagnostic/logging/Logger.h"


using namespace ae;


/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

PhysicsBodyModel::PhysicsBodyModel(PhysicsBody* body):
		_body(nullptr),
		_shapeModel(nullptr),
		_autocalculatesMomentOfInertia(true) {

	attachedToBody(body);
}

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

void PhysicsBodyModel::attachedToBody(PhysicsBody* body) {
	AE_LOG_T("body: {:p}", static_cast<void*>(body));

	_body = body;
}

void PhysicsBodyModel::detachedFromBody(PhysicsBody* body) {
	AE_LOG_T("body: {:p}", static_cast<void*>(body));

	_body = nullptr;
}
