//
//  AttenuatedLight.cc
//  avara3d
//
//  Created by Morgan Davis on 7/31/2024.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#include "a3d/rendering/light/AttenuatedLight.h"

#include "a3d/diagnostic/exception/NotImplementedException.h"
#include "a3d/diagnostic/log/Log.h"


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

LightCutoff AttenuatedLight::cutoff() const {
	throw NotImplementedException("AttenuatedLight cutoff is not yet implemented.");
	//return _cutoff;
}

void AttenuatedLight::cutoff(LightCutoff cutoff) {
	throw NotImplementedException("AttenuatedLight cutoff is not yet implemented.");
//	_cutoff = cutoff;
}

/*********************************************************************************************
	Private Lifecycle Functions
 *********************************************************************************************/

AttenuatedLight::AttenuatedLight():
		_constantAttenuation{1.0},
		_linearAttenuation{0.0},
		_quadraticAttenuation{0.1}
		/*_cutoff{std::monostate{}}*/ {}
