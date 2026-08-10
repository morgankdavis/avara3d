//
//  Background.cc
//  avara3d
//
//  Created by Morgan Davis on 8/9/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#include "a3d/visual/Background.h"

using namespace a3d;
using namespace a3d::math;

/// Public Lifecycle Functions ///

Background::Background(const Material::Property& contents):
    _contents {contents},
    _orientation {1.0f} {}

/// Public Member Functions ///

const Material::Property& Background::contents() const {
    return _contents;
}

quat Background::orientation() const {
    return _orientation;
}

void Background::orientation(const quat& orientation) {
    _orientation = orientation;
}
