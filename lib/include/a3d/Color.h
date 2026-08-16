//
//  Color.h
//  avara3d
//
//  Created by Morgan Davis on 10/21/16.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_COLOR_H
#define AVARA3D_COLOR_H

#include <cstdint>
#include <string>

#include "a3d/Math.h"

namespace a3d {

    class Color {

    public:
        /// Public Static Member Functions ///

        static Color Black();
        static Color DarkGray();
        static Color Gray();
        static Color LightGray();
        static Color White();
        static Color Maroon();
        static Color Red();
        static Color Orange();
        static Color Yellow();
        static Color Olive();
        static Color Lime();
        static Color Green();
        static Color Cyan();
        static Color Blue();
        static Color Navy();
        static Color Teal();
        static Color Magenta();
        static Color Purple();
        static Color Brown();

        static Color Random();

        /// Public Lifecycle Functions ///

        Color();
        explicit Color(const math::vec3& rgb);
        explicit Color(const math::vec4& rgba);
        explicit Color(const math::u8vec3& irgb);
        explicit Color(const math::u8vec4& irgba);
        explicit Color(float white);
        explicit Color(uint32_t color);
        explicit Color(const std::string& hexString);

        // TODO: operator*, operator[]

        /// Public Member Functions ///

        float        r() const;
        float        g() const;
        float        b() const;
        float        a() const;

        uint8_t      u8r() const;
        uint8_t      u8g() const;
        uint8_t      u8b() const;
        uint8_t      u8a() const;

        math::vec3   rgb() const;
        math::vec4   rgba() const;

        math::u8vec3 u8rgb() const;
        math::u8vec4 u8rgba() const;

    private:
        /// Private Member Variables ///

        math::vec4 _rgba;
    };

}

#endif /* AVARA3D_COLOR_H */
