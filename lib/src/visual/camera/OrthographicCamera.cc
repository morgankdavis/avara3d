//
//  OrthographicCamera.cc
//  avara3d
//
//  Created by Morgan Davis on 10/22/23.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#include "a3d/visual/camera/OrthographicCamera.h"

#include "a3d/log/Log.h"

using namespace a3d::math;
using namespace std;

namespace a3d {
// [Public Lifecycle Functions]

OrthographicCamera::OrthographicCamera():
    Camera {},
    _zNear {0.1f},
    _zFar {1000.0f},
    _ySize {2.0f} {}

OrthographicCamera::OrthographicCamera(float zNear, float zFar, float ySize):
    Camera {},
    _zNear {zNear},
    _zFar {zFar},
    _ySize {ySize} {}

OrthographicCamera::OrthographicCamera(const string& name, float zNear, float zFar, float ySize):
    Camera {name},
    _zNear {zNear},
    _zFar {zFar},
    _ySize {ySize} {}

OrthographicCamera::~OrthographicCamera() {

    if (_name != nullopt) {
        log::d()("Destroying OrthographicCamera '{}' ({:p})", *_name, static_cast<void*>(this));
    }
    else {
        log::d()("Destroying OrthographicCamera {:p}", static_cast<void*>(this));
    }
}

// [Public Member Functions]

float OrthographicCamera::zNear() const {
    return _zNear;
}

void OrthographicCamera::zNear(float zNear) {
    _zNear = zNear;
}

float OrthographicCamera::zFar() const {
    return _zFar;
}

void OrthographicCamera::zFar(float zFar) {
    _zFar = zFar;
}

float OrthographicCamera::ySize() const {
    return _ySize;
}

void OrthographicCamera::ySize(float ySize) {
    _ySize = ySize;
}

// [Camera Internal Member Functions]

mat4 OrthographicCamera::projection(const uvec2& viewportSize) const {

    const float aspect = float(viewportSize.x) / float(viewportSize.y);
    const float halfHeight = _ySize / 2.0f;
    const float halfWidth = halfHeight * aspect;

    return ortho(-halfWidth, halfWidth, -halfHeight, halfHeight, _zNear, _zFar);
}
} // namespace a3d
