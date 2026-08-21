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

}

namespace a3d::util::geom {

    // [Public Functions]

    /**
     * @brief Returns a uniform transform that centers @p source at the origin and fits it inside @p targetSize.
     *
     * @throws std::invalid_argument if the source bounds are invalid or non-finite, the target size is not
     * positive and finite, or the source has zero extent on every axis.
     */
    math::mat4 fit_inside(const AABB& source, const math::vec3& targetSize);

}

#endif // AVARA3D_UTIL_GEOMETRY_H
