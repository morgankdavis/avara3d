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
#include "a3d/rendering/light/AmbientLight.h"
#include "a3d/rendering/light/DirectionalLight.h"
#include "a3d/rendering/light/PointLight.h"
#include "a3d/rendering/light/SpotLight.h"


using namespace a3d;
using namespace std;


/*********************************************************************************************
	Public Static Member Functions
 *********************************************************************************************/

bool Light::classof(const Light*) {
	return true;
}

bool Light::classof(const Light& o) {
	return classof(&o);
}

shared_ptr<AmbientLight> Light::AmbientLight() {
	return make_shared<a3d::AmbientLight>();
}

shared_ptr<AmbientLight> Light::AmbientLight(const shared_ptr<Color>& color) {
	return make_shared<a3d::AmbientLight>(color);
}

shared_ptr<DirectionalLight> Light::DirectionalLight() {
	return make_shared<a3d::DirectionalLight>();
}

shared_ptr<DirectionalLight> Light::DirectionalLight(const shared_ptr<Color>& color) {
	return make_shared<a3d::DirectionalLight>(color);
}

shared_ptr<PointLight> Light::PointLight() {
	return make_shared<a3d::PointLight>();
}

shared_ptr<PointLight> Light::PointLight(const shared_ptr<Color>& color) {
	return make_shared<a3d::PointLight>(color);
}

shared_ptr<SpotLight> Light::SpotLight() {
	return make_shared<a3d::SpotLight>();
}

shared_ptr<SpotLight> Light::SpotLight(const shared_ptr<Color>& color) {
	return make_shared<a3d::SpotLight>(color);
}

/*********************************************************************************************
	Public Lifecycle Functions
 *********************************************************************************************/

Light::~Light() {

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

Light::Kind Light::kind() const {
	return _kind;
}

/*********************************************************************************************
	Protected Lifecycle Functions
 *********************************************************************************************/

Light::Light(Light::Kind k):
		_kind{k},
		_name{nullopt},
		_color{Color::White()} {}
