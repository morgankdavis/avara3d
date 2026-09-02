//
//  Color.cc
//  avara3d
//
//  Created by Morgan Davis on 10/21/16.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#include "a3d/Color.h"

#include <cctype>
#include <stdexcept>

using namespace a3d::math;
using namespace std;

namespace a3d {

// [Public Static Member Functions]

Color Color::Black() {
    return {0.0f, 0.0f, 0.0f, 1.0f};
}

Color Color::DarkGray() {
    return {0.25f, 0.25f, 0.25f, 1.0f};
}

Color Color::Gray() {
    return {0.5f, 0.5f, 0.5f, 1.0f};
}

Color Color::LightGray() {
    return {0.75f, 0.75f, 0.75f, 1.0f};
}

Color Color::White() {
    return {1.0f, 1.0f, 1.0f, 1.0f};
}

Color Color::Maroon() {
    return {128.0f / 255.0f, 0.0f, 0.0f, 1.0f};
}

Color Color::Red() {
    return {1.0f, 0.0f, 0.0f, 1.0f};
}

Color Color::Orange() {
    return {1.0f, 165.0f / 255.0f, 0.0f, 1.0f};
}

Color Color::Yellow() {
    return {1.0f, 1.0f, 0.0f, 1.0f};
}

Color Color::Olive() {
    return {128.0f / 255.0f, 128.0f / 255.0f, 0.0f, 1.0f};
}

Color Color::Lime() {
    return {0.0f, 1.0f, 0.0f, 1.0f};
}

Color Color::Green() {
    return {0.0f, 128.0f / 255.0f, 0.0f, 1.0f};
}

Color Color::Cyan() {
    return {0.0f, 1.0f, 1.0f, 1.0f};
}

Color Color::Blue() {
    return {0.0f, 0.0f, 1.0f, 1.0f};
}

Color Color::Navy() {
    return {0.0f, 0.0f, 128.0f / 255.0f, 1.0f};
}

Color Color::Teal() {
    return {0.0f, 128.0f / 255.0f, 128.0f / 255.0f, 1.0f};
}

Color Color::Magenta() {
    return {1.0f, 0.0f, 1.0f, 1.0f};
}

Color Color::Purple() {
    return {128.0f / 255.0f, 0.0f, 128.0f / 255.0f, 1.0f};
}

Color Color::Brown() {
    return {153.0f / 255.0f, 102.0f / 255.0f, 51.0f / 255.0f, 1.0f};
}

Color Color::Random() {
    return Color(u8vec3(uniform_linear(0, 255), uniform_linear(0, 255), uniform_linear(0, 255)));
}

} // namespace a3d

namespace a3d {

namespace {

    // [Private Non-Member Prototypes]

