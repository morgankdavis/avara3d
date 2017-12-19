//
//  Light.cpp
//	avara-engine
//
//  Created by Morgan Davis on 10/21/16.
//  Copyright © 2016 Morgan K Davis. All rights reserved.
//

#include "Light.h"

#include "Color.h"


using namespace ae;
using namespace std;


/***************************************************************************************
     MARK:   Lifecycle
 **************************************************************************************/

Light::Light(LightType type):
	Light(type, make_shared<Color>(Color::White())) {
	
}

Light::Light(LightType type, const shared_ptr<Color> color):
	m_name(boost::none),
	m_type(type),
	m_color(color) {
	
}

/***************************************************************************************
     MARK:   Public
 **************************************************************************************/

boost::optional<string> Light::name() const {
	return m_name;
}

void Light::name(const string& name) {
	m_name = name;
}

/***************************************************************************************
     MARK:   Internal
 **************************************************************************************/

Node* Light::node() const {
	return m_node;
}

void Light::node(Node* node) {
	m_node = node;
}
