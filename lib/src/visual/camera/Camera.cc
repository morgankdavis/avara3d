//
//  Camera.cc
//  avara3d
//
//  Created by Morgan Davis on 10/21/16.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#include "a3d/visual/camera/Camera.h"

#include <iostream>

#include "a3d/log/Log.h"

using namespace a3d;
using namespace std;
using namespace a3d::math;

/// Public Lifecycle Functions ///

Camera::Camera():
    _name {} {}

Camera::Camera(const string& name):
    _name {name} {}

Camera::~Camera() {

    if (_name != nullopt) {
        log::d()("Destroying Camera '{}' ({:p})", *_name, static_cast<void*>(this));
    }
    else {
        log::d()("Destroying Camera {:p}", static_cast<void*>(this));
    }
}

/// Public Member Functions ///

const optional<string>& Camera::name() const {
    return _name;
}

void Camera::name(const string& name) {
    _name = name;
}

/// Internal Member Functions ///

//mat4 Camera::projection() const {
//	return _projection;
//}
