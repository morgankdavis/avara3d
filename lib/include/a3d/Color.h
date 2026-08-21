//
//  Color.h
//  avara3d
//
//  Created by Morgan Davis on 10/21/16.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_COLOR_H
#define AVARA3D_COLOR_H

#include <cstdint>
#include <string>

#include "a3d/Math.h"

namespace a3d {

    /**
     * @brief Represents a normalized RGBA color.
     *
     * Components are stored as floating-point values in the range [0, 1].
     * Floating-point inputs outside this range are clamped. Eight-bit
     * accessors scale the stored components to [0, 255] and round to the
     * nearest integer.
     */
    class Color {

    public:
        // [Public Static Member Functions]

        /** @brief Returns opaque black. */
        static Color Black();

        /** @brief Returns opaque dark gray. */
        static Color DarkGray();

        /** @brief Returns opaque gray. */
        static Color Gray();

        /** @brief Returns opaque light gray. */
        static Color LightGray();

        /** @brief Returns opaque white. */
        static Color White();

        /** @brief Returns opaque maroon. */
        static Color Maroon();

        /** @brief Returns opaque red. */
        static Color Red();

        /** @brief Returns opaque orange. */
        static Color Orange();

        /** @brief Returns opaque yellow. */
        static Color Yellow();

        /** @brief Returns opaque olive. */
        static Color Olive();

        /** @brief Returns opaque lime. */
        static Color Lime();

        /** @brief Returns opaque green. */
        static Color Green();

        /** @brief Returns opaque cyan. */
        static Color Cyan();

        /** @brief Returns opaque blue. */
        static Color Blue();

        /** @brief Returns opaque navy. */
        static Color Navy();

        /** @brief Returns opaque teal. */
        static Color Teal();

        /** @brief Returns opaque magenta. */
        static Color Magenta();

        /** @brief Returns opaque purple. */
        static Color Purple();

        /** @brief Returns opaque brown. */
        static Color Brown();

        /** @brief Returns an opaque color with independently randomized 8-bit RGB channels. */
        static Color Random();

        // [Public Lifecycle Functions]

        /** @brief Creates opaque white. */
        Color();

        /** @brief Creates an opaque color from floating-point RGB components. */
        explicit Color(const math::vec3& rgb);

        /** @brief Creates a color from floating-point RGBA components. */
        explicit Color(const math::vec4& rgba);

        /** @brief Creates an opaque color from 8-bit RGB components. */
        explicit Color(const math::u8vec3& irgb);

        /** @brief Creates a color from 8-bit RGBA components. */
        explicit Color(const math::u8vec4& irgba);

        /** @brief Creates an opaque grayscale color with all RGB components set to @p white. */
        explicit Color(float white);

        /** @brief Creates a color from a packed 0xRRGGBBAA value. */
        explicit Color(uint32_t color);

        /**
         * @brief Creates a color from hexadecimal RGB or RGBA text.
         *
         * Supported forms are RRGGBB, #RRGGBB, RRGGBBAA, and #RRGGBBAA.
         */
        explicit Color(const std::string& hexString);

        // TODO: operator*, operator[]

        // [Public Member Functions]

        /** @brief Returns the floating-point red component. */
        float        r() const;

        /** @brief Returns the floating-point green component. */
        float        g() const;

        /** @brief Returns the floating-point blue component. */
        float        b() const;

        /** @brief Returns the floating-point alpha component. */
        float        a() const;

        /** @brief Returns the red component converted to 8-bit form. */
        uint8_t      u8r() const;

        /** @brief Returns the green component converted to 8-bit form. */
        uint8_t      u8g() const;

        /** @brief Returns the blue component converted to 8-bit form. */
        uint8_t      u8b() const;

        /** @brief Returns the alpha component converted to 8-bit form. */
        uint8_t      u8a() const;

        /** @brief Returns the floating-point RGB components. */
        math::vec3   rgb() const;

        /** @brief Returns the floating-point RGBA components. */
        math::vec4   rgba() const;

        /** @brief Returns the RGB components converted to 8-bit form. */
        math::u8vec3 u8rgb() const;

        /** @brief Returns the RGBA components converted to 8-bit form. */
        math::u8vec4 u8rgba() const;

    private:
        // [Private Member Variables]

        math::vec4 _rgba;
    };

}

#endif // AVARA3D_COLOR_H
