//
//  DirectionalLight.cc
//  avara3d
//
//  Created by Morgan Davis on 7/31/2024.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#include "a3d/rendering/light/DirectionalLight.h"

#include "a3d/Color.h"
#include "a3d/diagnostic/logging/Logger.h"


using namespace a3d;
using namespace glm;
using namespace std;


/*********************************************************************************************
	Public Lifecycle Functions
 *********************************************************************************************/

DirectionalLight::DirectionalLight():
		Light(Light::Kind::Directional) {}

DirectionalLight::DirectionalLight(const string& name):
		DirectionalLight() {
	_name = name;
}

DirectionalLight::DirectionalLight(const shared_ptr<Color>& color):
		DirectionalLight() {
	_color = color;
}

DirectionalLight::DirectionalLight(const string& name, const shared_ptr<Color>& color):
		DirectionalLight() {
	_name = name;
	_color = color;
}

//Light::~Light() {
//
//	if (_name != nullopt) {
//		A3D_LOG_D("Destroying Light '{}' ({:p})", *_name, static_cast<void*>(this));
//	}
//	else {
//		A3D_LOG_D("Destroying Light {:p}", static_cast<void*>(this));
//	}
//}

/*********************************************************************************************
	Public Member Functions
 *********************************************************************************************/

//const vec3& DirectionalLight::direction() const {
//	return _direction;
//}
//
//void DirectionalLight::direction(const vec3& direction) {
//	_direction = direction;
//}
