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

Line::Line(const Line& other) {

}

Line::Line(Line&& other) {

}

Line& Line::operator=(const Line& other) {

}

Line& Line::operator=(Line&& other) noexcept {

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
