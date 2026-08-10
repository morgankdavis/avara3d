//
//  Fog.h
//  avara3d
//
//  Created by Morgan Davis on 8/9/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_VISUAL_FOG_H
#define AVARA3D_VISUAL_FOG_H

#include <memory>

namespace a3d {

    class Color;

    struct Fog {

        std::shared_ptr<Color> color {};
        float                  startDistance {0.0f};
        float                  endDistance {1000.0f};
        float                  transitionExponent {1.0f};
    };

}

#endif //AVARA3D_VISUAL_FOG_H
