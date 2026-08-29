//
//  DirectionalLight.cc
//  avara3d
//
//  Created by Morgan Davis on 7/31/2024.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#include "a3d/visual/light/DirectionalLight.h"

#include "a3d/Color.h"
#include "a3d/log/Log.h"

using namespace a3d;
using namespace a3d::math;
using namespace std;

// [Public Lifecycle Functions]

DirectionalLight::DirectionalLight():
    Light() {
    _intensity = 1.0f;
}

DirectionalLight::DirectionalLight(const string& name):
    DirectionalLight() {
    _name = name;
    _intensity = 1.0f;
}

DirectionalLight::DirectionalLight(const Color& color):
    DirectionalLight() {
    _color = color;
    _intensity = 1.0f;
}

DirectionalLight::DirectionalLight(const string& name, const Color& color):
    DirectionalLight() {
    _name = name;
    _color = color;
    _intensity = 1.0f;
}

// [Public Member Functions]

float DirectionalLight::intensity() const {
    return _intensity;
}

void DirectionalLight::intensity(float intensity) {
    _intensity = intensity;
}
