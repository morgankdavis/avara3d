//
//  SpotLight.cc
//  avara3d
//
//  Created by Morgan Davis on 7/31/2024.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#include "a3d/rendering/light/SpotLight.h"

#include "a3d/Color.h"
#include "a3d/diagnostic/logging/Logger.h"


using namespace a3d;
using namespace std;


/*********************************************************************************************
	Public Lifecycle Functions
 *********************************************************************************************/

SpotLight::SpotLight():
		AttenuatedLight() {
	_innerAngle = {10.0};
	_outerAngle = {15.0};
}

SpotLight::SpotLight(const string& name):
		SpotLight() {
	_name = name;
}

SpotLight::SpotLight(const shared_ptr<Color>& color):
		SpotLight() {
	_color = color;
}

SpotLight::SpotLight(const string& name, const shared_ptr<Color>& color):
		SpotLight() {
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

float SpotLight::innerAngle() const {
	return _innerAngle;
}

void SpotLight::innerAngle(float angle) {
	_innerAngle = angle;
}

float SpotLight::outerAngle() const {
	return _outerAngle;
}

void SpotLight::outerAngle(float angle) {
	_outerAngle = angle;
}

/*********************************************************************************************
	Private Lifecycle Functions
 *********************************************************************************************/

//SpotLight::SpotLight():
//		_innerAngle{10.0},
//		_outerAngle{15.0} {}
