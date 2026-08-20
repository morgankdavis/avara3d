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
    Light() {}

PointLight::PointLight(const string& name):
    PointLight() {
    _name = name;
}

PointLight::PointLight(const Color& color):
    PointLight() {
    _color = color;
}

PointLight::PointLight(const string& name, const Color& color):
    PointLight() {
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

const Attenuation& PointLight::attenuation() const {
    return _attenuation;
}

void PointLight::attenuation(const Attenuation& attenuation) {
    _attenuation = attenuation;
}
