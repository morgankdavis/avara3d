//
//  Line.cc
//	avara-engine
//
//  Created by Morgan Davis on 5/20/18.
//  Copyright © 2018 Morgan K Davis. All rights reserved.
//

#include "Line.h"

#include "Color.h"


using namespace ae;
using namespace glm;
using namespace std;


/***************************************************************************************
     Lifecycle
 ***************************************************************************************/

Line::Line(vec3 fromLocation, vec3 toLocation):
	Line(fromLocation, toLocation, Color::White()) {
	
}

Line::Line(vec3 fromLocation, vec3 toLocation,
		   shared_ptr<Color> color):
	Line(fromLocation, toLocation, color, color) {
	
}

Line::Line(vec3 fromLocation, vec3 toLocation,
	shared_ptr<Color> fromColor, shared_ptr<Color> toColor):
	m_fromLocation(fromLocation),
	m_toLocation(toLocation),
	m_fromColor(fromColor),
	m_toColor(toColor) {
	
}

/***************************************************************************************
     Public
 ***************************************************************************************/

vec3 Line::fromLocation() const {
	return m_fromLocation;
}

void Line::fromLocation(vec3 point) {
	m_fromLocation = point;
}

vec3 Line::toLocation() const {
	return m_toLocation;
}

void Line::toLocation(vec3 point) {
	m_toLocation = point;
}

shared_ptr<Color> Line::fromColor() const {
	return m_fromColor;
}

void Line::fromColor(shared_ptr<Color> color) {
	m_fromColor = color;
}

shared_ptr<Color> Line::toColor() const {
	return m_toColor;
}

void Line::toColor(shared_ptr<Color> color) {
	m_toColor = color;
}
