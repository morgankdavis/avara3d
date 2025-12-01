//
//  AmbientLight.cc
//  avara3d
//
//  Created by Morgan Davis on 7/31/2024.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#include "a3d/rendering/light/AmbientLight.h"
#include "a3d/Color.h"
#include "a3d/diagnostic/log/Log.h"


using namespace a3d;
using namespace std;


/*********************************************************************************************
	Public Lifecycle Functions
 *********************************************************************************************/

AmbientLight::AmbientLight():
	Light() {}

AmbientLight::AmbientLight(const string& name):
		AmbientLight() {
	_name = name;
}

AmbientLight::AmbientLight(const shared_ptr<Color>& color):
		AmbientLight() {
	_color = color;
}

AmbientLight::AmbientLight(const string& name, const shared_ptr<Color>& color):
		AmbientLight() {
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
