//
//  Ground.h
//  avara3d
//
//  Created by Morgan Davis on 8/9/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_VISUAL_GROUND_H
#define AVARA3D_VISUAL_GROUND_H

#include "a3d/Color.h"

#include <optional>

#include "a3d/Math.h"

namespace a3d {

    struct Ground {

        struct Grid {

            Color color {};
            float spacing {1.0f};
            float lineWidthPixels {1.0f};
            float reliefStrength {0.0f};
        };

        struct RadialFade {

            Color      color {Color::Black()};
            math::vec2 center {0.0f, 0.0f};
            float      startDistance {10.0f};
            float      endDistance {100.0f};
        };

        struct HorizonHaze {

            Color color {};
            float angularWidthDegrees {3.0f};
        };

        Color                      color {};
        std::optional<Grid>        minorGrid {};
        std::optional<Grid>        majorGrid {};
        std::optional<RadialFade>  radialFade {};
        std::optional<HorizonHaze> horizonHaze {};
        float                      specularIntensity {0.15f};
        float                      specularExponent {32.0f};
    };

}

#endif //AVARA3D_VISUAL_GROUND_H
