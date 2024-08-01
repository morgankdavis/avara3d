//
//  SpotLight.cc
//  avara3d
//
//  Created by Morgan Davis on 7/31/2024.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#include "a3d/rendering/light/SpotLight.h"


using namespace a3d;
using namespace std;


/*********************************************************************************************
	Public Member Functions
 *********************************************************************************************/

float SpotLight::innerAngle() const {
	return _innerAngle;
}

void SpotLight::innerAngle(float angle) {
	_innerAngle = angle;
}

float SpotLight::outerAngle() const {
	return _outerAngle;
}

void SpotLight::outerAngle(float angle) {
	_outerAngle = angle;
}
