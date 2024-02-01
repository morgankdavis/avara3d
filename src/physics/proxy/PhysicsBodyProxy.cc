//
// Created by mkd on 11/13/23.
//

#include "ae/physics/proxy/PhysicsBodyProxy.h"

#include "ae/diagnostic/logging/Logger.h"


using namespace ae;


/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

PhysicsBodyProxy::PhysicsBodyProxy(PhysicsBody* body):
		_body(nullptr),
		_shapeModel(nullptr),
		_autocalculatesMomentOfInertia(true) {

	attachedToBody(body);
}

PhysicsBodyProxy::~PhysicsBodyProxy() { }

/*********************************************************************************************
	Internal
 *********************************************************************************************/

bool PhysicsBodyProxy::autocalculatesMomentOfInertia() const {
	return _autocalculatesMomentOfInertia;
}

void PhysicsBodyProxy::autocalculatesMomentOfInertia(bool autocalculate) {
	_autocalculatesMomentOfInertia = autocalculate;
}

void PhysicsBodyProxy::attachedToBody(PhysicsBody* body) {
	AE_LOG_T("body: {:p}", static_cast<void*>(body));

	_body = body;
}

void PhysicsBodyProxy::detachedFromBody(PhysicsBody* body) {
	AE_LOG_T("body: {:p}", static_cast<void*>(body));

	_body = nullptr;
}
