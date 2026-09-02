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
/** @brief Configures a solid-color or cubemap background for a VisualWorld. */
struct Background {

    // [Public Lifecycle Functions]

    /** @brief Creates a background with @p contents. */
    explicit Background(Material::Property contents);

    // [Public Member Variables]

    Material::Property
        contents {}; ///< Background contents; VisualWorld accepts a Color or a Texture containing a CubeImage.
    math::quat orientation {1.0f}; ///< Orientation applied when sampling a cubemap background.
};
} // namespace a3d

#endif // AVARA3D_VISUAL_BACKGROUND_H
