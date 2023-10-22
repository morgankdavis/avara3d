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
	_extent(Extent3D{vec3{-1, -1, -1}, vec3{1, 1, 1}}) {

}

OrthographicCamera::OrthographicCamera(std::optional<std::string> name,
									   float zNear, float zFar,
									   ae::Extent3D extent):
	Camera(name, zNear, zFar),
	_extent(extent) {

}

Extent3D OrthographicCamera::extent() const {
	return _extent;
}

void OrthographicCamera::extent(const ae::Extent3D& e) {
	_extent = e;
}

void OrthographicCamera::constructProjection() {
//	_projection = glm::ortho(_bounds.left,
//							 _bounds.right,
//							 _bounds.bottom,
//							 _bounds.top);

	_projection = glm::ortho(_extent.min.x,
							 _extent.min.y,
							 _extent.min.z,
							 _extent.max.x,
							 _extent.max.y,
							 _extent.max.z);
}
