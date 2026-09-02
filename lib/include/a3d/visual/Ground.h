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

/** @brief Configures ground rendering over a VisualWorld reference Surface. */
struct Ground {

    // [Public Types]

    /** @brief Configures a procedurally rendered ground fill. */
    struct Procedural {

        // [Public Types]

        /** @brief Configures one set of procedural grid lines. */
        struct GridComponent {

            // [Public Member Variables]

            Color color {};                 ///< Grid-line color.
            float spacing {1.0f}; ///< Distance between grid lines in scene units; must be greater than zero.
            float lineWidthPixels {1.0f}; ///< Grid-line width in pixels; must be greater than zero.
            float reliefStrength {0.0f}; ///< Strength of procedural grid relief; zero disables relief.
        };

        /** @brief Configures a procedural grid with optional minor and major line components. */
        struct Grid {

            // [Public Member Variables]

            Color                        color {}; ///< Base ground color between grid lines.
            std::optional<GridComponent> minor {}; ///< Optional minor grid lines.
            std::optional<GridComponent> major {}; ///< Optional major grid lines.
            float                        specularIntensity {
                0.15f}; ///< Phong specular intensity coefficient; must be non-negative; zero disables specular highlights.
            float specularExponent {
                32.0f}; ///< Phong specular exponent; must be greater than zero; higher values produce narrower highlights.
        };

        /** @brief Supported procedural ground content. */
        using Content = std::variant<Grid>;

        // [Public Member Variables]

        Content content {Grid {}}; ///< Procedural content to render.
    };

    /** @brief Procedural or material-backed ground fill. */
    using Fill = std::variant<Procedural, std::shared_ptr<Material>>;

    /** @brief Configures a radial transition from the ground fill toward a color. */
    struct RadialFade {

        // [Public Member Variables]

        Color      color {Color::Black()}; ///< Color approached beyond the fade range.
        math::vec2 center {0.0f, 0.0f}; ///< Center of the radial fade on the ground surface.
        float startDistance {10.0f}; ///< Distance from the center where fading begins; must be non-negative.
        float endDistance {100.0f}; ///< Distance where fading completes; must be greater than startDistance.
    };

    /** @brief Configures haze concentrated around the ground horizon. */
    struct HorizonHaze {

        // [Public Member Variables]

        Color color {}; ///< Horizon-haze color.
        float angularWidth {
            math::radians(3.0f)}; ///< Angular extent from the horizon, in radians; must be in (0, pi/2].
    };

    // [Public Member Variables]

    Fill                       fill {Procedural {}}; ///< Ground fill to render.
    std::optional<RadialFade>  radialFade {}; ///< Optional radial fade applied to the ground.
    std::optional<HorizonHaze> horizonHaze {}; ///< Optional haze applied around the ground horizon.
};

} // namespace a3d

#endif // AVARA3D_VISUAL_GROUND_H
