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

namespace a3d::surface {

    struct Plane {

        float height {0.0f};
    };

    struct Sphere {

        math::vec3 center {0.0f, 0.0f, 0.0f};
        float      radius {1.0f};
    };

    using Surface = std::variant<Plane, Sphere>;

}

#endif //AVARA3D_VISUAL_SURFACE_H
