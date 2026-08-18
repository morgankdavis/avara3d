//
//  PhysicsBodyProxy.cc
//  avara3d
//
//  Created by Morgan Davis on 11/13/23.
//  Copyright © 2023-2026 Morgan K Davis. All rights reserved.
//

#include "a3d/physics/proxy/PhysicsBodyProxy.h"

#include "a3d/log/Log.h"

using namespace a3d;

/// Internal Lifecycle Functions ///

PhysicsBodyProxy::PhysicsBodyProxy(PhysicsBody& body, PhysicsBody::Type type):
    _body {},
    _shapeProxy {},
    _autocalculatesMomentOfInertia {true} {

    attachedToBody(body);
}

PhysicsBodyProxy::~PhysicsBodyProxy() {}

/// Internal Member Functions ///

bool PhysicsBodyProxy::autocalculatesMomentOfInertia() const {
    return _autocalculatesMomentOfInertia;
}

void PhysicsBodyProxy::autocalculatesMomentOfInertia(bool autocalculate) {
    _autocalculatesMomentOfInertia = autocalculate;
}

void PhysicsBodyProxy::attachedToBody(PhysicsBody& body) {
    log::t()("body: {:p}", static_cast<void*>(&body));

    _body = &body;
}

void PhysicsBodyProxy::detachedFromBody(PhysicsBody& body) {
    log::t()("body: {:p}", static_cast<void*>(&body));

    _body = nullptr;
}
