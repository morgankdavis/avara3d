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
    _resolvedTarget {0.0f},
    _orbitButtonActive {false},
    _orbitButtonDragging {false},
    _orbitButtonDragMode {DragMode::Orbit},
    _orbitButtonPressPosition {0.0f},
    _panButtonActive {false},
    _panButtonDragging {false},
    _panButtonPressPosition {0.0f} {

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

    _view.pitch = math::clamp(_view.pitch, _config.minPitch, _config.maxPitch);
    _view.distance = math::clamp(_view.distance, _config.minDistance, _config.maxDistance);
}

const TurntableCameraController::View& TurntableCameraController::view() const {
    return _view;
}

void TurntableCameraController::view(const View& view) {

    if (!math::is_finite(view.yaw)) {
        throw invalid_argument("TurntableCameraController view yaw must be finite.");
    }

    if (!math::is_finite(view.pitch)) {
        throw invalid_argument("TurntableCameraController view pitch must be finite.");
    }

    if (!math::is_finite(view.distance)) {
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

TurntableCameraController::UpdateResult TurntableCameraController::update(DesktopInputContext& input,
                                                                          float                vFov,
                                                                          float                viewportHeight) {

    if (!math::is_finite(vFov) || vFov <= 0.0f || vFov >= math::PI) {
        throw invalid_argument(
            "TurntableCameraController vertical field of view must be finite and between 0 and 180 degrees.");
    }

    if (!math::is_finite(viewportHeight) || viewportHeight <= 0.0f) {
        throw invalid_argument("TurntableCameraController viewport height must be positive and finite.");
    }

    UpdateResult result {};

    const vec2 position = input.mousePosition();

    const vec2 inputDelta = input.mousePositionDelta();

    // turntable historically uses screen-coordinate deltas:
    // +X right, +Y down. DesktopInputContext mouse delta uses +Y up.
    const vec2 dragDelta {inputDelta.x, -inputDelta.y};

    auto orbit = [&](const vec2& delta) {
        if (delta.x == 0.0f && delta.y == 0.0f) {
            return;
        }

        const float previousYaw = _view.yaw;
        const float previousPitch = _view.pitch;

        _view.yaw -= delta.x * _config.orbitSensitivity;

        const float pitchDirection = _config.invertPitch ? 1.0f : -1.0f;
        _view.pitch = math::clamp(_view.pitch + delta.y * _config.orbitSensitivity * pitchDirection,
                                  _config.minPitch, _config.maxPitch);

        if (_view.yaw != previousYaw || _view.pitch != previousPitch) {
            result.cameraChanged = true;
        }
    };

    auto pan = [&](const vec2& delta) {
        if (delta.x == 0.0f && delta.y == 0.0f) {
            return;
        }

        const float worldUnitsPerPixel = 2.0f * _view.distance * math::tan(vFov * 0.5f) / viewportHeight;

        const float cosPitch = math::cos(_view.pitch);

        const vec3 targetToEye {
            math::sin(_view.yaw) * cosPitch,
            math::sin(_view.pitch),
            math::cos(_view.yaw) * cosPitch,
        };

        const vec3 forward = -targetToEye;
        const vec3 right = math::normalize(math::cross(forward, vec3 {0.0f, 1.0f, 0.0f}));
        const vec3 up = math::normalize(math::cross(right, forward));
        const vec3 translation = (-right * delta.x + up * delta.y) * worldUnitsPerPixel;

        translateTarget(translation);

        result.cameraChanged = true;
    };

    auto dolly = [&](float delta) {
        if (delta == 0.0f) {
            return;
        }

        const float previousDistance = _view.distance;

        _view.distance *= math::exp(delta * _config.dollySensitivity);
        _view.distance = math::clamp(_view.distance, _config.minDistance, _config.maxDistance);

        if (_view.distance != previousDistance) {
            result.cameraChanged = true;
        }
    };

    auto applyDrag = [&](DragMode mode, const vec2& delta) {
        switch (mode) {
            case DragMode::Orbit:
                orbit(delta);
                break;

            case DragMode::Pan:
                pan(delta);
                break;

            case DragMode::Dolly:
                dolly(delta.y);
                break;
        }
    };

    // orbit button: click candidate / orbit / modified drag

    if (input.mouseButtonPressed(_config.controls.orbitButton)) {

        _orbitButtonActive = true;
        _orbitButtonDragging = false;

        _orbitButtonPressPosition = position;

        if (input.keyDown(_config.controls.dollyModifier)) {
            _orbitButtonDragMode = DragMode::Dolly;
        }
        else if (input.keyDown(_config.controls.panModifier)) {
            _orbitButtonDragMode = DragMode::Pan;
        }
        else {
            _orbitButtonDragMode = DragMode::Orbit;
        }
    }

    if (_orbitButtonActive && input.mouseButtonDown(_config.controls.orbitButton)) {

        if (!_orbitButtonDragging) {

            const vec2  totalDelta = position - _orbitButtonPressPosition;
            const float dragDistance = math::length(totalDelta);

            if (dragDistance > 0.0f && dragDistance >= _config.dragThreshold) {

                _orbitButtonDragging = true;

                // apply the entire displacement from the original press so
                // crossing the threshold doesn't discard the first few pixels
                applyDrag(_orbitButtonDragMode, totalDelta);
            }
        }
        else {
            applyDrag(_orbitButtonDragMode, dragDelta);
        }
    }

    if (input.mouseButtonReleased(_config.controls.orbitButton)) {

        if (_orbitButtonActive && !_orbitButtonDragging) {
            result.orbitButtonClick = PointerClick {
                .position = position,
            };
        }

        _orbitButtonActive = false;
        _orbitButtonDragging = false;
    }

    // dedicated pan button

    // dedicated pan button: click candidate / pan

    if (input.mouseButtonPressed(_config.controls.panButton)) {

        _panButtonActive = true;
        _panButtonDragging = false;
        _panButtonPressPosition = position;
    }

    if (_panButtonActive && input.mouseButtonDown(_config.controls.panButton)) {

        if (!_panButtonDragging) {

            const vec2  totalDelta = position - _panButtonPressPosition;
            const float dragDistance = math::length(totalDelta);

            if (dragDistance > 0.0f && dragDistance >= _config.dragThreshold) {

                _panButtonDragging = true;

                // apply the entire displacement from the original press so
                // crossing the threshold doesn't discard the first few pixels
                pan(totalDelta);
            }
        }
        else {
            pan(dragDelta);
        }
    }

    if (input.mouseButtonReleased(_config.controls.panButton)) {

        if (_panButtonActive && !_panButtonDragging) {
            result.panButtonClick = PointerClick {
                .position = position,
            };
        }

        _panButtonActive = false;
        _panButtonDragging = false;
    }

    // wheel / trackpad dolly

    const float scroll = input.mouseScrollWheelDelta().y;

    if (scroll != 0.0f) {

        const float previousDistance = _view.distance;

        _view.distance *= math::exp(-scroll * _config.scrollDollySensitivity);

        _view.distance = math::clamp(_view.distance, _config.minDistance, _config.maxDistance);

        if (_view.distance != previousDistance) {
            result.cameraChanged = true;
        }
    }

    result.pointerDragging = _orbitButtonDragging || _panButtonDragging;

    return result;
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

void TurntableCameraController::translateTarget(const vec3& worldTranslation) {

    if (!IsFinite(worldTranslation)) {
        throw runtime_error("TurntableCameraController target translation must be finite.");
    }

    // refresh the cached world position first. this matters when the target
    // is attached to a Node that may have moved since the previous frame
    resolveTarget();

    if (auto* worldPosition = std::get_if<vec3>(&_view.target)) {

        *worldPosition += worldTranslation;
        _resolvedTarget = *worldPosition;
        return;
    }

    auto& nodeTarget = std::get<NodeTarget>(_view.target);

    if (auto node = nodeTarget.node.lock()) {

        // translation is a direction/vector, not a position, so W must be zero.
        // this removes translation from the inverse transform while correctly
        // accounting for the Node's rotation and scale
        const vec4 localTranslation4 = math::inverse(node->worldTransform()) * vec4 {worldTranslation, 0.0f};

        const vec3 localTranslation {localTranslation4};

        if (!IsFinite(localTranslation)) {
            throw runtime_error("TurntableCameraController resolved local target translation is not finite.");
        }

        nodeTarget.localPosition += localTranslation;

        resolveTarget();
    }
    else {

        // the Node has disappeared. preserve the controller's existing
        // freeze-at-last-world-position behavior, but convert it to a
        // world-space target now that the user is explicitly moving it
        _resolvedTarget += worldTranslation;
        _view.target = _resolvedTarget;
    }
}

/// Private Static Non-Member Functions ///

bool IsFinite(const vec3& value) {
    return math::is_finite(value.x) && math::is_finite(value.y) && math::is_finite(value.z);
}

void ValidateConfig(const TurntableCameraController::Config& config) {

    if (!math::is_finite(config.minPitch) || !math::is_finite(config.maxPitch)) {
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

    if (!math::is_finite(config.minDistance) || config.minDistance <= 0.0f) {
        throw invalid_argument("TurntableCameraController minimum distance must be positive and finite.");
    }

    if (!math::is_finite(config.maxDistance)) {
        throw invalid_argument("TurntableCameraController maximum distance must be finite.");
    }

    if (config.maxDistance < config.minDistance) {
        throw invalid_argument(
            "TurntableCameraController maximum distance must not be less than minimum distance.");
    }

    if (!math::is_finite(config.orbitSensitivity) || config.orbitSensitivity < 0.0f) {
        throw invalid_argument("TurntableCameraController orbit sensitivity must be finite and non-negative.");
    }

    if (!math::is_finite(config.dollySensitivity) || config.dollySensitivity < 0.0f) {
        throw invalid_argument("TurntableCameraController dolly sensitivity must be finite and non-negative.");
    }

    if (!math::is_finite(config.scrollDollySensitivity) || config.scrollDollySensitivity < 0.0f) {

        throw invalid_argument(
            "TurntableCameraController scroll dolly sensitivity must be finite and non-negative.");
    }

    if (!math::is_finite(config.dragThreshold) || config.dragThreshold < 0.0f) {
        throw invalid_argument("TurntableCameraController drag threshold must be finite and non-negative.");
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
