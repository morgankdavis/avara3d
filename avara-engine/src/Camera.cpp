//
//  Camera.cpp
//	avara-engine
//
//  Created by Morgan Davis on 10/21/16.
//  Copyright © 2016 Morgan K Davis. All rights reserved.
//

#include "Camera.h"


using namespace ae;
using namespace std;
using namespace glm;


/***************************************************************************************
     MARK:   Lifecycle
 **************************************************************************************/

Camera::Camera():
	m_fov(radians(45.0)),
	m_zNear(0.01),
	m_zFar(1000.0) {

	// TODO: HARD CODED VIEWPORT SIZE!
		m_projection = perspective(m_fov,
								   800.0f/600.0f,
								   m_zNear,
								   m_zFar);
}

Camera::Camera(const float zNear, const float zFar, const float fov):
	m_zNear(zNear),
	m_zFar(zFar),
	m_fov(radians(fov)) {

	// TODO: HARD CODED VIEWPORT SIZE!
	m_projection = perspective(m_fov,
							   800.0f/600.0f,
							   m_zNear,
							   m_zFar);
}

/***************************************************************************************
     MARK:   Public
 **************************************************************************************/

string Camera::name() {
	return m_name;
}

void Camera::name(const string name) {
	m_name = name;
}

//vec3 Camera::forward() const {
//	return vec3(0.0f);
//}
//
//vec3 Camera::up() const {
//	return vec3(0.0f);
//}
//
//vec3 Camera::right() const {
//	return vec3(0.0f);
//}

void Camera::translate(const vec3 translation) {
	
}

void Camera::rotate(const vec3 rotation) {
	
}

float Camera::fov() {
	return m_fov;
	// recompute projection
}

void Camera::fov(const float fov) {
	m_fov = fov;
}

float Camera::zNear() {
	return m_zNear;
}

void Camera::zNear(const float zNear) {
	m_zNear = zNear;
	// recompute projection
}

float Camera::zFar() {
	return m_zFar;
}

void Camera::zFar(const float zFar) {
	m_zFar = zFar;
	// recompute projection
}

mat4 Camera::projection() {
	return m_projection;
}

void Camera::projection(const mat4 projection) {
	m_projection = projection;
	// compute constituent properties??
}

/***************************************************************************************
     MARK:   Internal
 **************************************************************************************/

Node* Camera::node() const {
	return m_node;
}

void Camera::node(Node* node) {
	m_node = node;
}

