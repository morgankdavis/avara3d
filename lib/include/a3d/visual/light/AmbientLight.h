//
//  AmbientLight.h
//  avara3d
//
//  Created by Morgan Davis on 7/31/2024.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_VISUAL_LIGHT_AMBIENTLIGHT_H
#define AVARA3D_VISUAL_LIGHT_AMBIENTLIGHT_H

#include <string>

#include "a3d/Color.h"
#include "a3d/visual/light/Light.h"

namespace a3d {

    class AmbientLight : public Light {

    public:
        /// Public Lifecycle Functions ///

        AmbientLight();
        explicit AmbientLight(const std::string& name);
        explicit AmbientLight(Color color);
        AmbientLight(const std::string& name, Color color);

        AmbientLight(const AmbientLight&)            = default;
        AmbientLight& operator=(const AmbientLight&) = default;

        AmbientLight(AmbientLight&&) noexcept            = default;
        AmbientLight& operator=(AmbientLight&&) noexcept = default;
    };

}

#endif //AVARA3D_VISUAL_LIGHT_AMBIENTLIGHT_H */
