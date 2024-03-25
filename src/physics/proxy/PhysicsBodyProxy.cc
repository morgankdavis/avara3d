//
// Created by mkd on 11/13/23.
//

#include "a3d/physics/proxy/PhysicsBodyProxy.h"

#include "a3d/diagnostic/logging/Logger.h"


using namespace a3d;


/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

PhysicsBodyProxy::PhysicsBodyProxy(PhysicsBody& body):
		_body(nullptr),
		_shapeProxy(nullptr),
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

void PhysicsBodyProxy::attachedToBody(PhysicsBody& body) {
	A3D_LOG_T("body: {:p}", static_cast<void*>(&body));

	_body = &body;
}

void PhysicsBodyProxy::detachedFromBody(PhysicsBody& body) {
	A3D_LOG_T("body: {:p}", static_cast<void*>(&body));

	_body = nullptr;
}
