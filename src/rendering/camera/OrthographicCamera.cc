//
// Created by mkd on 10/22/23.
//

#include "a3d/rendering/camera/OrthographicCamera.h"

#include "glm/gtc/matrix_transform.hpp"

#include "a3d/Utilities.h"


using namespace a3d;
using namespace std;
using namespace glm;

OrthographicCamera::OrthographicCamera():
	Camera{},
	_extent{{-1, -1, -1}, {1, 1, 1}} {}

OrthographicCamera::OrthographicCamera(AABB extent):
		Camera{},
		_extent{extent} {}

OrthographicCamera::OrthographicCamera(const string& name, AABB extent):
	Camera{name},
	_extent{extent} {}

AABB OrthographicCamera::extent() const {
	return _extent;
}

void OrthographicCamera::extent(const AABB& e) {
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
