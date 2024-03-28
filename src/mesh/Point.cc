//
//  Point.cc
//	avara3d
//
//  Created by Morgan Davis on 5/20/18.
//  Copyright © 2018 Morgan K Davis. All rights reserved.
//

#include "a3d/mesh/Point.h"

#include "a3d/Color.h"


using namespace a3d;
using namespace glm;
using namespace std;


/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

Point::Point(const vec3& location):
	Point{location, Color::White()} {
	
}

Point::Point(const vec3& location, const shared_ptr<Color>& color):
	_location{location},
	_color{color} {
	
}

/*********************************************************************************************
	Public
 *********************************************************************************************/

const vec3& Point::location() const {
	return _location;
}

void Point::location(const vec3& point) {
	_location = point;
}

const shared_ptr<Color>& Point::color() const {
	return _color;
}

void Point::color(const shared_ptr<Color>& color) {
	_color = color;
}
