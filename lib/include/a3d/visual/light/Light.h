//
//  Light.h
//  avara3d
//
//  Created by Morgan Davis on 10/21/16.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
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

    class Light {

    public:
        /// Public Static Member Functions ///

        static std::shared_ptr<AmbientLight>     Ambient();
        static std::shared_ptr<AmbientLight>     Ambient(Color color);

        static std::shared_ptr<DirectionalLight> Directional();
        static std::shared_ptr<DirectionalLight> Directional(Color color);

        static std::shared_ptr<PointLight>       Point();
        static std::shared_ptr<PointLight>       Point(Color color);

        static std::shared_ptr<SpotLight>        Spot();
        static std::shared_ptr<SpotLight>        Spot(Color color);

    protected:
        /// Protected Lifecycle Functions ///

        Light();
        explicit Light(const std::string& name);
        explicit Light(Color color);
        Light(const std::string& name, Color color);

        Light(const Light&)            = default;
        Light& operator=(const Light&) = default;

        Light(Light&&) noexcept            = default;
        Light& operator=(Light&&) noexcept = default;

        virtual ~Light() = 0;

    public:
        /// Public Member Functions ///

        const std::optional<std::string>& name() const;
        void                              name(const std::string& name);

        const Color&                      color() const;
        void                              color(Color color);

    protected:
        /// Protected Member Variables ///

        std::optional<std::string> _name;
        Color                      _color;
    };

}

#endif //AVARA3D_VISUAL_LIGHT_LIGHT_H */
