//
//  Atmosphere.h
//  avara3d
//
//  Created by Morgan Davis on 8/11/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_VISUAL_ATMOSPHERE_H
#define AVARA3D_VISUAL_ATMOSPHERE_H

#include <optional>

#include "a3d/Color.h"

namespace a3d {

    /** @brief Configures atmospheric effects around a VisualWorld reference Surface. */
struct Atmosphere {

    // [Public Types]

        /** @brief Configures atmospheric haze. */
    struct Haze {

        // [Public Member Variables]

        Color color {};           ///< Haze color.
        float density {0.02f};     ///< Haze density; must be non-negative.
    };

        /** @brief Configures glow along the limb of a spherical reference Surface. */
    struct LimbGlow {

        // [Public Member Variables]

        Color color {};           ///< Limb-glow color.
        float intensity {1.0f};    ///< Limb-glow intensity; must be non-negative.
    };

    // [Public Member Variables]

    float scaleHeight {5.0f}; ///< Scale controlling atmospheric density falloff; must be greater than zero.
    std::optional<Haze>     haze {}; ///< Optional atmospheric haze.
    std::optional<LimbGlow> limbGlow {}; ///< Optional limb glow; requires a spherical reference Surface.
};

}

#endif // AVARA3D_VISUAL_ATMOSPHERE_H
