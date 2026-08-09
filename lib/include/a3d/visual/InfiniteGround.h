//
//  InfiniteGround.h
//  avara3d
//
//  Created by Morgan Davis on 8/9/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_VISUAL_INFINITEGROUND_H
#define AVARA3D_VISUAL_INFINITEGROUND_H

#include <memory>

namespace a3d {

    class Color;

    struct InfiniteGround {

        std::shared_ptr<Color> color {};
        float                  height {0.0f};

        float                  specularIntensity {0.15f};
        float                  specularExponent {32.0f};
    };

}

#endif //AVARA3D_VISUAL_INFINITEGROUND_H
