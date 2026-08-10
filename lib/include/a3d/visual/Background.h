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

    class Background {

    public:
        /// Public Lifecycle Functions ///

        explicit Background(const Material::Property& contents);

        /// Public Member Functions ///

        const Material::Property& contents() const;

        math::quat                orientation() const;
        void                      orientation(const math::quat& orientation);

    private:
        /// Private Member Variables ///

        Material::Property _contents;
        math::quat         _orientation;
    };

}

#endif //AVARA3D_VISUAL_BACKGROUND_H
