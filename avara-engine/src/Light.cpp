//
//  Light.cpp
//	avara-engine
//
//  Created by Morgan Davis on 10/21/16.
//  Copyright © 2016 Morgan K Davis. All rights reserved.
//

#include "Light.h"

#include "Color.h"
#include "Node.h"


using namespace ae;
using namespace std;


/***************************************************************************************
     Static
 ***************************************************************************************/

shared_ptr<Light> Light::DefaultAmbient() {
	static shared_ptr<Light> light = nullptr;
	if (!light) {
		light = make_shared<Light>(LIGHT_TYPE::AMBIENT, make_shared<Color>(0.25, 0.25, 0.25, 1.0));
	}
	return light;
}

shared_ptr<Light> Light::DefaultPoint() {
	static shared_ptr<Light> light = nullptr;
	if (!light) {
		light = make_shared<Light>(LIGHT_TYPE::POINT, make_shared<Color>(Color::White()));
		light->attenuationFactor(0.0);
	}
	return light;
}

shared_ptr<Node> Light::DefaultAmbientNode() {
	static shared_ptr<Node> node = nullptr;
	if (!node) {
		node = make_shared<Node>();
		node->light(DefaultAmbient());
	}
	return node;
}

shared_ptr<Node> Light::DefaultPointNode() {
	static shared_ptr<Node> node = nullptr;
	if (!node) {
		node = make_shared<Node>();
		node->light(DefaultPoint());
	}
	return node;
}

/***************************************************************************************
     Lifecycle
 ***************************************************************************************/

Light::Light(LIGHT_TYPE type):
	Light(type, make_shared<Color>(Color::White())) {
	
}

Light::Light(LIGHT_TYPE type, const shared_ptr<Color> color):
	m_name(boost::none),
	m_type(type),
	m_color(color),
	m_attenuationFactor(1.0f),
	m_node(weak_ptr<Node>()) {
	
}

/***************************************************************************************
     Public
 ***************************************************************************************/

boost::optional<string> Light::name() const {
	return m_name;
}

void Light::name(const string& name) {
	m_name = name;
}

LIGHT_TYPE Light::type() const {
	return m_type;
}

void Light::type(LIGHT_TYPE type) {
	m_type = type;
}

shared_ptr<Color> Light::color() const {
	return m_color;
}

void Light::color(shared_ptr<Color> color) {
	m_color = color;
}

float Light::attenuationFactor() const {
	return m_attenuationFactor;
}

void Light::attenuationFactor(float factor) {
	m_attenuationFactor = factor;
}

/***************************************************************************************
     Internal
 ***************************************************************************************/

weak_ptr<Node> Light::node() const {
	return m_node;
}

//void Light::node(shared_ptr<Node> node) {
//	m_node = node;
//}

void Light::attachedToNode(shared_ptr<Node> node) {
	m_node = node;
}
