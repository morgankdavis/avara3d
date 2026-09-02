//
//  Wanderer.cc
//  avara3d
//
//  Created by Morgan Davis on 8/12/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#include "a3d/extension/Wanderer.h"

#include <stdexcept>

#include "a3d/scene/Node.h"

using namespace a3d::math;
using namespace a3d::ext;
using namespace std;

// [Private Non-Member Prototypes]

static void ValidateConfig(const Wanderer::Config& config);
static vec3 EvaluateSpline(const vec3& p0, const vec3& p1, const vec3& p2, const vec3& p3, float t);

namespace a3d::ext {

// [Public Lifecycle Functions]

Wanderer::Wanderer(const shared_ptr<Node>& node):
    Wanderer(node, Config {}) {}

Wanderer::Wanderer(const shared_ptr<Node>& node, const Config& config):
    _node {node},
    _config {config},
    _random {config.seed} {

    if (!node) {
        throw invalid_argument("Wanderer requires a node");
    }

    ValidateConfig(config);

    _center = node->position();

    initializePath();
}

// [Public Member Functions]

const Wanderer::Config& Wanderer::config() const {

    return _config;
}

void Wanderer::config(const Config& config) {

    ValidateConfig(config);

    _config = config;

    reset();
}

const vec3& Wanderer::center() const {

    return _center;
}

void Wanderer::update(double deltaTime) {

    if (deltaTime <= 0.0) {
        return;
    }

    auto node = _node.lock();
    if (!node) {
        return;
    }

    _phase += deltaTime / _config.segmentDuration;

    while (_phase >= 1.0) {
        _phase -= 1.0;
        advancePath();
    }

    node->position(EvaluateSpline(_p0, _p1, _p2, _p3, static_cast<float>(_phase)));
}

void Wanderer::reset() {

    _random.seed(_config.seed);
    _phase = 0.0;

    if (auto node = _node.lock()) {
        node->position(_center);
    }

    initializePath();
}

void Wanderer::recenter() {

    auto node = _node.lock();
    if (!node) {
        return;
    }

    _center = node->position();

    reset();
}

// [Private Member Functions]

void Wanderer::initializePath() {

    //
    // We want the spline to begin exactly at _center while already having
    // a direction of travel.
    //
    // For a uniform cubic B-spline:
    //
    //     P(0) = (p0 + 4*p1 + p2) / 6
    //
    // Making p0 and p2 symmetrical around p1 = center therefore gives us
    // P(0) == center.
    //

    const vec3 direction = randomOffset(0.25f);

    _p0 = _center - direction;
    _p1 = _center;
    _p2 = _center + direction;
    _p3 = randomPoint();
}

void Wanderer::advancePath() {

    _p0 = _p1;
    _p1 = _p2;
    _p2 = _p3;
    _p3 = randomPoint();
}

vec3 Wanderer::randomPoint() {

    return _center + randomOffset(1.0f);
}

vec3 Wanderer::randomOffset(float scale) {

    const auto randomComponent = [this](float extent) {
        uniform_real_distribution<float> distribution {-extent, extent};

        return distribution(_random);
    };

    const vec3 extent = _config.halfExtents * scale;

    return {randomComponent(extent.x), randomComponent(extent.y), randomComponent(extent.z)};
}

} // namespace a3d::ext

// [Private Non-Member Functions]

static void ValidateConfig(const Wanderer::Config& config) {

    if (config.halfExtents.x < 0.0f || config.halfExtents.y < 0.0f || config.halfExtents.z < 0.0f) {
        throw invalid_argument("Wanderer half extents must be non-negative");
    }

    if (config.segmentDuration <= 0.0) {
        throw invalid_argument("Wanderer segment duration must be greater than zero");
    }
}

static vec3 EvaluateSpline(const vec3& p0, const vec3& p1, const vec3& p2, const vec3& p3, float t) {

    //
    // Uniform cubic B-spline.
    //
    // All basis weights are non-negative and sum to one, so the resulting
    // position remains inside the convex hull of the control points. Since
    // every control point is inside the configured box, the node can never
    // wander outside it.
    //

    const float t2 = t * t;
    const float t3 = t2 * t;

    const float b0 = (1.0f - 3.0f * t + 3.0f * t2 - t3) / 6.0f;
    const float b1 = (4.0f - 6.0f * t2 + 3.0f * t3) / 6.0f;
    const float b2 = (1.0f + 3.0f * t + 3.0f * t2 - 3.0f * t3) / 6.0f;
    const float b3 = t3 / 6.0f;

    return p0 * b0 + p1 * b1 + p2 * b2 + p3 * b3;
}
