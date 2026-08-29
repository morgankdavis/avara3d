//
//  PointLight.cc
//  avara3d
//
//  Created by Morgan Davis on 7/31/2024.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#include "a3d/visual/light/PointLight.h"

#include "a3d/Color.h"
#include "a3d/log/Log.h"

using namespace a3d;
using namespace std;

// [Public Lifecycle Functions]

PointLight::PointLight():
    Light() {
    _intensity = 1.0f;
}

PointLight::PointLight(const string& name):
    PointLight() {
    _name = name;
    _intensity = 1.0f;
}

PointLight::PointLight(const Color& color):
    PointLight() {
    _color = color;
    _intensity = 1.0f;
}

PointLight::PointLight(const string& name, const Color& color):
    PointLight() {
    _name = name;
    _color = color;
    _intensity = 1.0f;
}

// [Public Member Functions]

float PointLight::intensity() const {
    return _intensity;
}

void PointLight::intensity(float intensity) {
    _intensity = intensity;
}

const Attenuation& PointLight::attenuation() const {
    return _attenuation;
}

void PointLight::attenuation(const Attenuation& attenuation) {
    _attenuation = attenuation;
}
