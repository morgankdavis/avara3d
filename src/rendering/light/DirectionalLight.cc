//
//  DirectionalLight.cc
//  avara3d
//
//  Created by Morgan Davis on 7/31/2024.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#include "a3d/rendering/light/DirectionalLight.h"


using namespace a3d;
using namespace glm;
using namespace std;


/*********************************************************************************************
	Public Member Functions
 *********************************************************************************************/

const vec3& DirectionalLight::direction() const {
	return _direction;
}

void DirectionalLight::direction(const vec3& direction) {
	_direction = direction;
}
