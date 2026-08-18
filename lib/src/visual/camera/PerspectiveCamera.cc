//
//  PerspectiveCamera.cc
//  avara3d
//
//  Created by Morgan Davis on 10/22/23.
//  Copyright © 2023-2026 Morgan K Davis. All rights reserved.
//

#include "a3d/visual/camera/PerspectiveCamera.h"

#include "a3d/log/Log.h"

using namespace a3d;
using namespace std;
using namespace a3d::math;

/// Public Lifecycle Functions ///

PerspectiveCamera::PerspectiveCamera():
    Camera {},
    _zNear {0.1},
    _zFar {1000.0},
    _yFov {radians(45.0)} {}

PerspectiveCamera::PerspectiveCamera(float zNear, float zFar, float yFov):
    Camera {},
    _zNear {zNear},
    _zFar {zFar},
    _yFov {yFov} {}

PerspectiveCamera::PerspectiveCamera(const string& name, float zNear, float zFar, float yFov):
    Camera {name},
    _zNear {zNear},
    _zFar {zFar},
    _yFov {yFov} {}

PerspectiveCamera::~PerspectiveCamera() {

    if (_name != nullopt) {
        log::d()("Destroying PerspectiveCamera '{}' ({:p})", *_name, static_cast<void*>(this));
    }
    else {
        log::d()("Destroying PerspectiveCamera {:p}", static_cast<void*>(this));
    }
}

/// Public Member Functions ///

float PerspectiveCamera::zNear() const {
    return _zNear;
}

void PerspectiveCamera::zNear(float zNear) {
    _zNear = zNear;
}

float PerspectiveCamera::zFar() const {
    return _zFar;
}

void PerspectiveCamera::zFar(float zFar) {
    _zFar = zFar;
}

float PerspectiveCamera::yFov() const {
    return _yFov;
}

void PerspectiveCamera::yFov(float yFov) {
    _yFov = yFov;
}

/// Camera Internal Member Functions ///

mat4 PerspectiveCamera::projection(const uvec2& viewportSize) const {

    const float aspect = float(viewportSize.x) / float(viewportSize.y);
    return perspective(_yFov, aspect, _zNear, _zFar);
}
