//
//  SpotLight.cc
//  avara3d
//
//  Created by Morgan Davis on 7/31/2024.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#include "a3d/visual/light/SpotLight.h"

#include "a3d/Color.h"
#include "a3d/log/Log.h"

using namespace a3d;
using namespace a3d::math;
using namespace std;

// [Public Lifecycle Functions]

SpotLight::SpotLight():
    Light() {
    // see DeVries 16.5
    _innerAngleCos = math::radians(15.0f);
    _outerAngleCos = math::radians(25.0f);
    _featherMode = FeatheringMode::Linear;
}

SpotLight::SpotLight(const string& name):
    SpotLight() {
    _name = name;
}

SpotLight::SpotLight(const Color& color):
    SpotLight() {
    _color = color;
}

SpotLight::SpotLight(const string& name, const Color& color):
    SpotLight() {
    _name = name;
    _color = color;
}

//Light::~Light() {
//
//	if (_name != nullopt) {
//		log::d()("Destroying Light '{}' ({:p})", *_name, static_cast<void*>(this));
//	}
//	else {
//		log::d()("Destroying Light {:p}", static_cast<void*>(this));
//	}
//}

// [Public Member Functions]

float SpotLight::innerAngle() const {
    return math::acos(_innerAngleCos);
}

void SpotLight::innerAngle(float angle) {
    _innerAngleCos = static_cast<float>(math::cos(angle));
}

float SpotLight::outerAngle() const {
    return math::acos(_outerAngleCos);
}

void SpotLight::outerAngle(float angle) {
    _outerAngleCos = static_cast<float>(math::cos(angle));
}

SpotLight::FeatheringMode SpotLight::featheringMode() const {
    return _featherMode;
}

void SpotLight::featheringMode(FeatheringMode mode) {
    _featherMode = mode;
}

const Attenuation& SpotLight::attenuation() const {
    return _attenuation;
}

void SpotLight::attenuation(const Attenuation& attenuation) {
    _attenuation = attenuation;
}

// [Internal Member Functions]

float SpotLight::innerAngleCos() const {
    return _innerAngleCos;
}

float SpotLight::outerAngleCos() const {
    return _outerAngleCos;
}

// [Private Lifecycle Functions]

//SpotLight::SpotLight():
//		_innerAngle{10.0},
//		_outerAngle{15.0} {}
