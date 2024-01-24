//
// Created by mkd on 11/13/23.
//

#include "physics/model_proxy/PhysicsBodyModelProxy.h"
#include "diagnostic/logging/Logger.h"


using namespace ae;


/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

PhysicsBodyModelProxy::PhysicsBodyModelProxy(PhysicsBody* body):
		_body(nullptr),
		_shapeModel(nullptr),
		_autocalculatesMomentOfInertia(true) {

	attachedToBody(body);
}

PhysicsBodyModelProxy::~PhysicsBodyModelProxy() { }

/*********************************************************************************************
	Internal
 *********************************************************************************************/

bool PhysicsBodyModelProxy::autocalculatesMomentOfInertia() const {
	return _autocalculatesMomentOfInertia;
}

void PhysicsBodyModelProxy::autocalculatesMomentOfInertia(bool autocalculate) {
	_autocalculatesMomentOfInertia = autocalculate;
}

void PhysicsBodyModelProxy::attachedToBody(PhysicsBody* body) {
	AE_LOG_T("body: {:p}", static_cast<void*>(body));

	_body = body;
}

void PhysicsBodyModelProxy::detachedFromBody(PhysicsBody* body) {
	AE_LOG_T("body: {:p}", static_cast<void*>(body));

	_body = nullptr;
}
