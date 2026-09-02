//
//  Surface.h
//  avara3d
//
//  Created by Morgan Davis on 8/16/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_VISUAL_SURFACE_H
#define AVARA3D_VISUAL_SURFACE_H

#include <variant>

#include "a3d/Math.h"

namespace a3d {

// [Public Types]

    /** @brief Infinite planar reference surface used by ground and atmospheric effects. */
struct PlaneSurface {

    // [Public Member Variables]

    float height {0.0f}; ///< Height of the reference plane in scene units.
};

    /** @brief Spherical reference surface used by ground and atmospheric effects. */
struct SphereSurface {

    // [Public Member Variables]

    math::vec3 center {0.0f, 0.0f, 0.0f}; ///< Center of the reference sphere in scene coordinates.
    float      radius {1.0f}; ///< Radius of the reference sphere in scene units; must be greater than zero.
};

    /** @brief Planar or spherical reference surface used by VisualWorld environment effects. */
using Surface = std::variant<PlaneSurface, SphereSurface>;

}

#endif // AVARA3D_VISUAL_SURFACE_H
