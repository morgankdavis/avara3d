//
//  OrthographicCamera.cc
//  avara3d
//
//  Created by Morgan Davis on 10/22/23.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#include "a3d/visual/camera/OrthographicCamera.h"

#include "a3d/log/Log.h"

using namespace a3d;
using namespace std;
using namespace a3d::math;

/// Public Lifecycle Functions ///

OrthographicCamera::OrthographicCamera():
	Camera{},
	_extent{{-1, -1, -1}, {1, 1, 1}} {
	//constructProjectionMatrix();
}

OrthographicCamera::OrthographicCamera(const AABB& extent):
		Camera{},
		_extent{extent} {
	//constructProjectionMatrix();
}

OrthographicCamera::OrthographicCamera(const string& name, const AABB& extent):
	Camera{name},
	_extent{extent} {
	//constructProjectionMatrix();
}

OrthographicCamera::~OrthographicCamera() {

	if (_name != nullopt) {
		log::d()("Destroying OrthographicCamera '{}' ({:p})", *_name, static_cast<void*>(this));
	}
	else {
		log::d()("Destroying OrthographicCamera {:p}", static_cast<void*>(this));
	}
}

/// Public Member Functions ///

AABB OrthographicCamera::extent() const {
	return _extent;
}

void OrthographicCamera::extent(const AABB& e) {
	_extent = e;
	//constructProjectionMatrix();
}

/// Camera Internal Member Functions ///

mat4 OrthographicCamera::projection() const {
	return ortho(_extent.min.x,
				 _extent.max.x,
				 _extent.min.y,
				 _extent.max.y,
				 _extent.min.z,
				 _extent.max.z);
}

/// Camera Protected Member Functions ///

//void OrthographicCamera::constructProjectionMatrix() {
//
//	// left, right, bottom, top, near, far
//	// DeVries 9.5.1
//
//	_projection = ortho(_extent.min.x,
//						_extent.max.x,
//						_extent.min.y,
//						_extent.max.y,
//						_extent.min.z,
//						_extent.max.z);
//}
