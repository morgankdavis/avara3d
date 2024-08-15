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
using namespace std;


/*********************************************************************************************
	Public Lifecycle Functions
 *********************************************************************************************/

DirectionalLight::DirectionalLight():
		Light() {}

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
