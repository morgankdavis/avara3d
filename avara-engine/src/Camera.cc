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


/***************************************************************************************
     Lifecycle
 ***************************************************************************************/

Camera::Camera():
	Camera(0.1, 1000.0, 45.0) { 

}

Camera::Camera(float zNear, float zFar, float fov):
	m_zNear(zNear),
	m_zFar(zFar),
	m_fov(radians(fov)),
	m_aspectRatio(1.0),
	m_node({}) {

		constructProjectionMat();
}

/***************************************************************************************
     Public
 ***************************************************************************************/

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
}

void Camera::fov(float fov) {
	//if (fov > 0 || fov < M_PI) {
		m_fov = fov;
		constructProjectionMat();
	//}
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
	if (!utils::Equal(ratio, m_aspectRatio, 0.001)) {
		m_aspectRatio = ratio;
		constructProjectionMat();
	}
}

mat4 Camera::projection() {
	return m_projection;
}

/***************************************************************************************
     Internal
 ***************************************************************************************/

void Camera::constructProjectionMat() {
	
	m_projection = perspective(m_fov,
							   m_aspectRatio,
							   m_zNear,
							   m_zFar);
}

weak_ptr<Node> Camera::node() const {
	return m_node;
}

//void Camera::node(shared_ptr<Node> node) {
//	m_node = node;
//}

void Camera::attachedToNode(shared_ptr<Node> node) {
	m_node = node;
}

