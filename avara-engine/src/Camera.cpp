//
//  Camera.cpp
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


/***************************************************************************************
     MARK:   Lifecycle
 **************************************************************************************/

Camera::Camera():
	m_fov(radians(45.0)),
	m_zNear(0.1),
	m_zFar(5000.0),
	m_aspectRatio(1.0) {

		constructProjectionMat();
}

Camera::Camera(float zNear, float zFar, float fov):
	m_zNear(zNear),
	m_zFar(zFar),
	m_fov(radians(fov)),
	m_aspectRatio(1.0) {

		constructProjectionMat();
}

/***************************************************************************************
     MARK:   Public
 **************************************************************************************/

string Camera::name() {
	return m_name;
}

void Camera::name(string name) {
	m_name = name;
}

void Camera::translate(vec3 translation) {
	
}

void Camera::rotate(vec3 rotation) {
	
}

float Camera::fov() {
	return m_fov;
	constructProjectionMat();
}

void Camera::fov(float fov) {
	m_fov = fov;
}

float Camera::zNear() {
	return m_zNear;
}

void Camera::zNear(float zNear) {
	m_zNear = zNear;
	constructProjectionMat();
}

float Camera::zFar() {
	return m_zFar;
}

void Camera::zFar(float zFar) {
	m_zFar = zFar;
	constructProjectionMat();
}

float Camera::aspectRatio() {
	return m_aspectRatio;
}

void Camera::aspectRatio(float ratio) {
	// small optimization as Window::mainLoop() calls this every draw
	if (!utils::FloatEqual(ratio, m_aspectRatio, 0.001)) {
		m_aspectRatio = ratio;
		constructProjectionMat();
	}
}

mat4 Camera::projection() {
	return m_projection;
}

//void Camera::projection(mat4 projection) {
//	m_projection = projection;
//	// compute constituent properties??
//}

/***************************************************************************************
     MARK:   Internal
 **************************************************************************************/

void Camera::constructProjectionMat() {
	
	m_projection = perspective(m_fov,
							   m_aspectRatio,
							   m_zNear,
							   m_zFar);
}

Node* Camera::node() const {
	return m_node;
}

void Camera::node(Node* node) {
	m_node = node;
}

