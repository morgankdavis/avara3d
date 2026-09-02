//
//  Geometry.h
//  avara3d
//
//  Created by Morgan Davis on 8/18/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_UTIL_GEOMETRY_H
#define AVARA3D_UTIL_GEOMETRY_H

#include "a3d/Math.h"

namespace a3d {

    struct AABB;

} // namespace a3d

namespace a3d::util::geom {

    // [Public Functions]

    /**
     * @brief Returns a uniform transform that centers @p source at the origin and fits it inside @p targetSize.
     *
     * @throws std::invalid_argument if the source bounds or target size are invalid.
     */
    math::mat4 fit_inside(const AABB& source, const math::vec3& targetSize);

} // namespace a3d::util::geom

#endif // AVARA3D_UTIL_GEOMETRY_H
