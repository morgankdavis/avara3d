//
//  Color.cc
//  avara3d
//
//  Created by Morgan Davis on 10/21/16.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#include "a3d/Color.h"

#include <cctype>

using namespace a3d;
using namespace a3d::math;
using namespace std;

// [Public Static Member Functions]

Color Color::Black() {
    return Color(vec4 {0.0f, 0.0f, 0.0f, 1.0f});
}

Color Color::DarkGray() {
    return Color(vec4 {0.25f, 0.25f, 0.25f, 1.0f});
}

Color Color::Gray() {
    return Color(vec4 {0.5f, 0.5f, 0.5f, 1.0f});
}

Color Color::LightGray() {
    return Color(vec4 {0.75f, 0.75f, 0.75f, 1.0f});
}

Color Color::White() {
    return Color(vec4 {1.0f, 1.0f, 1.0f, 1.0f});
}

Color Color::Maroon() {
    return Color(vec4 {128.0f / 255.0f, 0.0f, 0.0f, 1.0f});
}

Color Color::Red() {
    return Color(vec4 {1.0f, 0.0f, 0.0f, 1.0f});
}

Color Color::Orange() {
    return Color(vec4 {1.0f, 165.0f / 255.0f, 0.0f, 1.0f});
}

Color Color::Yellow() {
    return Color(vec4 {1.0f, 1.0f, 0.0f, 1.0f});
}

Color Color::Olive() {
    return Color(vec4 {128.0f / 255.0f, 128.0f / 255.0f, 0.0f, 1.0f});
}

Color Color::Lime() {
    return Color(vec4 {0.0f, 1.0f, 0.0f, 1.0f});
}

Color Color::Green() {
    return Color(vec4 {0.0f, 128.0f / 255.0f, 0.0f, 1.0f});
}

Color Color::Cyan() {
    return Color(vec4 {0.0f, 1.0f, 1.0f, 1.0f});
}

Color Color::Blue() {
    return Color(vec4 {0.0f, 0.0f, 1.0f, 1.0f});
}

Color Color::Navy() {
    return Color(vec4 {0.0f, 0.0f, 128.0f / 255.0f, 1.0f});
}

Color Color::Teal() {
    return Color(vec4 {0.0f, 128.0f / 255.0f, 128.0f / 255.0f, 1.0f});
}

Color Color::Magenta() {
    return Color(vec4 {1.0f, 0.0f, 1.0f, 1.0f});
}

Color Color::Purple() {
    return Color(vec4 {128.0f / 255.0f, 0.0f, 128.0f / 255.0f, 1.0f});
}

Color Color::Brown() {
    return Color(vec4 {153.0f / 255.0f, 102.0f / 255.0f, 51.0f / 255.0f, 1.0f});
}

Color Color::Random() {
    return Color(u8vec3(uniform_linear(0, 255), uniform_linear(0, 255), uniform_linear(0, 255)));
}

// [Private Static Non-Member Prototypes]

static bool ParseHexRgb(const char* s, u8vec3& rgb);   // "RRGGBB" or "#RRGGBB"
static bool ParseHexRgba(const char* s, u8vec4& rgba);  // "RRGGBBAA" or "#RRGGBBAA"
//static u32 ParseHexRgb(const char* s);   // "RRGGBB" or "#RRGGBB"
//static u32 ParseHexRgba(const char* s);  // "RRGGBBAA" or "#RRGGBBAA"
static int         HexNibble(char c);
static const char* SkipWs(const char* s);
static bool        ParseHexByte(const char* s, u8& out);
static uint8_t     FloatToU8(float x);

// [Public Lifecycle Functions]

Color::Color():
    _rgba {1.0, 1.0, 1.0, 1.0} {}

Color::Color(const vec3& rgb):
    _rgba {vec4(rgb, 1.0)} {}

Color::Color(const vec4& rgba):
    _rgba {rgba} {}

Color::Color(const u8vec3& irgb):
    _rgba {(float) irgb.r / 255.0f, (float) irgb.g / 255.0f, (float) irgb.b / 255.0f, 1.0} {}

Color::Color(const u8vec4& irgba):
    _rgba {(float) irgba.r / 255.0f, (float) irgba.g / 255.0f, (float) irgba.b / 255.0f,
           (float) irgba.a / 255.0f} {}

Color::Color(float white):
    _rgba {white, white, white, 1.0} {}

Color::Color(uint32_t color):
    _rgba {(float) ((color & 0xFF000000) >> 24) / 255.0f, (float) ((color & 0x00FF0000) >> 16) / 255.0f,
           (float) ((color & 0x0000FF00) >> 8) / 255.0f, (float) ((color & 0x000000FF) >> 0) / 255.0f} {}

Color::Color(const string& hexString) {
    if (hexString.length() > 7) { // assume this means it has alpha...
        u8vec4 u8rgba;
        ParseHexRgba(hexString.c_str(), u8rgba);
        _rgba = {float(u8rgba.r) / 255.0f, float(u8rgba.g) / 255.0f, float(u8rgba.b) / 255.0f,
                 float(u8rgba.a) / 255.0f};
    }
    else {
        u8vec3 u8rgb;
        ParseHexRgb(hexString.c_str(), u8rgb);
        _rgba = {float(u8rgb.r) / 255.0f, float(u8rgb.g) / 255.0f, float(u8rgb.b) / 255.0f, 1.0f};
    }
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

// [Private Static Non-Members]

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

//u32 ParseHexRgb(const char* s) {
//	s = SkipWs(s);
//	if (!s) return 0;
//	if (*s == '#') ++s;
//
//	u8 r=0,g=0,b=0;
//	if (!ParseHexByte(s + 0, r)) return 0;
//	if (!ParseHexByte(s + 2, g)) return 0;
//	if (!ParseHexByte(s + 4, b)) return 0;
//
//	return (u32(r) << 16) | (u32(g) << 8) | (u32(b) << 0); // 0xRRGGBB
//}
//
//u32 ParseHexRgba(const char* s) {
//	s = SkipWs(s);
//	if (!s) return 0;
//	if (*s == '#') ++s;
//
//	u8 r=0,g=0,b=0,a=0;
//	if (!ParseHexByte(s + 0, r)) return 0;
//	if (!ParseHexByte(s + 2, g)) return 0;
//	if (!ParseHexByte(s + 4, b)) return 0;
//	if (!ParseHexByte(s + 6, a)) return 0;
//
//	return (u32(r) << 24) | (u32(g) << 16) | (u32(b) << 8) | (u32(a) << 0); // 0xRRGGBBAA
//}

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

static uint8_t FloatToU8(float x) {
    if (!math::is_finite(x)) {
        return 0;
    }
    x = math::clamp(x, 0.0f, 1.0f);
    return static_cast<uint8_t>(math::round(x * 255.0f));
}
