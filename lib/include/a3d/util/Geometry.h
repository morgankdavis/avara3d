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
#include "a3d/mesh/AABB.h"

namespace a3d::util::geom {

    math::mat4 fit_inside(const AABB& source, const math::vec3& targetSize);

}

#endif //AVARA3D_UTIL_GEOMETRY_H
