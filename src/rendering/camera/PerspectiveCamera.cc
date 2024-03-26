//
// Created by mkd on 10/22/23.
//

#include "a3d/rendering/camera/PerspectiveCamera.h"

#include "glm/gtc/matrix_transform.hpp"

#include "a3d/Utilities.h"


using namespace a3d;
using namespace std;
using namespace glm;


PerspectiveCamera::PerspectiveCamera():
		Camera(),
		_zNear(0.1),
		_zFar(1000.0),
		_yFov(radians(45.0)),
		_aspectRatio(0) {}

PerspectiveCamera::PerspectiveCamera(float zNear, float zFar, float yFov):
		Camera(),
		_zNear(zNear),
		_zFar(zFar),
		_yFov(yFov) {

	constructProjectionMatrix();
}

PerspectiveCamera::PerspectiveCamera(const string& name, float zNear, float zFar, float yFov):
		Camera(name),
		_zNear(zNear),
		_zFar(zFar),
		_yFov(yFov) {

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

float PerspectiveCamera::yFov() const {
	return _yFov;
}

void PerspectiveCamera::yFov(float yFov) {
	//if (fov > 0 || fov < M_PI) {
	_yFov = yFov;
	constructProjectionMatrix();
	//}
}

float PerspectiveCamera::aspectRatio() const {
	return _aspectRatio;
}

void PerspectiveCamera::aspectRatio(float ratio) {
	// small optimization as Window::mainLoop() calls this every update
	if (!utils::Equal(ratio, _aspectRatio, 0.001)) {
		_aspectRatio = ratio;
		constructProjectionMatrix();
	}
}

void PerspectiveCamera::constructProjectionMatrix() {
	_projection = glm::perspective(_yFov,
								   _aspectRatio,
								   _zNear,
								   _zFar);
}