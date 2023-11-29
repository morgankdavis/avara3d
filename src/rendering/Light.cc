//
//  Light.cc
//	avara-engine
//
//  Created by Morgan Davis on 10/21/16.
//  Copyright © 2016 Morgan K Davis. All rights reserved.
//

#include "rendering/Light.h"

#include "utilities/Color.h"


using namespace ae;
using namespace std;


/*********************************************************************************************
	Public Static
 *********************************************************************************************/

shared_ptr<Light> Light::DefaultAmbient() {
	static shared_ptr<Light> light = nullptr;
	if (!light) {
		light = make_shared<Light>(LIGHT_TYPE::AMBIENT, make_shared<Color>(0.25f, 0.25, 0.25, 1.0));
		//light = make_shared<Light>(LIGHT_TYPE::AMBIENT, Color::White());
	}
	return light;
}

shared_ptr<Light> Light::DefaultPoint() {
	static shared_ptr<Light> light = nullptr;
	if (!light) {
		light = make_shared<Light>(LIGHT_TYPE::POINT, Color::White());
		light->attenuationFactor(0.0);
	}
	return light;
}

/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

Light::Light(LIGHT_TYPE type):
	Light(type, Color::White()) {
	
}

Light::Light(LIGHT_TYPE type, const shared_ptr<Color> color):
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

LIGHT_TYPE Light::type() const {
	return _type;
}

void Light::type(LIGHT_TYPE type) {
	_type = type;
}

shared_ptr<Color> Light::color() const {
	return _color;
}

void Light::color(shared_ptr<Color> color) {
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
