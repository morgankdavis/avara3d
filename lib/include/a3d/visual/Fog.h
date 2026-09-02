//
//  Fog.h
//  avara3d
//
//  Created by Morgan Davis on 8/9/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_VISUAL_FOG_H
#define AVARA3D_VISUAL_FOG_H

#include "a3d/Color.h"

namespace a3d {

/** @brief Configures distance-based fog for a VisualWorld. */
struct Fog {

    // [Public Member Variables]

    Color color {};                       ///< Fog color.
    float startDistance {0.0f}; ///< Distance where fog begins, in scene units; must be non-negative.
    float endDistance {
        1000.0f}; ///< Distance where fog reaches full strength, in scene units; must be greater than startDistance.
    /** @brief Fog transition exponent; must be non-negative. 0 uses color alpha, 1 is linear, and values above/below 1 shift the transition later/earlier. */
    float transitionExponent {1.0f};
};

} // namespace a3d

#endif // AVARA3D_VISUAL_FOG_H
