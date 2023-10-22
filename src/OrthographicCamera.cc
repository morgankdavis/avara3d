//
// Created by mkd on 10/22/23.
//

#include "OrthographicCamera.h"

#include <glm/gtc/matrix_transform.hpp>

#include "Utilities.h"


using namespace ae;
using namespace std;
using namespace glm;

OrthographicCamera::OrthographicCamera():
	Camera(),
	_bounds({-1, 1, 1, -1}) {

}

OrthographicCamera::OrthographicCamera(std::optional<std::string> name,
									   float zNear, float zFar,
									   ae::Bounds bounds):
	Camera(name, zNear, zFar),
	_bounds(bounds) {

}

Bounds OrthographicCamera::bounds() const {
	return _bounds;
}

void OrthographicCamera::bounds(const ae::Bounds& b) {
	_bounds = b;
}

void OrthographicCamera::constructProjection() {
	_projection = glm::ortho(_bounds.left,
							 _bounds.right,
							 _bounds.bottom,
							 _bounds.top);
}
