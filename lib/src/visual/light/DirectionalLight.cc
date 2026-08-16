//
//  DirectionalLight.cc
//  avara3d
//
//  Created by Morgan Davis on 7/31/2024.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#include "a3d/visual/light/DirectionalLight.h"

#include "a3d/Color.h"
#include "a3d/log/Log.h"

using namespace a3d;
using namespace a3d::math;
using namespace std;

/// Public Lifecycle Functions ///

DirectionalLight::DirectionalLight():
    Light() {}

DirectionalLight::DirectionalLight(const string& name):
    DirectionalLight() {
    _name = name;
}

DirectionalLight::DirectionalLight(const Color& color):
    DirectionalLight() {
    _color = color;
}

DirectionalLight::DirectionalLight(const string& name, const Color& color):
    DirectionalLight() {
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

/// Public Member Functions ///

//const vec3& DirectionalLight::direction() const {
//	return _direction;
//}
//
//void DirectionalLight::direction(const vec3& direction) {
//	_direction = direction;
//}
