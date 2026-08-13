//
//  Wander.cc
//  avara3d
//
//  Created by Morgan Davis on 8/12/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#include "a3d/extension/Wander.h"

#include <stdexcept>

#include "a3d/scene/Node.h"

using namespace a3d;
using namespace a3d::math;
using namespace a3d::ext;
using namespace std;

Wander::Wander(const shared_ptr<Node>& node):
    Wander(node, Config {}) {}

Wander::Wander(const shared_ptr<Node>& node, const Config& config):
    _node {node},
    _config {config},
    _random {config.seed} {

    if (!node) {
        throw invalid_argument("Wander requires a node");
    }

    validateConfig(config);

    _center = node->position();

    initializePath();
}

const Wander::Config& Wander::config() const {

    return _config;
}

void Wander::config(const Config& config) {

    validateConfig(config);

    _config = config;

    reset();
}

vec3 Wander::center() const {

    return _center;
}

void Wander::update(double deltaTime) {

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

    node->position(evaluate(static_cast<float>(_phase)));
}

void Wander::reset() {

    _random.seed(_config.seed);
    _phase = 0.0;

    if (auto node = _node.lock()) {
        node->position(_center);
    }

    initializePath();
}

void Wander::recenter() {

    auto node = _node.lock();
    if (!node) {
        return;
    }

    _center = node->position();

    reset();
}

void Wander::validateConfig(const Config& config) const {

    if (config.halfExtents.x < 0.0f || config.halfExtents.y < 0.0f || config.halfExtents.z < 0.0f) {
        throw invalid_argument("Wander half extents must be non-negative");
    }

    if (config.segmentDuration <= 0.0) {
        throw invalid_argument("Wander segment duration must be greater than zero");
    }
}

void Wander::initializePath() {

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

void Wander::advancePath() {

    _p0 = _p1;
    _p1 = _p2;
    _p2 = _p3;
    _p3 = randomPoint();
}

vec3 Wander::randomPoint() {

    return _center + randomOffset(1.0f);
}

vec3 Wander::randomOffset(float scale) {

    const auto randomComponent = [this](float extent) {
        uniform_real_distribution<float> distribution {-extent, extent};

        return distribution(_random);
    };

    const vec3 extent = _config.halfExtents * scale;

    return {randomComponent(extent.x), randomComponent(extent.y), randomComponent(extent.z)};
}

vec3 Wander::evaluate(float t) const {

    //
    // Uniform cubic B-spline.
    //
    // All basis weights are non-negative and sum to one, so the resulting
    // position remains inside the convex hull of the control points. Since
    // every control point is inside our configured box, the node can never
    // wander outside it.
    //

    const float t2 = t * t;
    const float t3 = t2 * t;

    const float b0 = (1.0f - 3.0f * t + 3.0f * t2 - t3) / 6.0f;
    const float b1 = (4.0f - 6.0f * t2 + 3.0f * t3) / 6.0f;
    const float b2 = (1.0f + 3.0f * t + 3.0f * t2 - 3.0f * t3) / 6.0f;
    const float b3 = t3 / 6.0f;

    return _p0 * b0 + _p1 * b1 + _p2 * b2 + _p3 * b3;
}
