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

//unique_ptr<Light> Light::DefaultAmbient() {
//	return make_unique<Light>(LightType::Ambient, Color{0.25f, 0.25, 0.25, 1.0});
//}
//
//unique_ptr<Light> Light::DefaultPoint() {
//	auto light = make_unique<Light>(LightType::Point, *Color::White());
//	light->attenuationFactor(0.0);
//	return light;
//}

shared_ptr<Light> Light::DefaultAmbient() {
	return make_shared<Light>(LightType::Ambient, make_shared<Color>(0.25f, 0.25, 0.25, 1.0));
}

shared_ptr<Light> Light::DefaultPoint() {
	auto light = make_shared<Light>(LightType::Point, Color::White());
	light->attenuationFactor(0.0);
	return light;
}

/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

Light::Light(LightType type):
	Light(type, make_shared<Color>(1.0f, 1.0f, 1.0f)) {
	
}

Light::Light(LightType type, const shared_ptr<Color>& color):
	_name(nullopt),
	_type(type),
	_color(color),
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

shared_ptr<Color> Light::color() const {
	return _color;
}

void Light::color(const shared_ptr<Color>& color) {
	_color = color;
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
