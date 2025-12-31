//
//  Light.cc
//  avara3d
//
//  Created by Morgan Davis on 10/21/16.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#include "a3d/rendering/light/Light.h"

#include "a3d/Color.h"
#include "a3d/diagnostic/log/Log.h"
#include "a3d/rendering/light/AmbientLight.h"
#include "a3d/rendering/light/DirectionalLight.h"
#include "a3d/rendering/light/PointLight.h"
#include "a3d/rendering/light/SpotLight.h"

using namespace a3d;
using namespace std;

/// Public Static Member Functions ///

shared_ptr<AmbientLight> Light::AmbientLight() {
	return make_unique<a3d::AmbientLight>();
}

shared_ptr<AmbientLight> Light::AmbientLight(const shared_ptr<Color>& color) {
	return make_unique<a3d::AmbientLight>(color);
}

shared_ptr<DirectionalLight> Light::DirectionalLight() {
	return make_unique<a3d::DirectionalLight>();
}

shared_ptr<DirectionalLight> Light::DirectionalLight(const shared_ptr<Color>& color) {
	return make_unique<a3d::DirectionalLight>(color);
}

shared_ptr<PointLight> Light::PointLight() {
	return make_unique<a3d::PointLight>();
}

shared_ptr<PointLight> Light::PointLight(const shared_ptr<Color>& color) {
	return make_unique<a3d::PointLight>(color);
}

shared_ptr<SpotLight> Light::SpotLight() {
	return make_unique<a3d::SpotLight>();
}

shared_ptr<SpotLight> Light::SpotLight(const shared_ptr<Color>& color) {
	return make_unique<a3d::SpotLight>(color);
}

/// Public Lifecycle Functions ///

Light::~Light() {

	if (_name != nullopt) {
		log::d()("Destroying Light '{}' ({:p})", *_name, static_cast<void*>(this));
	}
	else {
		log::d()("Destroying Light {:p}", static_cast<void*>(this));
	}
}

/// Private Lifecycle Functions ///

Light::Light():
	_name{nullopt},
	_color{Color::White()} {}

/// Public Member Functions ///

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
