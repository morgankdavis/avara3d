//
//  Camera.cc
//  avara3d
//
//  Created by Morgan Davis on 10/21/16.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#include "a3d/rendering/camera/Camera.h"

#include <iostream>

#include "a3d/Utilities.h"
#include "a3d/diagnostic/logging/Logger.h"


using namespace a3d;
using namespace std;
using namespace glm;


/*********************************************************************************************
	Public Lifecycle
 *********************************************************************************************/

Camera::Camera():
		_name{} {}

Camera::Camera(const string& name):
		_name{name} {}

Camera::~Camera() {

	if (_name != nullopt) {
		A3D_LOG_D("Destroying Camera '{}' ({:p})", *_name, static_cast<void*>(this));
	}
	else {
		A3D_LOG_D("Destroying Camera {:p}", static_cast<void*>(this));
	}
}

/*********************************************************************************************
	Public Members
 *********************************************************************************************/

const optional<string>& Camera::name() const {
	return _name;
}

void Camera::name(const string& name) {
	_name = name;
}

/*********************************************************************************************
	Internal Members
 *********************************************************************************************/

mat4 Camera::projection() const {
	return _projection;
}
