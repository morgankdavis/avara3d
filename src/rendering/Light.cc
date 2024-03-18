//
//  Light.cc
//	avara3d
//
//  Created by Morgan Davis on 10/21/16.
//  Copyright © 2016 Morgan K Davis. All rights reserved.
//

#include "a3d/rendering/Light.h"

#include "a3d/Color.h"


using namespace a3d;
using namespace std;


/*********************************************************************************************
	Public Static
 *********************************************************************************************/

unique_ptr<Light> Light::DefaultAmbient() {
	return make_unique<Light>(LightType::Ambient, make_unique<Color>(0.25f, 0.25, 0.25, 1.0));
}

unique_ptr<Light> Light::DefaultPoint() {
	auto light = make_unique<Light>(LightType::Point, Color::White());
	light->attenuationFactor(0.0);
	return light;
}

/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

Light::Light(LightType type):
	Light(type, Color::White()) {
	
}

Light::Light(LightType type, unique_ptr<Color> color):
	_name(nullopt),
	_type(type),
	_color(std::move(color)),
	_attenuationFactor(1.0f) {
//	_node({}) {
	
}

/*********************************************************************************************
	Public
 *********************************************************************************************/

optional<string> Light::name() const {
	return _name;
}

void Light::name(const string& name) {
	_name = name;
}

LightType Light::type() const {
	return _type;
}

void Light::type(LightType type) {
	_type = type;
}

Color* Light::color() const {
	return _color.get();
}

void Light::color(unique_ptr<Color> color) {
	_color = std::move(color);
}

float Light::attenuationFactor() const {
	return _attenuationFactor;
}

void Light::attenuationFactor(float factor) {
	_attenuationFactor = factor;
}

/*********************************************************************************************
	Internal
 *********************************************************************************************/

//weak_ptr<Node> Light::node() const {
//	return _node;
//}
//
//void Light::attachedToNode(shared_ptr<Node> node) {
//	_node = node;
//}
