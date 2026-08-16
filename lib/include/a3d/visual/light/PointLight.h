//
//  PointLight.h
//  avara3d
//
//  Created by Morgan Davis on 7/31/2024.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_VISUAL_LIGHT_POINTLIGHT_H
#define AVARA3D_VISUAL_LIGHT_POINTLIGHT_H

#include <string>

#include "a3d/Color.h"
#include "a3d/visual/light/Attenuation.h"
#include "a3d/visual/light/Light.h"

namespace a3d {

    class PointLight : public Light {

    public:
        /// Public Lifecycle Functions ///

        PointLight();
        explicit PointLight(const std::string& name);
        explicit PointLight(Color color);
        PointLight(const std::string& name, Color color);

        PointLight(const PointLight&)            = default;
        PointLight& operator=(const PointLight&) = default;

        PointLight(PointLight&&) noexcept                   = default;
        PointLight&        operator=(PointLight&&) noexcept = default;

        /// Public Member Functions ///

        const Attenuation& attenuation() const;
        void               attenuation(const Attenuation& attenuation);

    private:
        /// Private Member Variables ///

        Attenuation _attenuation;
    };

}

#endif //AVARA3D_VISUAL_LIGHT_POINTLIGHT_H */
