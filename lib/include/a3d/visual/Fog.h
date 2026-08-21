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

    // [Public Types]

    /** @brief Configures distance-based fog for a VisualWorld. */
    struct Fog {

        // [Public Member Variables]

        Color color {};                       ///< Fog color.
        float startDistance {0.0f};            ///< Distance where fog begins, in scene units; must be non-negative.
        float endDistance {1000.0f};           ///< Distance where fog reaches full strength, in scene units; must be greater than startDistance.
        /**
         * @brief Exponent applied to the normalized fog-distance transition.
         *
         * For positive values, the fog amount is
         * pow(clamp((d - startDistance) / (endDistance - startDistance), 0, 1),
         *     transitionExponent),
         * where d is the distance from the camera in scene units.
         *
         * A value of 1 produces a linear transition; larger values delay the transition
         * toward endDistance. A value of 0 applies a constant fog amount given by color alpha.
         */
        float transitionExponent {1.0f};
    };

}

#endif // AVARA3D_VISUAL_FOG_H
