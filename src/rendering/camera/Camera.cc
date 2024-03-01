//
//  Camera.cc
//	avara3d
//
//  Created by Morgan Davis on 10/21/16.
//  Copyright © 2016 Morgan K Davis. All rights reserved.
//

#include "a3d/rendering/camera/Camera.h"

#include <iostream>

#include "a3d/Utilities.h"


using namespace a3d;
using namespace std;
using namespace glm;


/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

Camera::Camera():
		_name(nullopt) {

}

Camera::Camera(string name):
		_name(name) {

}

/*********************************************************************************************
	Public
 *********************************************************************************************/

optional<string> Camera::name() const {
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

//void Camera::attachedToNode(shared_ptr<Node> node) {
//	_node = node;
//}
