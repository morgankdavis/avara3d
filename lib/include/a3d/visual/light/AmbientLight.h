//
//  AmbientLight.h
//  avara3d
//
//  Created by Morgan Davis on 7/31/2024.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_VISUAL_LIGHT_AMBIENTLIGHT_H
#define AVARA3D_VISUAL_LIGHT_AMBIENTLIGHT_H

#include <string>

#include "a3d/Color.h"
#include "a3d/visual/light/Light.h"

namespace a3d {
/** @brief Uniform light that contributes independent of position, direction, or distance. */
class AmbientLight : public Light {

public:
    // [Public Lifecycle Functions]

    /** @brief Creates an unnamed white AmbientLight. */
    AmbientLight();

    /** @brief Creates a named white AmbientLight. */
    explicit AmbientLight(const std::string& name);

    /** @brief Creates an unnamed AmbientLight with @p color. */
    explicit AmbientLight(const Color& color);

    /** @brief Creates an AmbientLight with @p name and @p color. */
    AmbientLight(const std::string& name, const Color& color);

    AmbientLight(const AmbientLight&)            = default;
    AmbientLight& operator=(const AmbientLight&) = default;

    AmbientLight(AmbientLight&&) noexcept            = default;
    AmbientLight& operator=(AmbientLight&&) noexcept = default;
};
} // namespace a3d

#endif // AVARA3D_VISUAL_LIGHT_AMBIENTLIGHT_H
