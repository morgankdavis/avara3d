//
//  Point.cc
//	avara-engine
//
//  Created by Morgan Davis on 5/20/18.
//  Copyright © 2018 Morgan K Davis. All rights reserved.
//

#include "Point.h"

#include "Color.h"


using namespace ae;
using namespace glm;
using namespace std;


/***************************************************************************************
     Lifecycle
 ***************************************************************************************/

Point::Point(vec3 location):
	Point(location, Color::White()) {
	
}

Point::Point(vec3 location, shared_ptr<Color> color):
	m_location(location),
	m_color(color) {
	
}

/***************************************************************************************
     Public
 ***************************************************************************************/

vec3 Point::location() const {
	return m_location;
}

void Point::location(vec3 point) {
	m_location = point;
}

shared_ptr<Color> Point::color() const {
	return m_color;
}

void Point::color(shared_ptr<Color> color) {
	m_color = color;
}
