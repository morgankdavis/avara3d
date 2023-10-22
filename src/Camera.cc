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
	Camera(0.1, 1000.0, 45.0, PROJECTION_TYPE::PERSPECTIVE) {

}

Camera::Camera(float zNear, float zFar, float fov, PROJECTION_TYPE projectionType):
	_name(std::nullopt),
	_zNear(zNear),
	_zFar(zFar),
	_fov(radians(fov)),
	_projectionType(projectionType),
	_aspectRatio(1.0),
	_node({}) {

	constructProjection();
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

float Camera::fov() const {
	return _fov;
}

void Camera::fov(float fov) {
	//if (fov > 0 || fov < M_PI) {
		_fov = fov;
	constructProjection();
	//}
}

float Camera::zNear() const {
	return _zNear;
}

void Camera::zNear(float zNear) {
	_zNear = zNear;
	constructProjection();
}

float Camera::zFar() const {
	return _zFar;
}

void Camera::zFar(float zFar) {
	_zFar = zFar;
	constructProjection();
}

float Camera::aspectRatio() const {
	return _aspectRatio;
}

void Camera::aspectRatio(float ratio) {
	// small optimization as Window::mainLoop() calls this every draw
	if (!utils::Equal(ratio, _aspectRatio, 0.001)) {
		_aspectRatio = ratio;
		constructProjection();
	}
}

//PROJECTION_TYPE Camera::projectionType() const {
//	return _projectionType;
//}

//void Camera::projectionType(PROJECTION_TYPE type) {
//	_projectionType = type;
//	constructProjection();
//}

mat4 Camera::projection() const {
	return _projection;
}

/*********************************************************************************************
	Internal
 *********************************************************************************************/

void Camera::constructProjection() {

//	if (_projectionType == PROJECTION_TYPE::PERSPECTIVE) {
//
//		_projection = perspective(_fov,
//								  _aspectRatio,
//								  _zNear,
//								  _zFar);
//	}
//	else {
//			// ortho(T left, T right, T bottom, T top, T zNear, T zFar)
////			_projection = ortho(_fov,
////									  _aspectRatio,
////									  _zNear,
////									  _zFar);
//	}
}

weak_ptr<Node> Camera::node() const {
	return _node;
}

void Camera::attachedToNode(std::shared_ptr<ae::Node> node) {
	_node = node;
}

