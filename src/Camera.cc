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
	Camera(0.1, 1000.0, 45.0) { 

}

Camera::Camera(float zNear, float zFar, float fov):
	_name(boost::none),
	_zNear(zNear),
	_zFar(zFar),
	_fov(radians(fov)),
	_aspectRatio(1.0),
	_node({}) {

		constructProjectionMat();
}

/*********************************************************************************************
	Public
 *********************************************************************************************/

boost::optional<std::string> Camera::name() {
	return _name;
}

void Camera::name(string name) {
	_name = name;
}

void Camera::translate(vec3 translation) {
	
}

void Camera::rotate(vec3 rotation) {
	
}

float Camera::fov() {
	return _fov;
}

void Camera::fov(float fov) {
	//if (fov > 0 || fov < M_PI) {
		_fov = fov;
		constructProjectionMat();
	//}
}

float Camera::zNear() {
	return _zNear;
}

void Camera::zNear(float zNear) {
	_zNear = zNear;
	constructProjectionMat();
}

float Camera::zFar() {
	return _zFar;
}

void Camera::zFar(float zFar) {
	_zFar = zFar;
	constructProjectionMat();
}

float Camera::aspectRatio() {
	return _aspectRatio;
}

void Camera::aspectRatio(float ratio) {
	// small optimization as Window::mainLoop() calls this every draw
	if (!utils::Equal(ratio, _aspectRatio, 0.001)) {
		_aspectRatio = ratio;
		constructProjectionMat();
	}
}

mat4 Camera::projection() {
	return _projection;
}

/*********************************************************************************************
	Internal
 *********************************************************************************************/

void Camera::constructProjectionMat() {
	
	_projection = perspective(_fov,
							   _aspectRatio,
							   _zNear,
							   _zFar);
}

weak_ptr<Node> Camera::node() const {
	return _node;
}

void Camera::attachedToNode(shared_ptr<Node> node) {
	_node = node;
}

