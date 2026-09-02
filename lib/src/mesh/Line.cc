//
//  Line.cc
//  avara3d
//
//  Created by Morgan Davis on 5/20/18.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#include "a3d/mesh/Line.h"

#include "a3d/Color.h"

using namespace a3d::math;
using namespace std;

namespace a3d {
// [Internal Lifecycle Functions]

Line::Line(const vec3& fromLocation, const vec3& toLocation):
    Line {fromLocation, toLocation, Color {1.0f}} {}

Line::Line(const vec3& fromLocation, const vec3& toLocation, const Color& color):
    Line {fromLocation, toLocation, color, color} {}

Line::Line(const vec3& fromLocation, const vec3& toLocation, const vec3& fromColor, const vec3& toColor):
    _fromLocation {fromLocation},
    _toLocation {toLocation},
    _fromColor {Color(fromColor)},
    _toColor {Color(toColor)} {}

Line::Line(const vec3& fromLocation, const vec3& toLocation, const Color& fromColor, const Color& toColor):
    _fromLocation {fromLocation},
    _toLocation {toLocation},
    _fromColor {fromColor},
    _toColor {toColor} {}

// [Internal Member Functions]

const vec3& Line::fromLocation() const {
    return _fromLocation;
}

void Line::fromLocation(const vec3& point) {
    _fromLocation = point;
}

const vec3& Line::toLocation() const {
    return _toLocation;
}

void Line::toLocation(const vec3& point) {
    _toLocation = point;
}

const Color& Line::fromColor() const {
    return _fromColor;
}

void Line::fromColor(const Color& color) {
    _fromColor = color;
}

const Color& Line::toColor() const {
    return _toColor;
}

void Line::toColor(const Color& color) {
    _toColor = color;
}
} // namespace a3d