    bool        ParseHexRgb(const char* s, u8vec3& rgb);   // "RRGGBB" or "#RRGGBB"
    bool        ParseHexRgba(const char* s, u8vec4& rgba);  // "RRGGBBAA" or "#RRGGBBAA"
    int         HexNibble(char c);
    const char* SkipWs(const char* s);
    bool        ParseHexByte(const char* s, u8& out);
    uint8_t     FloatToU8(float x);

} // namespace

// [Public Lifecycle Functions]

Color::Color():
    _rgba {1.0, 1.0, 1.0, 1.0} {}

Color::Color(const vec3& rgb):
    _rgba {vec4 {math::clamp_01(rgb), 1.0f}} {}

Color::Color(float r, float g, float b):
    Color(math::vec3 {r, g, b}) {}

Color::Color(const vec4& rgba):
    _rgba {math::clamp_01(rgba)} {}

Color::Color(float r, float g, float b, float a):
    Color(math::vec4 {r, g, b, a}) {}

Color::Color(const u8vec3& irgb):
    _rgba {(float) irgb.r / 255.0f, (float) irgb.g / 255.0f, (float) irgb.b / 255.0f, 1.0} {}

Color::Color(const u8vec4& irgba):
    _rgba {(float) irgba.r / 255.0f, (float) irgba.g / 255.0f, (float) irgba.b / 255.0f,
           (float) irgba.a / 255.0f} {}

Color::Color(float white):
    _rgba {math::clamp_01(white), math::clamp_01(white), math::clamp_01(white), 1.0f} {}

Color::Color(uint32_t color):
    _rgba {(float) ((color & 0xFF000000) >> 24) / 255.0f, (float) ((color & 0x00FF0000) >> 16) / 255.0f,
           (float) ((color & 0x0000FF00) >> 8) / 255.0f, (float) ((color & 0x000000FF) >> 0) / 255.0f} {}

Color::Color(const string& hexString) {

    const bool rgb = hexString.length() == 6 || (hexString.length() == 7 && hexString.front() == '#');
    const bool rgba = hexString.length() == 8 || (hexString.length() == 9 && hexString.front() == '#');

    if (rgb) {

        u8vec3 value;

        if (!ParseHexRgb(hexString.c_str(), value)) {
            throw invalid_argument("Invalid hexadecimal color string.");
        }

        _rgba = {
            float(value.r) / 255.0f,
            float(value.g) / 255.0f,
            float(value.b) / 255.0f,
            1.0f,
        };

        return;
    }

    if (rgba) {

        u8vec4 value;

        if (!ParseHexRgba(hexString.c_str(), value)) {
            throw invalid_argument("Invalid hexadecimal color string.");
        }

        _rgba = {
            float(value.r) / 255.0f,
            float(value.g) / 255.0f,
            float(value.b) / 255.0f,
            float(value.a) / 255.0f,
        };

        return;
    }

    throw invalid_argument("Invalid hexadecimal color string.");
}

// [Public Member Functions]

float Color::r() const {
    return _rgba.r;
}

float Color::g() const {
    return _rgba.g;
}

float Color::b() const {
    return _rgba.b;
}

float Color::a() const {
    return _rgba.a;
}

uint8_t Color::u8r() const {
    return FloatToU8(_rgba.r);
}

uint8_t Color::u8g() const {
    return FloatToU8(_rgba.g);
}

uint8_t Color::u8b() const {
    return FloatToU8(_rgba.b);
}

uint8_t Color::u8a() const {
    return FloatToU8(_rgba.a);
}

vec3 Color::rgb() const {
    return vec3(_rgba);
}

vec4 Color::rgba() const {
    return _rgba;
}

u8vec3 Color::u8rgb() const {
    return u8vec3 {u8r(), u8g(), u8b()};
}

u8vec4 Color::u8rgba() const {
    return u8vec4 {u8r(), u8g(), u8b(), u8a()};
}

namespace {

    // [Private Non-Member Functions]

    bool ParseHexRgb(const char* s, u8vec3& rgb) {
        s = SkipWs(s);
        if (!s) {
            return false;
        }
        if (*s == '#') {
            ++s;
        }

        u8 r = 0, g = 0, b = 0;
        if (!ParseHexByte(s + 0, r)) {
            return false;
        }
        if (!ParseHexByte(s + 2, g)) {
            return false;
        }
        if (!ParseHexByte(s + 4, b)) {
            return false;
        }

        rgb.r = r;
        rgb.g = g;
        rgb.b = b;

        return true;
    }

    bool ParseHexRgba(const char* s, u8vec4& rgba) {
        s = SkipWs(s);
        if (!s) {
            return 0;
        }
        if (*s == '#') {
            ++s;
        }

        u8 r = 0, g = 0, b = 0, a = 0;
        if (!ParseHexByte(s + 0, r)) {
            return false;
        }
        if (!ParseHexByte(s + 2, g)) {
            return false;
        }
        if (!ParseHexByte(s + 4, b)) {
            return false;
        }
        if (!ParseHexByte(s + 6, a)) {
            return false;
        }

        rgba.r = r;
        rgba.g = g;
        rgba.b = b;
        rgba.a = a;

        return true;
    }

    int HexNibble(char c) {
        if (c >= '0' && c <= '9') {
            return c - '0';
        }
        if (c >= 'a' && c <= 'f') {
            return 10 + (c - 'a');
        }
        if (c >= 'A' && c <= 'F') {
            return 10 + (c - 'A');
        }
        return -1;
    }

    const char* SkipWs(const char* s) {
        while (s && *s && std::isspace(static_cast<unsigned char>(*s))) {
            ++s;
        }
        return s;
    }

    bool ParseHexByte(const char* s, u8& out) {
        const int hi = HexNibble(s[0]);
        const int lo = HexNibble(s[1]);
        if (hi < 0 || lo < 0) {
            return false;
        }
        out = static_cast<u8>((hi << 4) | lo);
        return true;
    }

    uint8_t FloatToU8(float x) {
        if (!math::is_finite(x)) {
            return 0;
        }
        x = math::clamp(x, 0.0f, 1.0f);
        return static_cast<uint8_t>(math::round(x * 255.0f));
    }

} // namespace

} // namespace a3d
