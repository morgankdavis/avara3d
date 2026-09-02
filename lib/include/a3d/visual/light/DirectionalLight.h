//
//  DirectionalLight.h
//  avara3d
//
//  Created by Morgan Davis on 7/31/2024.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_VISUAL_LIGHT_DIRECTIONALLIGHT_H
#define AVARA3D_VISUAL_LIGHT_DIRECTIONALLIGHT_H

#include <string>

#include "a3d/Color.h"
#include "a3d/visual/light/Light.h"

namespace a3d {
class Node;

/**
 * @brief Infinitely distant light with uniform direction and no distance attenuation.
 *
 * The light direction follows the containing Node's world-forward direction.
 */
class DirectionalLight : public Light {

public:
    // [Public Lifecycle Functions]

    /** @brief Creates an unnamed white DirectionalLight. */
    DirectionalLight();

    /** @brief Creates a named white DirectionalLight. */
    explicit DirectionalLight(const std::string& name);

    /** @brief Creates an unnamed DirectionalLight with @p color. */
    explicit DirectionalLight(const Color& color);

    /** @brief Creates a DirectionalLight with @p name and @p color. */
    DirectionalLight(const std::string& name, const Color& color);

    DirectionalLight(const DirectionalLight&)            = default;
    DirectionalLight& operator=(const DirectionalLight&) = default;

    DirectionalLight(DirectionalLight&&) noexcept            = default;
    DirectionalLight& operator=(DirectionalLight&&) noexcept = default;

    // [Public Member Functions]

    /** @brief Returns the light intensity multiplier. */
    float             intensity() const;

    /** @brief Sets the light intensity multiplier; 1 is the default intensity. */
    void              intensity(float intensity);

private:
    // [Private Member Variables]

    float _intensity;
};
} // namespace a3d

#endif // AVARA3D_VISUAL_LIGHT_DIRECTIONALLIGHT_H
