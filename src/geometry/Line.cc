//
//  Line.cc
//	avara-engine
//
//  Created by Morgan Davis on 5/20/18.
//  Copyright © 2018 Morgan K Davis. All rights reserved.
//

#include "ae/geometry/Line.h"

#include "ae/Color.h"


using namespace ae;
using namespace glm;
using namespace std;


/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

Line::Line(vec3 fromLocation, vec3 toLocation):
	Line(fromLocation, toLocation, Color::White()) {
	
}

Line::Line(vec3 fromLocation, vec3 toLocation,
		   shared_ptr<Color> color):
	Line(fromLocation, toLocation, color, color) {
	
}

Line::Line(vec3 fromLocation, vec3 toLocation,
	shared_ptr<Color> fromColor, shared_ptr<Color> toColor):
	_fromLocation(fromLocation),
	_toLocation(toLocation),
	_fromColor(fromColor),
	_toColor(toColor) {
	
}

/*********************************************************************************************
	Public
 *********************************************************************************************/

vec3 Line::fromLocation() const {
	return _fromLocation;
}

void Line::fromLocation(vec3 point) {
	_fromLocation = point;
}

vec3 Line::toLocation() const {
	return _toLocation;
}

void Line::toLocation(vec3 point) {
	_toLocation = point;
}

shared_ptr<Color> Line::fromColor() const {
	return _fromColor;
}

void Line::fromColor(shared_ptr<Color> color) {
	_fromColor = color;
}

shared_ptr<Color> Line::toColor() const {
	return _toColor;
}

void Line::toColor(shared_ptr<Color> color) {
	_toColor = color;
}
