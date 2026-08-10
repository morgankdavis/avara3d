//
//  TurntableCameraController.cc
//  avara3d
//
//  Created by Morgan Davis on 8/9/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#include "a3d/extension/camera/TurntableCameraController.h"

#include <algorithm>
#include <cmath>
#include <stdexcept>

#include "a3d/scene/Node.h"

using namespace a3d;
using namespace a3d::ext;
using namespace a3d::math;
using namespace std;

/// Private Static Non-Member Prototypes ///

static bool IsFinite(const vec3& value);

static void ValidateConfig(const TurntableCameraController::Config& config);

static bool TryResolveNodeTarget(const TurntableCameraController::NodeTarget& target, vec3& worldPosition);

/// Public Lifecycle Functions ///

TurntableCameraController::TurntableCameraController():
    TurntableCameraController(Config {}) {}

TurntableCameraController::TurntableCameraController(const Config& config):
    _config {},
    _view {},
    _resolvedTarget {0.0f} {

    this->config(config);
    resolveTarget();
}

/// Public Member Functions ///

const TurntableCameraController::Config& TurntableCameraController::config() const {

    return _config;
}

void TurntableCameraController::config(const Config& config) {

    ValidateConfig(config);

    _config = config;

    _view.pitch = std::clamp(_view.pitch, _config.minPitch, _config.maxPitch);
    _view.distance = std::clamp(_view.distance, _config.minDistance, _config.maxDistance);
}

const TurntableCameraController::View& TurntableCameraController::view() const {

    return _view;
}

void TurntableCameraController::view(const View& view) {

    if (!std::isfinite(view.yaw)) {
        throw invalid_argument("TurntableCameraController view yaw must be finite.");
    }

    if (!std::isfinite(view.pitch)) {
        throw invalid_argument("TurntableCameraController view pitch must be finite.");
    }

    if (!std::isfinite(view.distance)) {
        throw invalid_argument("TurntableCameraController view distance must be finite.");
    }

    View nextView = view;

    nextView.pitch = math::clamp(nextView.pitch, _config.minPitch, _config.maxPitch);
    nextView.distance = math::clamp(nextView.distance, _config.minDistance, _config.maxDistance);

    vec3 nextResolvedTarget {0.0f};

    if (const auto* worldPosition = std::get_if<vec3>(&nextView.target)) {

        if (!IsFinite(*worldPosition)) {
            throw invalid_argument("TurntableCameraController world target must be finite.");
        }

        nextResolvedTarget = *worldPosition;
    }
    else {

        const auto& nodeTarget = std::get<NodeTarget>(nextView.target);

        if (!IsFinite(nodeTarget.localPosition)) {
            throw invalid_argument("TurntableCameraController local target position must be finite.");
        }

        if (!TryResolveNodeTarget(nodeTarget, nextResolvedTarget)) {
            throw invalid_argument("TurntableCameraController view has an expired target Node.");
        }
    }

    _view = nextView;
    _resolvedTarget = nextResolvedTarget;
}

void TurntableCameraController::target(const vec3& worldPosition) {

    if (!IsFinite(worldPosition)) {
        throw invalid_argument("TurntableCameraController world target must be finite.");
    }

    _view.target = worldPosition;
    _resolvedTarget = worldPosition;
}

void TurntableCameraController::target(const shared_ptr<Node>& node) {

    target(node, vec3 {0.0f});
}

void TurntableCameraController::target(const shared_ptr<Node>& node, const vec3& localPosition) {

    if (!node) {
        throw invalid_argument("TurntableCameraController target Node must not be null.");
    }

    if (!IsFinite(localPosition)) {
        throw invalid_argument("TurntableCameraController local target position must be finite.");
    }

    NodeTarget nodeTarget {
        .node = node,
        .localPosition = localPosition,
    };

    vec3 resolvedTarget;

    if (!TryResolveNodeTarget(nodeTarget, resolvedTarget)) {
        throw invalid_argument("TurntableCameraController target Node must not be expired.");
    }

    _view.target = nodeTarget;
    _resolvedTarget = resolvedTarget;
}

void TurntableCameraController::apply(Node& pov) {

    const vec3 targetPosition = resolveTarget();

    const float cosPitch = math::cos(_view.pitch);

    const vec3 offset {
        math::sin(_view.yaw) * cosPitch,
        math::sin(_view.pitch),
        math::cos(_view.yaw) * cosPitch,
    };

    const vec3 eye = targetPosition + offset * _view.distance;

    const mat4 viewTransform = math::look_at(eye, targetPosition, vec3 {0.0f, 1.0f, 0.0f});

    const mat4 worldTransform = math::inverse(viewTransform);

    if (auto parent = pov.parent().lock()) {

        const mat4 localTransform = math::inverse(parent->worldTransform()) * worldTransform;

        pov.transform(localTransform);
    }
    else {
        pov.transform(worldTransform);
    }
}

/// Private Member Functions ///

vec3 TurntableCameraController::resolveTarget() {

    if (const auto* worldPosition = std::get_if<vec3>(&_view.target)) {

        _resolvedTarget = *worldPosition;
    }
    else {

        const auto& nodeTarget = std::get<NodeTarget>(_view.target);

        vec3 resolvedTarget;

        if (TryResolveNodeTarget(nodeTarget, resolvedTarget)) {
            _resolvedTarget = resolvedTarget;
        }
    }

    return _resolvedTarget;
}

/// Private Static Non-Member Functions ///

bool IsFinite(const vec3& value) {

    return std::isfinite(value.x) && std::isfinite(value.y) && std::isfinite(value.z);
}

void ValidateConfig(const TurntableCameraController::Config& config) {

    if (!std::isfinite(config.minPitch) || !std::isfinite(config.maxPitch)) {
        throw invalid_argument("TurntableCameraController pitch limits must be finite.");
    }

    const float pole = math::radians(90.0f);

    if (config.minPitch <= -pole || config.maxPitch >= pole) {
        throw invalid_argument(
            "TurntableCameraController pitch limits must remain between -90 and 90 degrees.");
    }

    if (config.minPitch >= config.maxPitch) {
        throw invalid_argument("TurntableCameraController minimum pitch must be less than maximum pitch.");
    }

    if (!std::isfinite(config.minDistance) || config.minDistance <= 0.0f) {
        throw invalid_argument("TurntableCameraController minimum distance must be positive and finite.");
    }

    if (!std::isfinite(config.maxDistance)) {
        throw invalid_argument("TurntableCameraController maximum distance must be finite.");
    }

    if (config.maxDistance < config.minDistance) {
        throw invalid_argument(
            "TurntableCameraController maximum distance must not be less than minimum distance.");
    }
}

bool TryResolveNodeTarget(const TurntableCameraController::NodeTarget& target, vec3& worldPosition) {

    auto node = target.node.lock();

    if (!node) {
        return false;
    }

    const vec4 world = node->worldTransform() * vec4 {target.localPosition, 1.0f};

    const vec3 resolved {world};

    if (!IsFinite(resolved)) {
        throw runtime_error("TurntableCameraController resolved target position is not finite.");
    }

    worldPosition = resolved;

    return true;
}
