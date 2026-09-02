//
//  PointLight.h
//  avara3d
//
//  Created by Morgan Davis on 7/31/2024.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_VISUAL_LIGHT_POINTLIGHT_H
#define AVARA3D_VISUAL_LIGHT_POINTLIGHT_H

#include <string>

#include "a3d/Color.h"
#include "a3d/visual/light/Attenuation.h"
#include "a3d/visual/light/Light.h"

namespace a3d {
/**
 * @brief Omnidirectional light located at its containing Node's world position.
 *
 * Light contribution is reduced with distance according to the configured
 * Attenuation coefficients.
 */
class PointLight : public Light {

public:
    // [Public Lifecycle Functions]

    /** @brief Creates an unnamed white PointLight. */
    PointLight();

    /** @brief Creates a named white PointLight. */
    explicit PointLight(const std::string& name);

    /** @brief Creates an unnamed PointLight with @p color. */
    explicit PointLight(const Color& color);

    /** @brief Creates a PointLight with @p name and @p color. */
    PointLight(const std::string& name, const Color& color);

    PointLight(const PointLight&)            = default;
    PointLight& operator=(const PointLight&) = default;

    PointLight(PointLight&&) noexcept                   = default;
    PointLight&        operator=(PointLight&&) noexcept = default;

    // [Public Member Functions]

    /** @brief Returns the light intensity multiplier. */
    float              intensity() const;

    /** @brief Sets the light intensity multiplier; 1 is the default intensity. */
    void               intensity(float intensity);

    /** @brief Returns the distance-attenuation coefficients. */
    const Attenuation& attenuation() const;

    /** @brief Sets the distance-attenuation coefficients. */
    void               attenuation(const Attenuation& attenuation);

private:
    // [Private Member Variables]

    float       _intensity;
    Attenuation _attenuation;
};
} // namespace a3d

#endif // AVARA3D_VISUAL_LIGHT_POINTLIGHT_H
