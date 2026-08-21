//
//  WanderRotator.cc
//  avara3d
//
//  Created by Morgan Davis on 8/21/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#include "a3d/extension/WanderRotator.h"

#include "a3d/scene/Node.h"

using namespace a3d;
using namespace a3d::ext;
using namespace a3d::math;

// [Public Member Functions]

void WanderRotator::update(Node& node, float deltaTime) {

    _timer += deltaTime;
    if (_timer >= _nextChange) {
        chooseNewTarget();
    }

    // exponential smoothing toward the target angular velocity. this form is
    // independent of update frequency for a constant target.

    const float alpha = 1.0f - math::exp(-smoothing * deltaTime);
    _angularVelocity = _angularVelocity + (_targetAngularVelocity - _angularVelocity) * alpha;

    // integrate angular velocity into the node orientation.

    const float angle = length(_angularVelocity) * deltaTime;
    if (angle > 1e-6f) {
        const vec3 axis = normalize(_angularVelocity);
        const quat deltaOrientation = quaternion(axis, angle);

        node.orientation(normalize(deltaOrientation * node.orientation()));
    }
}

// [Private Member Functions]

void WanderRotator::chooseNewTarget() {

    const vec3  axis = uniform_spherical(1.0f);
    const float speed = uniform_linear(minSpeed, maxSpeed);

    _targetAngularVelocity = axis * speed;
    _nextChange = uniform_linear(minInterval, maxInterval);
    _timer = 0.0f;
}
