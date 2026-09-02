//
//  SpotLight.h
//  avara3d
//
//  Created by Morgan Davis on 7/31/2024.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_VISUAL_LIGHT_SPOTLIGHT_H
#define AVARA3D_VISUAL_LIGHT_SPOTLIGHT_H

#include <cstdint>
#include <string>

#include "a3d/Color.h"
#include "a3d/visual/light/Attenuation.h"
#include "a3d/visual/light/Light.h"

namespace a3d {
/**
 * @brief Cone-shaped light located and oriented by its containing Node.
 *
 * The light originates at the Node's world position and points along the Node's
 * world-forward direction. Light contribution is reduced by both angular
 * feathering and distance attenuation.
 */
class SpotLight : public Light {

public:
    // [Public Types]

    /** @brief Selects the angular falloff curve between the outer and inner cone angles. */
    enum class FeatheringMode : uint8_t {
        Linear = 0, ///< Uses a linear angular falloff transition.
        Sharp  = 1, ///< Uses y = x(2-x).
        Soft   = 2  ///< Uses y = x^2.
    };

    // [Public Lifecycle Functions]

    /** @brief Creates an unnamed white SpotLight. */
    SpotLight();

    /** @brief Creates a named white SpotLight. */
    explicit SpotLight(const std::string& name);

    /** @brief Creates an unnamed SpotLight with @p color. */
    explicit SpotLight(const Color& color);

    /** @brief Creates a SpotLight with @p name and @p color. */
    SpotLight(const std::string& name, const Color& color);

    SpotLight(const SpotLight&)            = default;
    SpotLight& operator=(const SpotLight&) = default;

    SpotLight(SpotLight&&) noexcept            = default;
    SpotLight& operator=(SpotLight&&) noexcept = default;

public:
    // [Public Member Functions]

    /** @brief Returns the inner cone half-angle in radians. */
    float              innerAngle() const;

    /** @brief Sets the inner cone half-angle in radians. */
    void               innerAngle(float angle);

    /** @brief Returns the outer cone half-angle in radians. */
    float              outerAngle() const;

    /** @brief Sets the outer cone half-angle in radians. */
    void               outerAngle(float angle);

    /** @brief Returns the angular feathering curve. */
    FeatheringMode     featheringMode() const;

    /** @brief Sets the angular feathering curve. */
    void               featheringMode(FeatheringMode mode);

    /** @brief Returns the light intensity multiplier. */
    float              intensity() const;

    /** @brief Sets the light intensity multiplier; 1 is the default intensity. */
    void               intensity(float intensity);

    /** @brief Returns the distance-attenuation coefficients. */
    const Attenuation& attenuation() const;

    /** @brief Sets the distance-attenuation coefficients. */
    void               attenuation(const Attenuation& attenuation);

    // [Internal Member Functions]

    float              innerAngleCos() const;
    float              outerAngleCos() const;

private:
    // [Private Member Variables]

    float          _innerAngleCos;
    float          _outerAngleCos;
    FeatheringMode _featherMode;
    float          _intensity;
    Attenuation    _attenuation;
};
} // namespace a3d

#endif // AVARA3D_VISUAL_LIGHT_SPOTLIGHT_H
