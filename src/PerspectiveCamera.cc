//
// Created by mkd on 10/22/23.
//

#include "PerspectiveCamera.h"

#include <glm/gtc/matrix_transform.hpp>

#include "Utilities.h"


using namespace ae;
using namespace std;
using namespace glm;


PerspectiveCamera::PerspectiveCamera():
	Camera(),
	_fov(radians(45.0)) {

}

PerspectiveCamera::PerspectiveCamera(optional<string> name,
									 float zNear, float zFar,
									 float fov):
		Camera(name, zNear, zFar),
		_fov(radians(fov)) {

}

float PerspectiveCamera::fov() const {
	return _fov;
}

void PerspectiveCamera::fov(float fov) {
	//if (fov > 0 || fov < M_PI) {
	_fov = fov;
	constructProjection();
	//}
}

float PerspectiveCamera::aspectRatio() const {
	return _aspectRatio;
}

void PerspectiveCamera::aspectRatio(float ratio) {
	// small optimization as Window::mainLoop() calls this every draw
	if (!utils::Equal(ratio, _aspectRatio, 0.001)) {
		_aspectRatio = ratio;
		constructProjection();
	}
}

void PerspectiveCamera::constructProjection() {
	_projection = glm::perspective(_fov,
								   _aspectRatio,
								   _zNear,
								   _zFar);
}