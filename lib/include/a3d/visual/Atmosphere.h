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

    struct Atmosphere {

        struct Haze {

            Color color {};
            float density {0.02f};
        };

        struct LimbGlow {

            Color color {};
            float intensity {1.0f};
        };

        float                   scaleHeight {5.0f};
        std::optional<Haze>     haze {};
        std::optional<LimbGlow> limbGlow {};
    };

}

#endif // AVARA3D_VISUAL_ATMOSPHERE_H
