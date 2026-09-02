//
//  Light.h
//  avara3d
//
//  Created by Morgan Davis on 10/21/16.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_VISUAL_LIGHT_LIGHT_H
#define AVARA3D_VISUAL_LIGHT_LIGHT_H

#include "a3d/Color.h"

#include <memory>
#include <optional>
#include <string>

namespace a3d {

class AmbientLight;
class DirectionalLight;
class PointLight;
class SpotLight;

/**
 * @brief Base class for light sources attached to Scene Nodes.
 *
 * Concrete lights use the containing Node transform where spatial information
 * is required. Directional and spot lights use the Node's world-forward
 * direction; point and spot lights use its world position.
 */
class Light {

public:
    // [Public Static Member Functions]

    /** @brief Creates a white AmbientLight. */
    static std::shared_ptr<AmbientLight>     Ambient();

    /** @brief Creates an AmbientLight with @p color. */
    static std::shared_ptr<AmbientLight>     Ambient(const Color& color);

    /** @brief Creates a white DirectionalLight. */
    static std::shared_ptr<DirectionalLight> Directional();

    /** @brief Creates a DirectionalLight with @p color. */
    static std::shared_ptr<DirectionalLight> Directional(const Color& color);

    /** @brief Creates a white PointLight. */
    static std::shared_ptr<PointLight>       Point();

    /** @brief Creates a PointLight with @p color. */
    static std::shared_ptr<PointLight>       Point(const Color& color);

    /** @brief Creates a white SpotLight. */
    static std::shared_ptr<SpotLight>        Spot();

    /** @brief Creates a SpotLight with @p color. */
    static std::shared_ptr<SpotLight>        Spot(const Color& color);

protected:
    // [Protected Lifecycle Functions]

    Light();
    explicit Light(const std::string& name);
    explicit Light(const Color& color);
    Light(const std::string& name, const Color& color);

    Light(const Light&)            = default;
    Light& operator=(const Light&) = default;

    Light(Light&&) noexcept            = default;
    Light& operator=(Light&&) noexcept = default;

    virtual ~Light() = 0;

public:
    // [Public Member Functions]

    /** @brief Returns the optional light name. */
    const std::optional<std::string>& name() const;

    /** @brief Sets the light name. */
    void                              name(const std::string& name);

    /** @brief Returns the light color. */
    const Color&                      color() const;

    /** @brief Sets the light color. */
    void                              color(const Color& color);

protected:
    // [Protected Member Variables]

    std::optional<std::string> _name;
    Color                      _color;
};

}

#endif // AVARA3D_VISUAL_LIGHT_LIGHT_H
