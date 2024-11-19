//
//  Light.cc
//  avara3d
//
//  Created by Morgan Davis on 10/21/16.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#include "a3d/rendering/light/Light.h"

#include "a3d/Color.h"
#include "a3d/diagnostic/logging/Logger.h"


using namespace a3d;
using namespace std;


/*********************************************************************************************
	Public Lifecycle Functions
 *********************************************************************************************/

Light::Light(const string& name):
		Light() {
	_name = name;
	_type = LightType::Point;
}

Light::Light(const shared_ptr<Color>& color):
		Light() {
	_color = color;
	_type = LightType::Point;
}

Light::Light(const string& name, const shared_ptr<Color>& color):
		Light() {
	_name = name;
	_color = color;
	_type = LightType::Point;
}

Light::~Light() {

	if (_name != nullopt) {
		A3D_LOG_D("Destroying Light '{}' ({:p})", *_name, static_cast<void*>(this));
	}
	else {
		A3D_LOG_D("Destroying Light {:p}", static_cast<void*>(this));
	}
}

/*********************************************************************************************
	Private Lifecycle Functions
 *********************************************************************************************/

Light::Light():
	_name{nullopt},
	_color{Color::White()} {}

/*********************************************************************************************
	Public Member Functions
 *********************************************************************************************/

const optional<string>& Light::name() const {
	return _name;
}

void Light::name(const string& name) {
	_name = name;
}

const shared_ptr<Color>& Light::color() const {
	return _color;
}

void Light::color(const shared_ptr<Color>& color) {
	_color = color;
}

LightType Light::type() const {
	return _type;
}

void Light::type(LightType type) {
	_type = type;
}
