//
//  FlyCameraController.cc
//  avara3d
//
//  Created by Morgan Davis on 8/9/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#include "a3d/extension/camera/FlyCameraController.h"

#include "a3d/Math.h"
#include "a3d/scene/Node.h"

using namespace a3d;
using namespace a3d::ext;
using namespace a3d::math;
using namespace std;

// [Public Lifecycle Functions]

FlyCameraController::FlyCameraController():
    FlyCameraController(Config {}) {}

FlyCameraController::FlyCameraController(const Config& config):
    _config {config} {}

// [Public Member Functions]

const FlyCameraController::Config& FlyCameraController::config() const {
    return _config;
}

void FlyCameraController::config(const Config& config) {
    _config = config;
}

bool FlyCameraController::update(Node& pov, DesktopInputContext& input, double deltaTime) {

    bool changed = false;

    // look

    const vec2 mouseDelta = input.mousePositionDelta();

    if (mouseDelta.x != 0.0f || mouseDelta.y != 0.0f) {

        const float deltaYaw = math::atan(_config.lookSensitivity * mouseDelta.x);
        const float deltaPitch = math::atan(_config.lookSensitivity * mouseDelta.y);
        const vec3  angles = pov.eulerAngles();

        pov.eulerAngles({
            angles.x + deltaPitch,
            angles.y - deltaYaw,
            0.0f,
        });

        changed = true;
    }

    // movement

    float moveSpeed = _config.moveSpeed;

    if (input.keyDown(_config.controls.fastModifier)) {
        moveSpeed *= _config.fastMoveMultiplier;
    }

    const float distance = static_cast<float>(deltaTime) * moveSpeed;

    vec3 movement {0.0f};

    if (input.keyDown(_config.controls.forward)) {
        movement += pov.forward();
    }
    else if (input.keyDown(_config.controls.back)) {
        movement -= pov.forward();
    }

    if (input.keyDown(_config.controls.left)) {
        movement -= pov.right();
    }
    else if (input.keyDown(_config.controls.right)) {
        movement += pov.right();
    }

    if (input.keyDown(_config.controls.up)) {
        const float direction = input.keyDown(_config.controls.descendModifier) ? -1.0f : 1.0f;
        movement += pov.up() * direction;
    }

    if (math::length(movement) > 0.0f) {
        pov.position(pov.position() + movement * distance);
        changed = true;
    }

    return changed;
}
