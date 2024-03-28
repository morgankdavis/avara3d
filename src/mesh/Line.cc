//
//  Line.cc
//	avara3d
//
//  Created by Morgan Davis on 5/20/18.
//  Copyright © 2018 Morgan K Davis. All rights reserved.
//

#include "a3d/mesh/Line.h"

#include "a3d/Color.h"


using namespace a3d;
using namespace glm;
using namespace std;


/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

Line::Line(const vec3& fromLocation,
		   const vec3& toLocation):
		Line{fromLocation, toLocation, Color::White()} { }

Line::Line(const vec3& fromLocation, const vec3& toLocation,
		   const shared_ptr<Color>& color):
		Line{fromLocation, color, toLocation, color} { }

Line::Line(const vec3& fromLocation,
		   const shared_ptr<Color>& fromColor,
		   const vec3& toLocation,
		   const shared_ptr<Color>& toColor):
		_fromLocation{fromLocation},
		_toLocation{toLocation},
		_fromColor{fromColor},
		_toColor{toColor} { }

/*********************************************************************************************
	Public
 *********************************************************************************************/

const vec3& Line::fromLocation() const {
	return _fromLocation;
}

void Line::fromLocation(const vec3& point) {
	_fromLocation = point;
}

const shared_ptr<Color>& Line::fromColor() const {
	return _fromColor;
}

void Line::fromColor(const shared_ptr<Color>& color) {
	_fromColor = color;
}

const vec3& Line::toLocation() const {
	return _toLocation;
}

void Line::toLocation(const vec3& point) {
	_toLocation = point;
}

const shared_ptr<Color>& Line::toColor() const {
	return _toColor;
}

void Line::toColor(const shared_ptr<Color>& color) {
	_toColor = color;
}
