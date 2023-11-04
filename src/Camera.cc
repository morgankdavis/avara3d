//
//  Camera.cc
//	avara-engine
//
//  Created by Morgan Davis on 10/21/16.
//  Copyright © 2016 Morgan K Davis. All rights reserved.
//

#include "Camera.h"

#include <iostream>

#include "Utilities.h"


using namespace ae;
using namespace std;
using namespace glm;


/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

Camera::Camera():
		_name(nullopt) {

}

Camera::Camera(optional<string> name):
		_name(name) {

}

/*********************************************************************************************
	Public
 *********************************************************************************************/

std::optional<std::string> Camera::name() const {
	return _name;
}

void Camera::name(string name) {
	_name = name;
}

mat4 Camera::projection() const {
	return _projection;
}

/*********************************************************************************************
	Internal
 *********************************************************************************************/

//weak_ptr<Node> Camera::node() const {
//	return _node;
//}

//void Camera::attachedToNode(std::shared_ptr<ae::Node> node) {
//	_node = node;
//}
