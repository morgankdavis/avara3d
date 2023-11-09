//
//  Point.cc
//	avara-engine
//
//  Created by Morgan Davis on 5/20/18.
//  Copyright © 2018 Morgan K Davis. All rights reserved.
//

#include "geometry/Point.h"

#include "utilities/Color.h"


using namespace ae;
using namespace glm;
using namespace std;


/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

Point::Point(vec3 location):
	Point(location, Color::White()) {
	
}

Point::Point(vec3 location, shared_ptr<Color> color):
	_location(location),
	_color(color) {
	
}

/*********************************************************************************************
	Public
 *********************************************************************************************/

vec3 Point::location() const {
	return _location;
}

void Point::location(vec3 point) {
	_location = point;
}

shared_ptr<Color> Point::color() const {
	return _color;
}

void Point::color(shared_ptr<Color> color) {
	_color = color;
}
