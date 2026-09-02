//
//  Point.cc
//  avara3d
//
//  Created by Morgan Davis on 5/20/18.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#include "a3d/mesh/Point.h"

#include "a3d/Color.h"
#include "a3d/Math.h"

using namespace a3d::math;
using namespace std;

namespace a3d {
// [Internal Lifecycle Functions]

Point::Point(const vec3& location):
    Point {location, Color {1.0f}} {}

Point::Point(const vec3& location, const Color& color):
    _location {location},
    _color {color} {}

// [Internal Member Functions]

const vec3& Point::location() const {
    return _location;
}

void Point::location(const vec3& point) {
    _location = point;
}

const Color& Point::color() const {
    return _color;
}

void Point::color(const Color& color) {
    _color = color;
}
} // namespace a3d
