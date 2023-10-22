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
	_extent(Extent{vec3{-1, -1, -1}, vec3{1, 1, 1}}) {

}

OrthographicCamera::OrthographicCamera(Extent extent):
		OrthographicCamera(nullopt, extent) {

}

OrthographicCamera::OrthographicCamera(optional<string> name, Extent extent):
	Camera(name),
	_extent(extent) {

}

Extent OrthographicCamera::extent() const {
	return _extent;
}

void OrthographicCamera::extent(const Extent& e) {
	_extent = e;
}

void OrthographicCamera::constructProjectionMatrix() {
	_projection = glm::ortho(_extent.min.x,
							 _extent.max.x,
							 _extent.max.y,
							 _extent.min.y,
							 _extent.min.z,
							 _extent.max.z);
}
