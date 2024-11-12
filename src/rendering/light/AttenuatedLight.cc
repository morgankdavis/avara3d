//
//  AttenuatedLight.cc
//  avara3d
//
//  Created by Morgan Davis on 7/31/2024.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#include "a3d/rendering/light/AttenuatedLight.h"

#include "a3d/diagnostic/logging/Logger.h"


using namespace a3d;
using namespace std;


/*********************************************************************************************
	Public Lifecycle Functions
 *********************************************************************************************/

AttenuatedLight::~AttenuatedLight() {

	if (_name != nullopt) {
		A3D_LOG_D("Destroying Light '{}' ({:p})", *_name, static_cast<void*>(this));
	}
	else {
		A3D_LOG_D("Destroying Light {:p}", static_cast<void*>(this));
	}
}

/*********************************************************************************************
	Public Member Functions
 *********************************************************************************************/

float AttenuatedLight::constantAttenuation() const {
	return _constantAttenuation;
}

void AttenuatedLight::constantAttenuation(float factor) {
	_constantAttenuation = factor;
}

float AttenuatedLight::linearAttenuation() const {
	return _linearAttenuation;
}

void AttenuatedLight::linearAttenuation(float factor) {
	_linearAttenuation = factor;
}

float AttenuatedLight::quadraticAttenuation() const {
	return _quadraticAttenuation;
}

void AttenuatedLight::quadraticAttenuation(float factor) {
	_quadraticAttenuation = factor;
}

/*********************************************************************************************
	Private Lifecycle Functions
 *********************************************************************************************/

AttenuatedLight::AttenuatedLight():
		_constantAttenuation{0.0},
		_linearAttenuation{0.0},
		_quadraticAttenuation{0.0} {}
