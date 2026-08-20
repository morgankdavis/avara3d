//
//  AmbientLight.cc
//  avara3d
//
//  Created by Morgan Davis on 7/31/2024.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#include "a3d/visual/light/AmbientLight.h"

#include "a3d/Color.h"

using namespace a3d;
using namespace std;

// [Public Lifecycle Functions]

AmbientLight::AmbientLight():
    Light() {}

AmbientLight::AmbientLight(const string& name):
    AmbientLight() {
    _name = name;
}

AmbientLight::AmbientLight(const Color& color):
    AmbientLight() {
    _color = color;
}

AmbientLight::AmbientLight(const string& name, const Color& color):
    AmbientLight() {
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
