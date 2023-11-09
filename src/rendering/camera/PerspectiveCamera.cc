//
// Created by mkd on 10/22/23.
//

#include "rendering/camera/PerspectiveCamera.h"

#include "glm/gtc/matrix_transform.hpp"

#include "utilities/Utilities.h"


using namespace ae;
using namespace std;
using namespace glm;


PerspectiveCamera::PerspectiveCamera():
	Camera(),
	_zNear(0.1),
	_zFar(1000.0),
	_fov(radians(45.0)) {
}

PerspectiveCamera::PerspectiveCamera(float zNear, float zFar, float fov):
		PerspectiveCamera(nullopt, zNear, zFar, fov) {

}

PerspectiveCamera::PerspectiveCamera(optional<string> name, float zNear, float zFar, float fov):
		Camera(name),
		_zNear(zNear),
		_zFar(zFar),
		_fov(radians(fov)) {
	constructProjectionMatrix();
}

float PerspectiveCamera::zNear() const {
	return _zNear;
}

void PerspectiveCamera::zNear(float zNear) {
	_zNear = zNear;
	constructProjectionMatrix();
}

float PerspectiveCamera::zFar() const {
	return _zFar;
}

void PerspectiveCamera::zFar(float zFar) {
	_zFar = zFar;
	constructProjectionMatrix();
}

float PerspectiveCamera::fov() const {
	return _fov;
}

void PerspectiveCamera::fov(float fov) {
	//if (fov > 0 || fov < M_PI) {
	_fov = fov;
	constructProjectionMatrix();
	//}
}

float PerspectiveCamera::aspectRatio() const {
	return _aspectRatio;
}

void PerspectiveCamera::aspectRatio(float ratio) {
	// small optimization as Window::mainLoop() calls this every draw
	if (!utils::Equal(ratio, _aspectRatio, 0.001)) {
		_aspectRatio = ratio;
		constructProjectionMatrix();
	}
}

void PerspectiveCamera::constructProjectionMatrix() {
	_projection = glm::perspective(_fov,
								   _aspectRatio,
								   _zNear,
								   _zFar);
}