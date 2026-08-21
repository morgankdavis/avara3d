//
//  Background.h
//  avara3d
//
//  Created by Morgan Davis on 8/9/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_VISUAL_BACKGROUND_H
#define AVARA3D_VISUAL_BACKGROUND_H

#include "a3d/Math.h"
#include "a3d/visual/material/Material.h"

namespace a3d {

    struct Background {

        explicit Background(Material::Property contents);

        Material::Property contents {};
        math::quat         orientation {1.0f};
    };

}

#endif // AVARA3D_VISUAL_BACKGROUND_H
