//
//  Ground.h
//  avara3d
//
//  Created by Morgan Davis on 8/9/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_VISUAL_GROUND_H
#define AVARA3D_VISUAL_GROUND_H

#include "a3d/Color.h"

#include <memory>
#include <optional>
#include <variant>

#include "a3d/Math.h"

namespace a3d {

    class Material;

    struct Ground {

        struct Procedural {

            struct GridComponent {

                Color color {};
                float spacing {1.0f};
                float lineWidthPixels {1.0f};
                float reliefStrength {0.0f};
            };

            struct Grid {

                Color                        color {};
                std::optional<GridComponent> minor {};
                std::optional<GridComponent> major {};
                float                        specularIntensity {0.15f};
                float                        specularExponent {32.0f};
            };

            using Content = std::variant<Grid>;

            Content content {Grid {}};
        };

        using Fill = std::variant<Procedural, std::shared_ptr<Material>>;

        struct RadialFade {

            Color      color {Color::Black()};
            math::vec2 center {0.0f, 0.0f};
            float      startDistance {10.0f};
            float      endDistance {100.0f};
        };

        struct HorizonHaze {

            Color color {};
            float angularWidthDegrees {3.0f};
        };

        Fill                       fill {Procedural {}};
        std::optional<RadialFade>  radialFade {};
        std::optional<HorizonHaze> horizonHaze {};
    };

}

#endif //AVARA3D_VISUAL_GROUND_H
