//
//  AtmosphericHaze.h
//  avara3d
//
//  Created by Morgan Davis on 8/11/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_VISUAL_ATMOSPHERICHAZE_H
#define AVARA3D_VISUAL_ATMOSPHERICHAZE_H

#include "a3d/Color.h"

namespace a3d {

    struct AtmosphericHaze {

        Color color {};
        float baseHeight {0.0f};
        float density {0.02f};
        float heightFalloff {0.25f};
    };

}

#endif //AVARA3D_VISUAL_ATMOSPHERICHAZE_H
