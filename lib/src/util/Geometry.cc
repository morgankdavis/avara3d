//
//  Geometry.cc
//  avara3d
//
//  Created by Morgan Davis on 8/18/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#include "a3d/util/Geometry.h"

#include <stdexcept>

#include "a3d/mesh/AABB.h"

using namespace a3d::math;
using namespace std;

namespace a3d::util::geom {

mat4 fit_inside(const AABB& source, const vec3& targetSize) {

    if (!source.valid()) {
        throw invalid_argument("Source bounds must be valid.");
    }

    if (targetSize.x <= 0.0f || targetSize.y <= 0.0f || targetSize.z <= 0.0f) {
        throw invalid_argument("Target size must be greater than zero.");
    }

    const vec3 sourceSize = source.max - source.min;

    if (sourceSize.x == 0.0f && sourceSize.y == 0.0f && sourceSize.z == 0.0f) {
        throw invalid_argument("Source bounds must be non-zero.");
    }

    const vec3 scaleFactors {
        sourceSize.x > 0.0f ? targetSize.x / sourceSize.x : F32_INFINITY,
        sourceSize.y > 0.0f ? targetSize.y / sourceSize.y : F32_INFINITY,
        sourceSize.z > 0.0f ? targetSize.z / sourceSize.z : F32_INFINITY,
    };

    const float scaleFactor = math::min(scaleFactors);
    const vec3  center = AABB::Center(source);

    return math::scale(mat4(1.0f), scaleFactor) * math::translate(mat4(1.0f), -center);
}

} // namespace a3d::util::geom
