//
//  BulletMotionState.cc
//  avara3d
//
//  Created by Morgan Davis on 12/15/23.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#include "a3d/physics/backend/bullet/BulletMotionState.h"

#include "a3d/physics/PhysicsBody.h"
#include "a3d/physics/backend/bullet/BulletUtilities.h"
#include "a3d/scene/Node.h"

#include "a3d/log/Log.h"

using namespace a3d::math;

namespace a3d {
// [Internal Lifecycle Functions]

BulletMotionState::BulletMotionState(PhysicsBody& body):
    btMotionState {},
    _body {&body} {}

// [btMotionState Members]

void BulletMotionState::getWorldTransform(btTransform& transform) const {

    transform.setIdentity();

    if (auto node = _body->node().lock()) {

        const mat4 modelWorldTransform =
            translate(mat4(1.0f), node->worldPosition()) * mat4_cast(node->worldOrientation());

        transform = centerOfMassWorldTransform(modelWorldTransform);
    }
}

void BulletMotionState::setWorldTransform(const btTransform& transform) {

    if (auto node = _body->node().lock()) {
        node->applyPhysicsTransform(modelWorldTransform(transform));
    }
}

// [Internal Member Functions]

btTransform BulletMotionState::centerOfMassWorldTransform(const mat4& modelWorldTransform) const {

    const mat4 centerOfMassTransform = translate(mat4(1.0f), _body->centerOfMass());
    return BTTransformFromA3DMat4(modelWorldTransform * centerOfMassTransform);
}

mat4 BulletMotionState::modelWorldTransform(const btTransform& centerOfMassWorldTransform) const {

    const mat4 centerOfMassTransform = translate(mat4(1.0f), -_body->centerOfMass());
    return A3DMat4FromBTTransform(centerOfMassWorldTransform) * centerOfMassTransform;
}

PhysicsBody* BulletMotionState::body() const {
    return _body;
}
} // namespace a3d
