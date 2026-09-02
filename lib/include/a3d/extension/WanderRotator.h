//
//  WanderRotator.h
//  avara3d
//
//  Created by Morgan Davis on 7/21/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_EXTENSION_WANDERROTATOR_H
#define AVARA3D_EXTENSION_WANDERROTATOR_H

#include "a3d/Math.h"

namespace a3d {

class Node;

}

namespace a3d::ext {

/**
 * @brief Applies smoothly varying random rotation to a Node.
 *
 * WanderRotator periodically chooses a random angular velocity and smoothly
 * transitions toward it as update() advances.
 */
class WanderRotator {

public:
    // [Public Member Variables]

    float minInterval {0.8f}; ///< Minimum time between target changes, in seconds.
    float maxInterval {2.5f}; ///< Maximum time between target changes, in seconds.
    float minSpeed {0.5f};    ///< Minimum target angular speed, in radians per second.
    float maxSpeed {1.0f};    ///< Maximum target angular speed, in radians per second.
    float smoothing {1.0f};   ///< Smoothing rate toward target angular velocity; larger values respond faster.

    // [Public Member Functions]

    /** @brief Advances the rotation by @p deltaTime seconds and applies it to @p node. */
    void  update(Node& node, float deltaTime);

private:
    // [Private Member Functions]

    void       chooseNewTarget();

    // [Private Member Variables]

    float      _timer {0.0f};
    float      _nextChange {1.0f};

    math::vec3 _angularVelocity {0.0f, 0.0f, 0.0f};
    math::vec3 _targetAngularVelocity {0.0f, 0.0f, 0.0f};
};

}

#endif // AVARA3D_EXTENSION_WANDERROTATOR_H
