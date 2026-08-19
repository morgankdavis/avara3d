//
//  PhysicsBodyProxy.cc
//  avara3d
//
//  Created by Morgan Davis on 11/13/23.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#include "a3d/physics/proxy/PhysicsBodyProxy.h"

#include "a3d/log/Log.h"

using namespace a3d;

/// Internal Lifecycle Functions ///

PhysicsBodyProxy::PhysicsBodyProxy(PhysicsBody& body, PhysicsBody::Type type):
    _body {},
    _shapeProxy {},
    _centerOfMass {0.0f},
    _autocalculatesCenterOfMass {type == PhysicsBody::Type::Dynamic},
    _centerOfMassCalculation {PhysicsBody::CenterOfMassCalculation::BoundsCenter},
    _autocalculatesMomentOfInertia {true} {
    attachedToBody(body);
}

PhysicsBodyProxy::~PhysicsBodyProxy() {}

/// Internal Member Functions ///

bool PhysicsBodyProxy::autocalculatesCenterOfMass() const {
    return _autocalculatesCenterOfMass;
}

void PhysicsBodyProxy::autocalculatesCenterOfMass(bool autocalculate) {
    _autocalculatesCenterOfMass = autocalculate;
}

bool PhysicsBodyProxy::autocalculatesMomentOfInertia() const {
    return _autocalculatesMomentOfInertia;
}

void PhysicsBodyProxy::autocalculatesMomentOfInertia(bool autocalculate) {
    _autocalculatesMomentOfInertia = autocalculate;
}

PhysicsBody::CenterOfMassCalculation PhysicsBodyProxy::centerOfMassCalculation() const {
    return _centerOfMassCalculation;
}

void PhysicsBodyProxy::centerOfMassCalculation(PhysicsBody::CenterOfMassCalculation calculation) {
    _centerOfMassCalculation = calculation;
}

void PhysicsBodyProxy::attachedToBody(PhysicsBody& body) {
    log::t()("body: {:p}", static_cast<void*>(&body));

    _body = &body;
}

void PhysicsBodyProxy::detachedFromBody(PhysicsBody& body) {
    log::t()("body: {:p}", static_cast<void*>(&body));

    _body = nullptr;
}
