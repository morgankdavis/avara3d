//
//  Math.cc
//  avara3d
//
//  Created by Morgan Davis on 12/7/25.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#include "a3d/Math.h"

#include <algorithm>
#include <cmath>
#include <format>
#include <sstream>

#include "a3d/Assert.h"

namespace a3d::math {

    /// Private Utility Prototypes ///

    static std::mt19937& default_random_gen();
    static std::mt19937& pick_random_gen(std::mt19937* gen);
    static f32           ease_out_bounce_impl(f32 t);
    static f32           hue2rgb(f32 p, f32 q, f32 t);
    static std::string   mat_right_fit(std::string s, int width);
    static std::string   mat_fmtf(float v, unsigned width);
    static u8            saturate_u8(f32 v);
    f32quat              operator-(const f32quat& q);
    f32quat              operator+(const f32quat& a, const f32quat& b);
    f32quat&             operator+=(f32quat& a, const f32quat& b);

    /// Types ///

    f32vec2::f32vec2():
        x {0},
        y {0} {}

    f32vec2::f32vec2(f32 x_, f32 y_):
        x {x_},
        y {y_} {}

    f32vec2::f32vec2(f32 n):
        x {n},
        y {n} {}

    f32vec2::f32vec2(const f32vec3& v):
        x {v.x},
        y {v.y} {}

    f32vec2::f32vec2(const f32vec4& v):
        x {v.x},
        y {v.y} {}

    f32& f32vec2::operator[](std::size_t i) {
        A3D_ASSERT(i < 2);
        return (&x)[i];
    }

    const f32& f32vec2::operator[](std::size_t i) const {
        A3D_ASSERT(i < 2);
        return (&x)[i];
    }

    f32vec3::f32vec3():
        x {0},
        y {0},
        z {0} {}

    f32vec3::f32vec3(f32 x_, f32 y_, f32 z_):
        x {x_},
        y {y_},
        z {z_} {}

    f32vec3::f32vec3(f32 n):
        x {n},
        y {n},
        z {n} {}

    f32vec3::f32vec3(const f32vec2& v, f32 z_):
        x {v.x},
        y {v.y},
        z {z_} {}

    f32vec3::f32vec3(const f32vec2& v):
        x {v.x},
        y {v.y},
        z {0} {}

    f32vec3::f32vec3(const f32vec4& v):
        x {v.x},
        y {v.y},
        z {v.z} {}

    f32& f32vec3::operator[](std::size_t i) {
        A3D_ASSERT(i < 3);
        return (&x)[i];
    }

    const f32& f32vec3::operator[](std::size_t i) const {
        A3D_ASSERT(i < 3);
        return (&x)[i];
    }

    f32vec4::f32vec4():
        x {0},
        y {0},
        z {0},
        w {0} {}

    f32vec4::f32vec4(f32 x_, f32 y_, f32 z_, f32 w_):
        x {x_},
        y {y_},
        z {z_},
        w {w_} {}

    f32vec4::f32vec4(const f32vec3& v, f32 w_):
        x {v.x},
        y {v.y},
        z {v.z},
        w {w_} {}

    f32vec4::f32vec4(f32 n):
        x {n},
        y {n},
        z {n},
        w {n} {}

    f32vec4::f32vec4(const f32vec2& v):
        x {v.x},
        y {v.y},
        z {0},
        w {0} {}

    f32vec4::f32vec4(const f32vec3& v):
        x {v.x},
        y {v.y},
        z {v.z},
        w {0} {}

    f32& vec4::operator[](std::size_t i) {
        A3D_ASSERT(i < 4);
        return (&x)[i];
    }

    const f32& vec4::operator[](std::size_t i) const {
        A3D_ASSERT(i < 4);
        return (&x)[i];
    }

    i32vec2::i32vec2():
        x {0},
        y {0} {}

    i32vec2::i32vec2(i32 x_, i32 y_):
        x {x_},
        y {y_} {}

    i32vec2::i32vec2(i32 n):
        x {n},
        y {n} {}

    i32vec2::i32vec2(const i32vec3& v):
        x {v.x},
        y {v.y} {}

    i32vec2::i32vec2(const i32vec4& v):
        x {v.x},
        y {v.y} {}

    i32& i32vec2::operator[](std::size_t i) {
        A3D_ASSERT(i < 2);
        return (&x)[i];
    }

    const i32& i32vec2::operator[](std::size_t i) const {
        A3D_ASSERT(i < 2);
        return (&x)[i];
    }

    i32vec3::i32vec3():
        x {0},
        y {0},
        z {0} {}

    i32vec3::i32vec3(i32 x_, i32 y_, i32 z_):
        x {x_},
        y {y_},
        z {z_} {}

    i32vec3::i32vec3(const i32vec2& v, i32 z_):
        x {v.x},
        y {v.y},
        z {z_} {}

    i32vec3::i32vec3(i32 n):
        x {n},
        y {n},
        z {n} {}

    i32vec3::i32vec3(const i32vec2& v):
        x {v.x},
        y {v.y},
        z {0} {}

    i32vec3::i32vec3(const i32vec4& v):
        x {v.x},
        y {v.y},
        z {v.z} {}

    i32& i32vec3::operator[](std::size_t i) {
        A3D_ASSERT(i < 3);
        return (&x)[i];
    }

    const i32& i32vec3::operator[](std::size_t i) const {
        A3D_ASSERT(i < 3);
        return (&x)[i];
    }

    i32vec4::i32vec4():
        x {0},
        y {0},
        z {0},
        w {0} {}

    i32vec4::i32vec4(i32 x_, i32 y_, i32 z_, i32 w_):
        x {x_},
        y {y_},
        z {z_},
        w {w_} {}

    i32vec4::i32vec4(const i32vec3& v, i32 w_):
        x {v.x},
        y {v.y},
        z {v.z},
        w {w_} {}

    i32vec4::i32vec4(i32 n):
        x {n},
        y {n},
        z {n},
        w {n} {}

    i32vec4::i32vec4(const i32vec2& v):
        x {v.x},
        y {v.y},
        z {0},
        w {0} {}

    i32vec4::i32vec4(const i32vec3& v):
        x {v.x},
        y {v.y},
        z {v.z},
        w {0} {}

    i32& i32vec4::operator[](std::size_t i) {
        A3D_ASSERT(i < 4);
        return (&x)[i];
    }

    const i32& i32vec4::operator[](std::size_t i) const {
        A3D_ASSERT(i < 4);
        return (&x)[i];
    }

    u32vec2::u32vec2():
        x {0},
        y {0} {}

    u32vec2::u32vec2(u32 x_, u32 y_):
        x {x_},
        y {y_} {}

    u32vec2::u32vec2(u32 n):
        x {n},
        y {n} {}

    u32vec2::u32vec2(const u32vec3& v):
        x {v.x},
        y {v.y} {}

    u32vec2::u32vec2(const u32vec4& v):
        x {v.x},
        y {v.y} {}

    u32& u32vec2::operator[](std::size_t i) {
        A3D_ASSERT(i < 2);
        return (&x)[i];
    }

    const u32& u32vec2::operator[](std::size_t i) const {
        A3D_ASSERT(i < 2);
        return (&x)[i];
    }

    u32vec3::u32vec3():
        x {0},
        y {0},
        z {0} {}

    u32vec3::u32vec3(u32 x_, u32 y_, u32 z_):
        x {x_},
        y {y_},
        z {z_} {}

    u32vec3::u32vec3(const u32vec2& v, u32 z_):
        x {v.x},
        y {v.y},
        z {z_} {}

    u32vec3::u32vec3(u32 n):
        x {n},
        y {n},
        z {n} {}

    u32vec3::u32vec3(const u32vec2& v):
        x {v.x},
        y {v.y},
        z {0} {}

    u32vec3::u32vec3(const u32vec4& v):
        x {v.x},
        y {v.y},
        z {v.z} {}

    u32& u32vec3::operator[](std::size_t i) {
        A3D_ASSERT(i < 3);
        return (&x)[i];
    }

    const u32& u32vec3::operator[](std::size_t i) const {
        A3D_ASSERT(i < 3);
        return (&x)[i];
    }

    u32vec4::u32vec4():
        x {0},
        y {0},
        z {0},
        w {0} {}

    u32vec4::u32vec4(u32 x_, u32 y_, u32 z_, u32 w_):
        x {x_},
        y {y_},
        z {z_},
        w {w_} {}

    u32vec4::u32vec4(const u32vec3& v, u32 w_):
        x {v.x},
        y {v.y},
        z {v.z},
        w {w_} {}

    u32vec4::u32vec4(u32 n):
        x {n},
        y {n},
        z {n},
        w {n} {}

    u32vec4::u32vec4(const u32vec2& v):
        x {v.x},
        y {v.y},
        z {0},
        w {0} {}

    u32vec4::u32vec4(const u32vec3& v):
        x {v.x},
        y {v.y},
        z {v.z},
        w {0} {}

    u32& u32vec4::operator[](std::size_t i) {
        A3D_ASSERT(i < 4);
        return (&x)[i];
    }

    const u32& u32vec4::operator[](std::size_t i) const {
        A3D_ASSERT(i < 4);
        return (&x)[i];
    }

    u8vec2::u8vec2():
        x {0},
        y {0} {}

    u8vec2::u8vec2(u8 x_, u8 y_):
        x {x_},
        y {y_} {}

    u8vec2::u8vec2(u8 n):
        x {n},
        y {n} {}

    u8vec2::u8vec2(const u8vec3& v):
        x {v.x},
        y {v.y} {}

    u8vec2::u8vec2(const u8vec4& v):
        x {v.x},
        y {v.y} {}

    u8& u8vec2::operator[](std::size_t i) {
        A3D_ASSERT(i < 2);
        return (&x)[i];
    }

    const u8& u8vec2::operator[](std::size_t i) const {
        A3D_ASSERT(i < 2);
        return (&x)[i];
    }

    u8vec3::u8vec3():
        x {0},
        y {0},
        z {0} {}

    u8vec3::u8vec3(u8 x_, u8 y_, u8 z_):
        x {x_},
        y {y_},
        z {z_} {}

    u8vec3::u8vec3(const u8vec2& v, u8 z_):
        x {v.x},
        y {v.y},
        z {z_} {}

    u8vec3::u8vec3(u8 n):
        x {n},
        y {n},
        z {n} {}

    u8vec3::u8vec3(const u8vec2& v):
        x {v.x},
        y {v.y},
        z {0} {}

    u8vec3::u8vec3(const u8vec4& v):
        x {v.x},
        y {v.y},
        z {v.z} {}

    u8& u8vec3::operator[](std::size_t i) {
        A3D_ASSERT(i < 3);
        return (&x)[i];
    }

    const u8& u8vec3::operator[](std::size_t i) const {
        A3D_ASSERT(i < 3);
        return (&x)[i];
    }

    u8vec4::u8vec4():
        x {0},
        y {0},
        z {0},
        w {0} {}

    u8vec4::u8vec4(u8 x_, u8 y_, u8 z_, u8 w_):
        x {x_},
        y {y_},
        z {z_},
        w {w_} {}

    u8vec4::u8vec4(const u8vec3& v, u8 w_):
        x {v.x},
        y {v.y},
        z {v.z},
        w {w_} {}

    u8vec4::u8vec4(u8 n):
        x {n},
        y {n},
        z {n},
        w {n} {}

    u8vec4::u8vec4(const u8vec2& v):
        x {v.x},
        y {v.y},
        z {0},
        w {0} {}

    u8vec4::u8vec4(const u8vec3& v):
        x {v.x},
        y {v.y},
        z {v.z},
        w {0} {}

    u8& u8vec4::operator[](std::size_t i) {
        A3D_ASSERT(i < 4);
        return (&x)[i];
    }

    const u8& u8vec4::operator[](std::size_t i) const {
        A3D_ASSERT(i < 4);
        return (&x)[i];
    }

    f32mat2::f32mat2():
        c0 {1, 0},
        c1 {0, 1} {}

    f32mat2::f32mat2(f32 diag):
        c0 {diag, 0.0f},
        c1 {0.0f, diag} {}

    f32mat2::f32mat2(const f32vec2& c0_, const f32vec2& c1_):
        c0 {c0_},
        c1 {c1_} {}

    f32vec2& f32mat2::operator[](std::size_t i) {
        A3D_ASSERT(i < 2);
        return (&c0)[i];
    }

    const f32vec2& f32mat2::operator[](std::size_t i) const {
        A3D_ASSERT(i < 2);
        return (&c0)[i];
    }

    f32mat3::f32mat3():
        c0 {1, 0, 0},
        c1 {0, 1, 0},
        c2 {0, 0, 1} {}

    f32mat3::f32mat3(f32 diag):
        c0 {diag, 0.0f, 0.0f},
        c1 {0.0f, diag, 0.0f},
        c2 {0.0f, 0.0f, diag} {}

    f32mat3::f32mat3(const f32vec3& c0_, const f32vec3& c1_, const f32vec3& c2_):
        c0 {c0_},
        c1 {c1_},
        c2 {c2_} {}

    f32mat3::f32mat3(const f32mat4& m):
        c0 {m.c0.x, m.c0.y, m.c0.z},
        c1 {m.c1.x, m.c1.y, m.c1.z},
        c2 {m.c2.x, m.c2.y, m.c2.z} {}

    f32vec3& f32mat3::operator[](std::size_t i) {
        A3D_ASSERT(i < 3);
        return (&c0)[i];
    }

    const f32vec3& f32mat3::operator[](std::size_t i) const {
        A3D_ASSERT(i < 3);
        return (&c0)[i];
    }

    f32mat4::f32mat4():
        c0 {1, 0, 0, 0},
        c1 {0, 1, 0, 0},
        c2 {0, 0, 1, 0},
        c3 {0, 0, 0, 1} {}

    f32mat4::f32mat4(f32 diag):
        c0 {diag, 0.0f, 0.0f, 0.0f},
        c1 {0.0f, diag, 0.0f, 0.0f},
        c2 {0.0f, 0.0f, diag, 0.0f},
        c3 {0.0f, 0.0f, 0.0f, diag} {}

    f32mat4::f32mat4(const f32vec4& c0_, const f32vec4& c1_, const f32vec4& c2_, const f32vec4& c3_):
        c0 {c0_},
        c1 {c1_},
        c2 {c2_},
        c3 {c3_} {}

    f32mat4::f32mat4(const f32mat3& m):
        c0 {m.c0.x, m.c0.y, m.c0.z, 0.0f},
        c1 {m.c1.x, m.c1.y, m.c1.z, 0.0f},
        c2 {m.c2.x, m.c2.y, m.c2.z, 0.0f},
        c3 {0.0f, 0.0f, 0.0f, 1.0f} {}

    vec4& f32mat4::operator[](std::size_t i) {
        A3D_ASSERT(i < 4);
        return (&c0)[i];
    }

    const f32vec4& f32mat4::operator[](std::size_t i) const {
        A3D_ASSERT(i < 4);
        return (&c0)[i];
    }

    f32quat::f32quat():
        w {1.0f},
        x {0.0f},
        y {0.0f},
        z {0.0f} {}

    f32quat::f32quat(f32 w_, f32 x_, f32 y_, f32 z_):
        w {w_},
        x {x_},
        y {y_},
        z {z_} {}

    f32quat::f32quat(f32 s):
        w {s},
        x {0.0f},
        y {0.0f},
        z {0.0f} {}

    f32& f32quat::operator[](std::size_t i) {
        A3D_ASSERT(i < 4);
        return (&w)[i];
    }

    const f32& f32quat::operator[](std::size_t i) const {
        A3D_ASSERT(i < 4);
        return (&w)[i];
    }

    /// 32-bit Float Vector ///

    f32vec2 operator-(const f32vec2& v) {
        return {-v.x, -v.y};
    }

    f32vec3 operator-(const f32vec3& v) {
        return {-v.x, -v.y, -v.z};
    }

    f32vec4 operator-(const f32vec4& v) {
        return {-v.x, -v.y, -v.z, -v.w};
    }

    f32vec2 operator+(const f32vec2& a, const f32vec2& b) {
        return f32vec2 {a.x + b.x, a.y + b.y};
    }

    f32vec3 operator+(const f32vec3& a, const f32vec3& b) {
        return f32vec3 {a.x + b.x, a.y + b.y, a.z + b.z};
    }

    f32vec4 operator+(const f32vec4& a, const f32vec4& b) {
        return f32vec4 {a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w};
    }

    f32vec2 operator-(const f32vec2& a, const f32vec2& b) {
        return f32vec2 {a.x - b.x, a.y - b.y};
    }

    f32vec3 operator-(const f32vec3& a, const f32vec3& b) {
        return f32vec3 {a.x - b.x, a.y - b.y, a.z - b.z};
    }

    f32vec4 operator-(const f32vec4& a, const f32vec4& b) {
        return f32vec4 {a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w};
    }

    f32vec2 operator*(const f32vec2& a, const f32vec2& b) {
        return f32vec2 {a.x * b.x, a.y * b.y};
    }

    f32vec3 operator*(const f32vec3& a, const f32vec3& b) {
        return f32vec3 {a.x * b.x, a.y * b.y, a.z * b.z};
    }

    f32vec4 operator*(const f32vec4& a, const f32vec4& b) {
        return f32vec4 {a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w};
    }

    f32vec2 operator*(const f32vec2& v, f32 s) {
        return f32vec2 {v.x * s, v.y * s};
    }

    f32vec3 operator*(const f32vec3& v, f32 s) {
        return f32vec3 {v.x * s, v.y * s, v.z * s};
    }

    f32vec4 operator*(const f32vec4& v, f32 s) {
        return f32vec4 {v.x * s, v.y * s, v.z * s, v.w * s};
    }

    f32vec2 operator*(f32 s, const f32vec2& v) {
        return v * s;
    }

    f32vec3 operator*(f32 s, const f32vec3& v) {
        return v * s;
    }

    f32vec4 operator*(f32 s, const f32vec4& v) {
        return v * s;
    }

    f32vec2 operator/(const f32vec2& a, const f32vec2& b) {
        return f32vec2 {a.x / b.x, a.y / b.y};
    }

    f32vec3 operator/(const f32vec3& a, const f32vec3& b) {
        return f32vec3 {a.x / b.x, a.y / b.y, a.z / b.z};
    }

    f32vec4 operator/(const f32vec4& a, const f32vec4& b) {
        return f32vec4 {a.x / b.x, a.y / b.y, a.z / b.z, a.w / b.w};
    }

    f32vec2 operator/(const f32vec2& v, f32 s) {
        f32 inv = 1.0f / s;
        return f32vec2 {v.x * inv, v.y * inv};
    }

    f32vec3 operator/(const f32vec3& v, f32 s) {
        f32 inv = 1.0f / s;
        return f32vec3 {v.x * inv, v.y * inv, v.z * inv};
    }

    f32vec4 operator/(const f32vec4& v, f32 s) {
        f32 inv = 1.0f / s;
        return f32vec4 {v.x * inv, v.y * inv, v.z * inv, v.w * inv};
    }

    f32vec2& operator+=(f32vec2& a, const f32vec2& b) {
        a.x += b.x;
        a.y += b.y;
        return a;
    }

    f32vec3& operator+=(f32vec3& a, const f32vec3& b) {
        a.x += b.x;
        a.y += b.y;
        a.z += b.z;
        return a;
    }

    f32vec4& operator+=(f32vec4& a, const f32vec4& b) {
        a.x += b.x;
        a.y += b.y;
        a.z += b.z;
        a.w += b.w;
        return a;
    }

    f32vec2& operator-=(f32vec2& a, const f32vec2& b) {
        a.x -= b.x;
        a.y -= b.y;
        return a;
    }

    f32vec3& operator-=(f32vec3& a, const f32vec3& b) {
        a.x -= b.x;
        a.y -= b.y;
        a.z -= b.z;
        return a;
    }

    f32vec4& operator-=(f32vec4& a, const f32vec4& b) {
        a.x -= b.x;
        a.y -= b.y;
        a.z -= b.z;
        a.w -= b.w;
        return a;
    }

    f32vec2& operator*=(f32vec2& a, const f32vec2& b) {
        a.x *= b.x;
        a.y *= b.y;
        return a;
    }

    f32vec3& operator*=(f32vec3& a, const f32vec3& b) {
        a.x *= b.x;
        a.y *= b.y;
        a.z *= b.z;
        return a;
    }

    f32vec4& operator*=(f32vec4& a, const f32vec4& b) {
        a.x *= b.x;
        a.y *= b.y;
        a.z *= b.z;
        a.w *= b.w;
        return a;
    }

    f32vec2& operator*=(f32vec2& v, f32 s) {
        v.x *= s;
        v.y *= s;
        return v;
    }

    f32vec3& operator*=(f32vec3& v, f32 s) {
        v.x *= s;
        v.y *= s;
        v.z *= s;
        return v;
    }

    f32vec4& operator*=(f32vec4& v, f32 s) {
        v.x *= s;
        v.y *= s;
        v.z *= s;
        v.w *= s;
        return v;
    }

    f32vec2& operator/=(f32vec2& a, const f32vec2& b) {
        a.x /= b.x;
        a.y /= b.y;
        return a;
    }

    f32vec3& operator/=(f32vec3& a, const f32vec3& b) {
        a.x /= b.x;
        a.y /= b.y;
        a.z /= b.z;
        return a;
    }

    f32vec4& operator/=(f32vec4& a, const f32vec4& b) {
        a.x /= b.x;
        a.y /= b.y;
        a.z /= b.z;
        a.w /= b.w;
        return a;
    }

    f32vec2& operator/=(f32vec2& v, f32 s) {
        f32 inv = 1.0f / s;
        v.x *= inv;
        v.y *= inv;
        return v;
    }

    f32vec3& operator/=(f32vec3& v, f32 s) {
        f32 inv = 1.0f / s;
        v.x *= inv;
        v.y *= inv;
        v.z *= inv;
        return v;
    }

    f32vec4& operator/=(f32vec4& v, f32 s) {
        f32 inv = 1.0f / s;
        v.x *= inv;
        v.y *= inv;
        v.z *= inv;
        v.w *= inv;
        return v;
    }

    bool operator==(const f32vec2& a, const f32vec2& b) {
        return equal(a.x, b.x) && equal(a.y, b.y);
    }

    bool operator==(const f32vec3& a, const f32vec3& b) {
        return equal(a.x, b.x) && equal(a.y, b.y) && equal(a.z, b.z);
    }

    bool operator==(const f32vec4& a, const f32vec4& b) {
        return equal(a.x, b.x) && equal(a.y, b.y) && equal(a.z, b.z) && equal(a.w, b.w);
    }

    bool operator!=(const f32vec3& a, const f32vec3& b) {
        return !(a == b);
    }

    bool operator!=(const f32vec4& a, const f32vec4& b) {
        return !(a == b);
    }

    f32 min(const f32vec2& v) {
        return math::min(v.x, v.y);
    }

    f32 min(const f32vec3& v) {
        return math::min(v.x, math::min(v.y, v.z));
    }

    f32 min(const f32vec4& v) {
        return math::min(math::min(v.x, v.y), math::min(v.z, v.w));
    }

    f32 max(const f32vec2& v) {
        return math::max(v.x, v.y);
    }

    f32 max(const f32vec3& v) {
        return math::max(v.x, math::max(v.y, v.z));
    }

    f32 max(const f32vec4& v) {
        return math::max(math::max(v.x, v.y), math::max(v.z, v.w));
    }

    f32vec2 min(const f32vec2& a, const f32vec2& b) {
        return {math::min(a.x, b.x), math::min(a.y, b.y)};
    }

    f32vec3 min(const f32vec3& a, const f32vec3& b) {
        return {math::min(a.x, b.x), math::min(a.y, b.y), math::min(a.z, b.z)};
    }

    f32vec4 min(const f32vec4& a, const f32vec4& b) {
        return {math::min(a.x, b.x), math::min(a.y, b.y), math::min(a.z, b.z), math::min(a.w, b.w)};
    }

    f32vec2 max(const f32vec2& a, const f32vec2& b) {
        return {math::max(a.x, b.x), math::max(a.y, b.y)};
    }

    f32vec3 max(const f32vec3& a, const f32vec3& b) {
        return {math::max(a.x, b.x), math::max(a.y, b.y), math::max(a.z, b.z)};
    }

    f32vec4 max(const f32vec4& a, const f32vec4& b) {
        return {math::max(a.x, b.x), math::max(a.y, b.y), math::max(a.z, b.z), math::max(a.w, b.w)};
    }

    f32vec2 abs(const f32vec2& v) {
        return {math::abs(v.x), math::abs(v.y)};
    }

    f32vec3 abs(const f32vec3& v) {
        return {math::abs(v.x), math::abs(v.y), math::abs(v.z)};
    }

    f32vec4 abs(const f32vec4& v) {
        return {math::abs(v.x), math::abs(v.y), math::abs(v.z), math::abs(v.w)};
    }

    f32 dot(const f32vec2& a, const f32vec2& b) {
        return a.x * b.x + a.y * b.y;
    }

    f32 dot(const f32vec3& a, const f32vec3& b) {
        return a.x * b.x + a.y * b.y + a.z * b.z;
    }

    f32 dot(const f32vec4& a, const f32vec4& b) {
        return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
    }

    f32 length(const f32vec2& v) {
        return sqrt(dot(v, v));
    }

    f32 length(const f32vec3& v) {
        return sqrt(dot(v, v));
    }

    f32 length(const f32vec4& v) {
        return sqrt(dot(v, v));
    }

    f32vec2 normalize(const f32vec2& v) {
        f32 len = length(v);
        A3D_ASSERT(len != 0.0f && "math::normalize called on zero-length f32vec2");
        return (len > 0.0f) ? v / len : v;
    }

    f32vec3 normalize(const f32vec3& v) {
        f32 len = length(v);
        A3D_ASSERT(len != 0.0f && "math::normalize called on zero-length f32vec3");
        return (len > 0.0f) ? v / len : v;
    }

    f32vec4 normalize(const f32vec4& v) {
        f32 len = length(v);
        A3D_ASSERT(len != 0.0f && "math::normalize called on zero-length f32vec4");
        return (len > 0.0f) ? v / len : v;
    }

    f32vec3 cross(const f32vec3& a, const f32vec3& b) {
        return f32vec3 {a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x};
    }

    std::string to_string(const f32vec2& v) {
        std::ostringstream ss;
        ss << "f32vec2(" << v.x << ", " << v.y << ")";
        return ss.str();
    }

    std::string to_string(const f32vec3& v) {
        std::ostringstream ss;
        ss << "f32vec3(" << v.x << ", " << v.y << ", " << v.z << ")";
        return ss.str();
    }

    std::string to_string(const f32vec4& v) {
        std::ostringstream ss;
        ss << "f32vec4(" << v.x << ", " << v.y << ", " << v.z << ", " << v.w << ")";
        return ss.str();
    }

    f32* value_ptr(f32vec2& v) {
        return &v.x;
    }

    const f32* value_ptr(const f32vec2& v) {
        return &v.x;
    }

    f32* value_ptr(f32vec3& v) {
        return &v.x;
    }

    const f32* value_ptr(const f32vec3& v) {
        return &v.x;
    }

    f32* value_ptr(f32vec4& v) {
        return &v.x;
    }

    const f32* value_ptr(const f32vec4& v) {
        return &v.x;
    }

    f32vec2 make_vec2(const f32* ptr) {
        return f32vec2 {ptr[0], ptr[1]};
    }

    f32vec3 make_vec3(const f32* ptr) {
        return f32vec3 {ptr[0], ptr[1], ptr[2]};
    }

    f32vec4 make_vec4(const f32* ptr) {
        return f32vec4 {ptr[0], ptr[1], ptr[2], ptr[3]};
    }

    /// Signed 32-bit Integer Vector ///

    i32vec2 operator-(const i32vec2& v) {
        return {-v.x, -v.y};
    }

    i32vec3 operator-(const i32vec3& v) {
        return {-v.x, -v.y, -v.z};
    }

    i32vec4 operator-(const i32vec4& v) {
        return {-v.x, -v.y, -v.z, -v.w};
    }

    i32vec2 operator+(const i32vec2& a, const i32vec2& b) {
        return i32vec2 {a.x + b.x, a.y + b.y};
    }

    i32vec3 operator+(const i32vec3& a, const i32vec3& b) {
        return i32vec3 {a.x + b.x, a.y + b.y, a.z + b.z};
    }

    i32vec4 operator+(const i32vec4& a, const i32vec4& b) {
        return i32vec4 {a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w};
    }

    i32vec2 operator-(const i32vec2& a, const i32vec2& b) {
        return i32vec2 {a.x - b.x, a.y - b.y};
    }

    i32vec3 operator-(const i32vec3& a, const i32vec3& b) {
        return i32vec3 {a.x - b.x, a.y - b.y, a.z - b.z};
    }

    i32vec4 operator-(const i32vec4& a, const i32vec4& b) {
        return i32vec4 {a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w};
    }

    i32vec2 operator*(const i32vec2& v, i32 s) {
        return i32vec2 {v.x * s, v.y * s};
    }

    i32vec3 operator*(const i32vec3& v, i32 s) {
        return i32vec3 {v.x * s, v.y * s, v.z * s};
    }

    i32vec4 operator*(const i32vec4& v, i32 s) {
        return i32vec4 {v.x * s, v.y * s, v.z * s, v.w * s};
    }

    i32vec2 operator*(i32 s, const i32vec2& v) {
        return v * s;
    }

    i32vec3 operator*(i32 s, const i32vec3& v) {
        return v * s;
    }

    i32vec4 operator*(i32 s, const i32vec4& v) {
        return v * s;
    }

    i32vec2 operator/(const i32vec2& v, i32 s) {
        // no zero check -- A3D_ASSERT?
        return i32vec2 {v.x / s, v.y / s};
    }

    i32vec3 operator/(const i32vec3& v, i32 s) {
        return i32vec3 {v.x / s, v.y / s, v.z / s};
    }

    i32vec4 operator/(const i32vec4& v, i32 s) {
        return i32vec4 {v.x / s, v.y / s, v.z / s, v.w / s};
    }

    i32vec2& operator+=(i32vec2& a, const i32vec2& b) {
        a.x += b.x;
        a.y += b.y;
        return a;
    }

    i32vec3& operator+=(i32vec3& a, const i32vec3& b) {
        a.x += b.x;
        a.y += b.y;
        a.z += b.z;
        return a;
    }

    i32vec4& operator+=(i32vec4& a, const i32vec4& b) {
        a.x += b.x;
        a.y += b.y;
        a.z += b.z;
        a.w += b.w;
        return a;
    }

    i32vec2& operator-=(i32vec2& a, const i32vec2& b) {
        a.x -= b.x;
        a.y -= b.y;
        return a;
    }

    i32vec3& operator-=(i32vec3& a, const i32vec3& b) {
        a.x -= b.x;
        a.y -= b.y;
        a.z -= b.z;
        return a;
    }

    i32vec4& operator-=(i32vec4& a, const i32vec4& b) {
        a.x -= b.x;
        a.y -= b.y;
        a.z -= b.z;
        a.w -= b.w;
        return a;
    }

    i32vec2& operator*=(i32vec2& v, i32 s) {
        v.x *= s;
        v.y *= s;
        return v;
    }

    i32vec3& operator*=(i32vec3& v, i32 s) {
        v.x *= s;
        v.y *= s;
        v.z *= s;
        return v;
    }

    i32vec4& operator*=(i32vec4& v, i32 s) {
        v.x *= s;
        v.y *= s;
        v.z *= s;
        v.w *= s;
        return v;
    }

    i32vec2& operator/=(i32vec2& v, i32 s) {
        v.x /= s;
        v.y /= s;
        return v;
    }

    i32vec3& operator/=(i32vec3& v, i32 s) {
        v.x /= s;
        v.y /= s;
        v.z /= s;
        return v;
    }

    i32vec4& operator/=(i32vec4& v, i32 s) {
        v.x /= s;
        v.y /= s;
        v.z /= s;
        v.w /= s;
        return v;
    }

    bool operator==(const i32vec2& a, const i32vec2& b) {
        return a.x == b.x && a.y == b.y;
    }

    bool operator==(const i32vec3& a, const i32vec3& b) {
        return a.x == b.x && a.y == b.y && a.z == b.z;
    }

    bool operator==(const i32vec4& a, const i32vec4& b) {
        return a.x == b.x && a.y == b.y && a.z == b.z && a.w == b.w;
    }

    bool operator!=(const i32vec2& a, const i32vec2& b) {
        return !(a == b);
    }

    bool operator!=(const i32vec3& a, const i32vec3& b) {
        return !(a == b);
    }

    bool operator!=(const i32vec4& a, const i32vec4& b) {
        return !(a == b);
    }

    f32 min(const i32vec2& v) {
        return math::min(v.x, v.y);
    }

    f32 min(const i32vec3& v) {
        return math::min(v.x, math::min(v.y, v.z));
    }

    f32 min(const i32vec4& v) {
        return math::min(math::min(v.x, v.y), math::min(v.z, v.w));
    }

    f32 max(const i32vec2& v) {
        return math::max(v.x, v.y);
    }

    f32 max(const i32vec3& v) {
        return math::max(v.x, math::max(v.y, v.z));
    }

    f32 max(const i32vec4& v) {
        return math::max(math::max(v.x, v.y), math::max(v.z, v.w));
    }

    i32vec2 min(const i32vec2& a, const i32vec2& b) {
        return {math::min(a.x, b.x), math::min(a.y, b.y)};
    }

    i32vec3 min(const i32vec3& a, const i32vec3& b) {
        return {math::min(a.x, b.x), math::min(a.y, b.y), math::min(a.z, b.z)};
    }

    i32vec4 min(const i32vec4& a, const i32vec4& b) {
        return {math::min(a.x, b.x), math::min(a.y, b.y), math::min(a.z, b.z), math::min(a.w, b.w)};
    }

    i32vec2 max(const i32vec2& a, const i32vec2& b) {
        return {math::max(a.x, b.x), math::max(a.y, b.y)};
    }

    i32vec3 max(const i32vec3& a, const i32vec3& b) {
        return {math::max(a.x, b.x), math::max(a.y, b.y), math::max(a.z, b.z)};
    }

    i32vec4 max(const i32vec4& a, const i32vec4& b) {
        return {math::max(a.x, b.x), math::max(a.y, b.y), math::max(a.z, b.z), math::max(a.w, b.w)};
    }

    i32 dot(const i32vec2& a, const i32vec2& b) {
        return a.x * b.x + a.y * b.y;
    }

    i32 dot(const i32vec3& a, const i32vec3& b) {
        return a.x * b.x + a.y * b.y + a.z * b.z;
    }

    i32 dot(const i32vec4& a, const i32vec4& b) {
        return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
    }

    std::string to_string(const i32vec2& v) {
        std::ostringstream ss;
        ss << "i32vec2(" << v.x << ", " << v.y << ")";
        return ss.str();
    }

    std::string to_string(const i32vec3& v) {
        std::ostringstream ss;
        ss << "i32vec3(" << v.x << ", " << v.y << ", " << v.z << ")";
        return ss.str();
    }

    std::string to_string(const i32vec4& v) {
        std::ostringstream ss;
        ss << "i32vec4(" << v.x << ", " << v.y << ", " << v.z << ", " << v.w << ")";
        return ss.str();
    }

    i32* value_ptr(i32vec2& v) {
        return &v.x;
    }

    const i32* value_ptr(const i32vec2& v) {
        return &v.x;
    }

    i32* value_ptr(i32vec3& v) {
        return &v.x;
    }

    const i32* value_ptr(const i32vec3& v) {
        return &v.x;
    }

    i32* value_ptr(i32vec4& v) {
        return &v.x;
    }

    const i32* value_ptr(const i32vec4& v) {
        return &v.x;
    }

    i32vec2 make_vec2(const i32* ptr) {
        return i32vec2 {ptr[0], ptr[1]};
    }

    i32vec3 make_vec3(const i32* ptr) {
        return i32vec3 {ptr[0], ptr[1], ptr[2]};
    }

    i32vec4 make_vec4(const i32* ptr) {
        return i32vec4 {ptr[0], ptr[1], ptr[2], ptr[3]};
    }

    /// Unsigned 32-bit Integer Vector ///

    u32vec2 operator+(const u32vec2& a, const u32vec2& b) {
        return u32vec2 {a.x + b.x, a.y + b.y};
    }

    u32vec3 operator+(const u32vec3& a, const u32vec3& b) {
        return u32vec3 {a.x + b.x, a.y + b.y, a.z + b.z};
    }

    u32vec4 operator+(const u32vec4& a, const u32vec4& b) {
        return u32vec4 {a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w};
    }

    u32vec2 operator-(const u32vec2& a, const u32vec2& b) {
        return u32vec2 {a.x - b.x, a.y - b.y};
    }

    u32vec3 operator-(const u32vec3& a, const u32vec3& b) {
        return u32vec3 {a.x - b.x, a.y - b.y, a.z - b.z};
    }

    u32vec4 operator-(const u32vec4& a, const u32vec4& b) {
        return u32vec4 {a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w};
    }

    u32vec2 operator*(const u32vec2& v, u32 s) {
        return u32vec2 {v.x * s, v.y * s};
    }

    u32vec3 operator*(const u32vec3& v, u32 s) {
        return u32vec3 {v.x * s, v.y * s, v.z * s};
    }

    u32vec4 operator*(const u32vec4& v, u32 s) {
        return u32vec4 {v.x * s, v.y * s, v.z * s, v.w * s};
    }

    u32vec2 operator*(u32 s, const u32vec2& v) {
        return v * s;
    }

    u32vec3 operator*(u32 s, const u32vec3& v) {
        return v * s;
    }

    u32vec4 operator*(u32 s, const u32vec4& v) {
        return v * s;
    }

    u32vec2 operator/(const u32vec2& v, u32 s) {
        // no zero check -- A3D_ASSERT?
        return u32vec2 {v.x / s, v.y / s};
    }

    u32vec3 operator/(const u32vec3& v, u32 s) {
        return u32vec3 {v.x / s, v.y / s, v.z / s};
    }

    u32vec4 operator/(const u32vec4& v, u32 s) {
        return u32vec4 {v.x / s, v.y / s, v.z / s, v.w / s};
    }

    u32vec2& operator+=(u32vec2& a, const u32vec2& b) {
        a.x += b.x;
        a.y += b.y;
        return a;
    }

    u32vec3& operator+=(u32vec3& a, const u32vec3& b) {
        a.x += b.x;
        a.y += b.y;
        a.z += b.z;
        return a;
    }

    u32vec4& operator+=(u32vec4& a, const u32vec4& b) {
        a.x += b.x;
        a.y += b.y;
        a.z += b.z;
        a.w += b.w;
        return a;
    }

    u32vec2& operator-=(u32vec2& a, const u32vec2& b) {
        a.x -= b.x;
        a.y -= b.y;
        return a;
    }

    u32vec3& operator-=(u32vec3& a, const u32vec3& b) {
        a.x -= b.x;
        a.y -= b.y;
        a.z -= b.z;
        return a;
    }

    u32vec4& operator-=(u32vec4& a, const u32vec4& b) {
        a.x -= b.x;
        a.y -= b.y;
        a.z -= b.z;
        a.w -= b.w;
        return a;
    }

    u32vec2& operator*=(u32vec2& v, u32 s) {
        v.x *= s;
        v.y *= s;
        return v;
    }

    u32vec3& operator*=(u32vec3& v, u32 s) {
        v.x *= s;
        v.y *= s;
        v.z *= s;
        return v;
    }

    u32vec4& operator*=(u32vec4& v, u32 s) {
        v.x *= s;
        v.y *= s;
        v.z *= s;
        v.w *= s;
        return v;
    }

    u32vec2& operator/=(u32vec2& v, u32 s) {
        v.x /= s;
        v.y /= s;
        return v;
    }

    u32vec3& operator/=(u32vec3& v, u32 s) {
        v.x /= s;
        v.y /= s;
        v.z /= s;
        return v;
    }

    u32vec4& operator/=(u32vec4& v, u32 s) {
        v.x /= s;
        v.y /= s;
        v.z /= s;
        v.w /= s;
        return v;
    }

    bool operator==(const u32vec2& a, const u32vec2& b) {
        return a.x == b.x && a.y == b.y;
    }

    bool operator==(const u32vec3& a, const u32vec3& b) {
        return a.x == b.x && a.y == b.y && a.z == b.z;
    }

    bool operator==(const u32vec4& a, const u32vec4& b) {
        return a.x == b.x && a.y == b.y && a.z == b.z && a.w == b.w;
    }

    bool operator!=(const u32vec2& a, const u32vec2& b) {
        return !(a == b);
    }

    bool operator!=(const u32vec3& a, const u32vec3& b) {
        return !(a == b);
    }

    bool operator!=(const u32vec4& a, const u32vec4& b) {
        return !(a == b);
    }

    f32 min(const u32vec2& v) {
        return math::min(v.x, v.y);
    }

    f32 min(const u32vec3& v) {
        return math::min(v.x, math::min(v.y, v.z));
    }

    f32 min(const u32vec4& v) {
        return math::min(math::min(v.x, v.y), math::min(v.z, v.w));
    }

    f32 max(const u32vec2& v) {
        return math::max(v.x, v.y);
    }

    f32 max(const u32vec3& v) {
        return math::max(v.x, math::max(v.y, v.z));
    }

    f32 max(const u32vec4& v) {
        return math::max(math::max(v.x, v.y), math::max(v.z, v.w));
    }

    u32vec2 min(const u32vec2& a, const u32vec2& b) {
        return {math::min(a.x, b.x), math::min(a.y, b.y)};
    }

    u32vec3 min(const u32vec3& a, const u32vec3& b) {
        return {math::min(a.x, b.x), math::min(a.y, b.y), math::min(a.z, b.z)};
    }

    u32vec4 min(const u32vec4& a, const u32vec4& b) {
        return {math::min(a.x, b.x), math::min(a.y, b.y), math::min(a.z, b.z), math::min(a.w, b.w)};
    }

    u32vec2 max(const u32vec2& a, const u32vec2& b) {
        return {math::max(a.x, b.x), math::max(a.y, b.y)};
    }

    u32vec3 max(const u32vec3& a, const u32vec3& b) {
        return {math::max(a.x, b.x), math::max(a.y, b.y), math::max(a.z, b.z)};
    }

    u32vec4 max(const u32vec4& a, const u32vec4& b) {
        return {math::max(a.x, b.x), math::max(a.y, b.y), math::max(a.z, b.z), math::max(a.w, b.w)};
    }

    u32 dot(const u32vec2& a, const u32vec2& b) {
        return a.x * b.x + a.y * b.y;
    }

    u32 dot(const u32vec3& a, const u32vec3& b) {
        return a.x * b.x + a.y * b.y + a.z * b.z;
    }

    u32 dot(const u32vec4& a, const u32vec4& b) {
        return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
    }

    std::string to_string(const u32vec2& v) {
        std::ostringstream ss;
        ss << "u32vec2(" << v.x << ", " << v.y << ")";
        return ss.str();
    }

    std::string to_string(const u32vec3& v) {
        std::ostringstream ss;
        ss << "u32vec3(" << v.x << ", " << v.y << ", " << v.z << ")";
        return ss.str();
    }

    std::string to_string(const u32vec4& v) {
        std::ostringstream ss;
        ss << "u32vec4(" << v.x << ", " << v.y << ", " << v.z << ", " << v.w << ")";
        return ss.str();
    }

    u32* value_ptr(u32vec2& v) {
        return &v.x;
    }

    const u32* value_ptr(const u32vec2& v) {
        return &v.x;
    }

    u32* value_ptr(u32vec3& v) {
        return &v.x;
    }

    const u32* value_ptr(const u32vec3& v) {
        return &v.x;
    }

    u32* value_ptr(u32vec4& v) {
        return &v.x;
    }

    const u32* value_ptr(const u32vec4& v) {
        return &v.x;
    }

    u32vec2 make_vec2(const u32* ptr) {
        return u32vec2 {ptr[0], ptr[1]};
    }

    u32vec3 make_vec3(const u32* ptr) {
        return u32vec3 {ptr[0], ptr[1], ptr[2]};
    }

    u32vec4 make_vec4(const u32* ptr) {
        return u32vec4 {ptr[0], ptr[1], ptr[2], ptr[3]};
    }

    /// Unsigned 8-bit Integer Vector ///

    u8vec2 operator+(const u8vec2& a, const u8vec2& b) {
        return u8vec2 {static_cast<u8>(a.x + b.x), static_cast<u8>(a.y + b.y)};
    }

    u8vec3 operator+(const u8vec3& a, const u8vec3& b) {
        return u8vec3 {static_cast<u8>(a.x + b.x), static_cast<u8>(a.y + b.y), static_cast<u8>(a.z + b.z)};
    }

    u8vec4 operator+(const u8vec4& a, const u8vec4& b) {
        return u8vec4 {static_cast<u8>(a.x + b.x), static_cast<u8>(a.y + b.y), static_cast<u8>(a.z + b.z),
                       static_cast<u8>(a.w + b.w)};
    }

    u8vec2 operator-(const u8vec2& a, const u8vec2& b) {
        return u8vec2 {static_cast<u8>(a.x - b.x), static_cast<u8>(a.y - b.y)};
    }

    u8vec3 operator-(const u8vec3& a, const u8vec3& b) {
        return u8vec3 {static_cast<u8>(a.x - b.x), static_cast<u8>(a.y - b.y), static_cast<u8>(a.z - b.z)};
    }

    u8vec4 operator-(const u8vec4& a, const u8vec4& b) {
        return u8vec4 {static_cast<u8>(a.x - b.x), static_cast<u8>(a.y - b.y), static_cast<u8>(a.z - b.z),
                       static_cast<u8>(a.w - b.w)};
    }

    u8vec2 operator*(const u8vec2& v, f32 s) {
        return u8vec2 {saturate_u8(v.x * s), saturate_u8(v.y * s)};
    }

    u8vec3 operator*(const u8vec3& v, f32 s) {
        return u8vec3 {saturate_u8(v.x * s), saturate_u8(v.y * s), saturate_u8(v.z * s)};
    }

    u8vec4 operator*(const u8vec4& v, f32 s) {
        return u8vec4 {saturate_u8(v.x * s), saturate_u8(v.y * s), saturate_u8(v.z * s), saturate_u8(v.w * s)};
    }

    u8vec2 operator*(f32 s, const u8vec2& v) {
        return v * s;
    }

    u8vec3 operator*(f32 s, const u8vec3& v) {
        return v * s;
    }

    u8vec4 operator*(f32 s, const u8vec4& v) {
        return v * s;
    }

    u8vec2 operator/(const u8vec2& v, f32 s) {
        if (s == 0.0f) {
            return u8vec2 {0};
        }
        f32 inv = 1.0f / s;
        return u8vec2 {saturate_u8(v.x * inv), saturate_u8(v.y * inv)};
    }

    u8vec3 operator/(const u8vec3& v, f32 s) {
        if (s == 0.0f) {
            return u8vec3 {0};
        }
        f32 inv = 1.0f / s;
        return u8vec3 {saturate_u8(v.x * inv), saturate_u8(v.y * inv), saturate_u8(v.z * inv)};
    }

    u8vec4 operator/(const u8vec4& v, f32 s) {
        if (s == 0.0f) {
            return u8vec4 {0};
        }
        f32 inv = 1.0f / s;
        return u8vec4 {saturate_u8(v.x * inv), saturate_u8(v.y * inv), saturate_u8(v.z * inv),
                       saturate_u8(v.w * inv)};
    }

    u8vec2& operator+=(u8vec2& a, const u8vec2& b) {
        a.x = static_cast<u8>(a.x + b.x);
        a.y = static_cast<u8>(a.y + b.y);
        return a;
    }

    u8vec3& operator+=(u8vec3& a, const u8vec3& b) {
        a.x = static_cast<u8>(a.x + b.x);
        a.y = static_cast<u8>(a.y + b.y);
        a.z = static_cast<u8>(a.z + b.z);
        return a;
    }

    u8vec4& operator+=(u8vec4& a, const u8vec4& b) {
        a.x = static_cast<u8>(a.x + b.x);
        a.y = static_cast<u8>(a.y + b.y);
        a.z = static_cast<u8>(a.z + b.z);
        a.w = static_cast<u8>(a.w + b.w);
        return a;
    }

    u8vec2& operator-=(u8vec2& a, const u8vec2& b) {
        a.x = static_cast<u8>(a.x - b.x);
        a.y = static_cast<u8>(a.y - b.y);
        return a;
    }

    u8vec3& operator-=(u8vec3& a, const u8vec3& b) {
        a.x = static_cast<u8>(a.x - b.x);
        a.y = static_cast<u8>(a.y - b.y);
        a.z = static_cast<u8>(a.z - b.z);
        return a;
    }

    u8vec4& operator-=(u8vec4& a, const u8vec4& b) {
        a.x = static_cast<u8>(a.x - b.x);
        a.y = static_cast<u8>(a.y - b.y);
        a.z = static_cast<u8>(a.z - b.z);
        a.w = static_cast<u8>(a.w - b.w);
        return a;
    }

    u8vec2& operator*=(u8vec2& v, f32 s) {
        v = v * s;
        return v;
    }

    u8vec3& operator*=(u8vec3& v, f32 s) {
        v = v * s;
        return v;
    }

    u8vec4& operator*=(u8vec4& v, f32 s) {
        v = v * s;
        return v;
    }

    u8vec2& operator/=(u8vec2& v, f32 s) {
        v = v / s;
        return v;
    }

    u8vec3& operator/=(u8vec3& v, f32 s) {
        v = v / s;
        return v;
    }

    u8vec4& operator/=(u8vec4& v, f32 s) {
        v = v / s;
        return v;
    }

    bool operator==(const u8vec2& a, const u8vec2& b) {
        return a.x == b.x && a.y == b.y;
    }

    bool operator==(const u8vec3& a, const u8vec3& b) {
        return a.x == b.x && a.y == b.y && a.z == b.z;
    }

    bool operator==(const u8vec4& a, const u8vec4& b) {
        return a.x == b.x && a.y == b.y && a.z == b.z && a.w == b.w;
    }

    bool operator!=(const u8vec2& a, const u8vec2& b) {
        return !(a == b);
    }

    bool operator!=(const u8vec3& a, const u8vec3& b) {
        return !(a == b);
    }

    bool operator!=(const u8vec4& a, const u8vec4& b) {
        return !(a == b);
    }

    f32 min(const u8vec2& v) {
        return math::min(v.x, v.y);
    }

    f32 min(const u8vec3& v) {
        return math::min(v.x, math::min(v.y, v.z));
    }

    f32 min(const u8vec4& v) {
        return math::min(math::min(v.x, v.y), math::min(v.z, v.w));
    }

    f32 max(const u8vec2& v) {
        return math::max(v.x, v.y);
    }

    f32 max(const u8vec3& v) {
        return math::max(v.x, math::max(v.y, v.z));
    }

    f32 max(const u8vec4& v) {
        return math::max(math::max(v.x, v.y), math::max(v.z, v.w));
    }

    u8vec2 min(const u8vec2& a, const u8vec2& b) {
        return {math::min(a.x, b.x), math::min(a.y, b.y)};
    }

    u8vec3 min(const u8vec3& a, const u8vec3& b) {
        return {math::min(a.x, b.x), math::min(a.y, b.y), math::min(a.z, b.z)};
    }

    u8vec4 min(const u8vec4& a, const u8vec4& b) {
        return {math::min(a.x, b.x), math::min(a.y, b.y), math::min(a.z, b.z), math::min(a.w, b.w)};
    }

    u8vec2 max(const u8vec2& a, const u8vec2& b) {
        return {math::max(a.x, b.x), math::max(a.y, b.y)};
    }

    u8vec3 max(const u8vec3& a, const u8vec3& b) {
        return {math::max(a.x, b.x), math::max(a.y, b.y), math::max(a.z, b.z)};
    }

    u8vec4 max(const u8vec4& a, const u8vec4& b) {
        return {math::max(a.x, b.x), math::max(a.y, b.y), math::max(a.z, b.z), math::max(a.w, b.w)};
    }

    u8* value_ptr(u8vec2& v) {
        return &v.x;
    }

    const u8* value_ptr(const u8vec2& v) {
        return &v.x;
    }

    u8* value_ptr(u8vec3& v) {
        return &v.x;
    }

    const u8* value_ptr(const u8vec3& v) {
        return &v.x;
    }

    u8* value_ptr(u8vec4& v) {
        return &v.x;
    }

    const u8* value_ptr(const u8vec4& v) {
        return &v.x;
    }

    u8vec2 make_vec2(const u8* ptr) {
        return u8vec2 {ptr[0], ptr[1]};
    }

    u8vec3 make_vec3(const u8* ptr) {
        return u8vec3 {ptr[0], ptr[1], ptr[2]};
    }

    u8vec4 make_vec4(const u8* ptr) {
        return u8vec4 {ptr[0], ptr[1], ptr[2], ptr[3]};
    }

    std::string to_string(const u8vec2& v) {
        std::ostringstream ss;
        ss << "u8vec2(" << static_cast<unsigned>(v.x) << ", " << static_cast<unsigned>(v.y) << ")";
        return ss.str();
    }

    std::string to_string(const u8vec3& v) {
        std::ostringstream ss;
        ss << "u8vec3(" << static_cast<unsigned>(v.x) << ", " << static_cast<unsigned>(v.y) << ", "
           << static_cast<unsigned>(v.z) << ")";
        return ss.str();
    }

    std::string to_string(const u8vec4& v) {
        std::ostringstream ss;
        ss << "u8vec4(" << static_cast<unsigned>(v.x) << ", " << static_cast<unsigned>(v.y) << ", "
           << static_cast<unsigned>(v.z) << ", " << static_cast<unsigned>(v.w) << ")";
        return ss.str();
    }

    /// 32-bit Float Matrix ///

    f32mat2 operator-(const f32mat2& m) {
        // result.ci = -m.ci
        return f32mat2 {f32vec2 {-m.c0.x, -m.c0.y}, f32vec2 {-m.c1.x, -m.c1.y}};
    }

    f32mat3 operator-(const f32mat3& m) {
        return f32mat3 {f32vec3 {-m.c0.x, -m.c0.y, -m.c0.z}, f32vec3 {-m.c1.x, -m.c1.y, -m.c1.z},
                        f32vec3 {-m.c2.x, -m.c2.y, -m.c2.z}};
    }

    f32mat4 operator-(const f32mat4& m) {
        return f32mat4 {f32vec4 {-m.c0.x, -m.c0.y, -m.c0.z, -m.c0.w},
                        f32vec4 {-m.c1.x, -m.c1.y, -m.c1.z, -m.c1.w},
                        f32vec4 {-m.c2.x, -m.c2.y, -m.c2.z, -m.c2.w},
                        f32vec4 {-m.c3.x, -m.c3.y, -m.c3.z, -m.c3.w}};
    }

    f32vec2 operator*(const f32mat2& m, const f32vec2& v) {
        return f32vec2 {m.c0.x * v.x + m.c1.x * v.y, m.c0.y * v.x + m.c1.y * v.y};
    }

    f32vec3 operator*(const f32mat3& m, const f32vec3& v) {
        return f32vec3 {m.c0.x * v.x + m.c1.x * v.y + m.c2.x * v.z, m.c0.y * v.x + m.c1.y * v.y + m.c2.y * v.z,
                        m.c0.z * v.x + m.c1.z * v.y + m.c2.z * v.z};
    }

    f32vec4 operator*(const f32mat4& m, const f32vec4& v) {
        return f32vec4 {m.c0.x * v.x + m.c1.x * v.y + m.c2.x * v.z + m.c3.x * v.w,
                        m.c0.y * v.x + m.c1.y * v.y + m.c2.y * v.z + m.c3.y * v.w,
                        m.c0.z * v.x + m.c1.z * v.y + m.c2.z * v.z + m.c3.z * v.w,
                        m.c0.w * v.x + m.c1.w * v.y + m.c2.w * v.z + m.c3.w * v.w};
    }

    f32mat2 operator*(const f32mat2& a, const f32mat2& b) {
        f32mat2 r(0.0f);
        r.c0 = a * b.c0;
        r.c1 = a * b.c1;
        return r;
    }

    f32mat3 operator*(const f32mat3& a, const f32mat3& b) {
        f32mat3 r(0.0f);
        r.c0 = a * b.c0;
        r.c1 = a * b.c1;
        r.c2 = a * b.c2;
        return r;
    }

    f32mat4 operator*(const f32mat4& a, const f32mat4& b) {
        f32mat4 r(0.0f);
        r.c0 = a * b.c0;
        r.c1 = a * b.c1;
        r.c2 = a * b.c2;
        r.c3 = a * b.c3;
        return r;
    }

    f32mat2& operator*=(f32mat2& a, const f32mat2& b) {
        a = a * b;
        return a;
    }

    f32mat3& operator*=(f32mat3& a, const f32mat3& b) {
        a = a * b;
        return a;
    }

    f32mat4& operator*=(f32mat4& a, const f32mat4& b) {
        a = a * b;
        return a;
    }

    bool operator==(const f32mat2& a, const f32mat2& b) {
        return a.c0 == b.c0 && a.c1 == b.c1;
    }

    bool operator==(const f32mat3& a, const f32mat3& b) {
        return a.c0 == b.c0 && a.c1 == b.c1 && a.c2 == b.c2;
    }

    bool operator==(const f32mat4& a, const f32mat4& b) {
        return a.c0 == b.c0 && a.c1 == b.c1 && a.c2 == b.c2 && a.c3 == b.c3;
    }

    bool operator!=(const f32mat2& a, const f32mat2& b) {
        return !(a == b);
    }

    bool operator!=(const f32mat3& a, const f32mat3& b) {
        return !(a == b);
    }

    bool operator!=(const f32mat4& a, const f32mat4& b) {
        return !(a == b);
    }

    f32mat2 abs(const f32mat2& m) {
        using math::abs;
        return f32mat2 {f32vec2 {abs(m[0][0]), abs(m[0][1])}, f32vec2 {abs(m[1][0]), abs(m[1][1])}};
    }

    f32mat3 abs(const f32mat3& m) {
        using math::abs;
        return f32mat3 {f32vec3 {abs(m[0][0]), abs(m[0][1]), abs(m[0][2])},
                        f32vec3 {abs(m[1][0]), abs(m[1][1]), abs(m[1][2])},
                        f32vec3 {abs(m[2][0]), abs(m[2][1]), abs(m[2][2])}};
    }

    f32mat4 abs(const f32mat4& m) {
        using math::abs;
        return f32mat4 {f32vec4 {abs(m[0][0]), abs(m[0][1]), abs(m[0][2]), abs(m[0][3])},
                        f32vec4 {abs(m[1][0]), abs(m[1][1]), abs(m[1][2]), abs(m[1][3])},
                        f32vec4 {abs(m[2][0]), abs(m[2][1]), abs(m[2][2]), abs(m[2][3])},
                        f32vec4 {abs(m[3][0]), abs(m[3][1]), abs(m[3][2]), abs(m[3][3])}};
    }

    f32mat2 transpose(const f32mat2& m) {
        f32mat2 r;
        r.c0 = vec2 {m.c0.x, m.c1.x};
        r.c1 = vec2 {m.c0.y, m.c1.y};
        return r;
    }

    f32mat3 transpose(const f32mat3& m) {
        f32mat3 r;
        r.c0 = vec3 {m.c0.x, m.c1.x, m.c2.x};
        r.c1 = vec3 {m.c0.y, m.c1.y, m.c2.y};
        r.c2 = vec3 {m.c0.z, m.c1.z, m.c2.z};
        return r;
    }

    f32mat4 transpose(const f32mat4& m) {
        f32mat4 r;
        r.c0 = vec4 {m.c0.x, m.c1.x, m.c2.x, m.c3.x};
        r.c1 = vec4 {m.c0.y, m.c1.y, m.c2.y, m.c3.y};
        r.c2 = vec4 {m.c0.z, m.c1.z, m.c2.z, m.c3.z};
        r.c3 = vec4 {m.c0.w, m.c1.w, m.c2.w, m.c3.w};
        return r;
    }

    f32 determinant(const f32mat2& m) {
        return m[0][0] * m[1][1] - m[1][0] * m[0][1];
        ;
    }

    f32 determinant(const f32mat3& m) {
        return +m[0][0] * (m[1][1] * m[2][2] - m[2][1] * m[1][2])
               - m[1][0] * (m[0][1] * m[2][2] - m[2][1] * m[0][2])
               + m[2][0] * (m[0][1] * m[1][2] - m[1][1] * m[0][2]);
    }

    f32 determinant(const f32mat4& m) {
        f32 subFactor00 = m[2][2] * m[3][3] - m[3][2] * m[2][3];
        f32 subFactor01 = m[2][1] * m[3][3] - m[3][1] * m[2][3];
        f32 subFactor02 = m[2][1] * m[3][2] - m[3][1] * m[2][2];
        f32 subFactor03 = m[2][0] * m[3][3] - m[3][0] * m[2][3];
        f32 subFactor04 = m[2][0] * m[3][2] - m[3][0] * m[2][2];
        f32 subFactor05 = m[2][0] * m[3][1] - m[3][0] * m[2][1];

        vec4 detCof(+(m[1][1] * subFactor00 - m[1][2] * subFactor01 + m[1][3] * subFactor02),
                    -(m[1][0] * subFactor00 - m[1][2] * subFactor03 + m[1][3] * subFactor04),
                    +(m[1][0] * subFactor01 - m[1][1] * subFactor03 + m[1][3] * subFactor05),
                    -(m[1][0] * subFactor02 - m[1][1] * subFactor04 + m[1][2] * subFactor05));

        return m[0][0] * detCof[0] + m[0][1] * detCof[1] + m[0][2] * detCof[2] + m[0][3] * detCof[3];
    }

    f32mat2 inverse(const f32mat2& m) {
        f32 a = m.c0.x, c = m.c0.y;
        f32 b = m.c1.x, d = m.c1.y;

        f32 det = a * d - b * c;
        A3D_ASSERT(det != 0.0f);
        f32 invDet = 1.0f / det;

        f32mat2 r;
        r.c0 = f32vec2 {d * invDet, -c * invDet};
        r.c1 = f32vec2 {-b * invDet, a * invDet};
        return r;
    }

    f32mat3 inverse(const f32mat3& m) {
        f32 m00 = m.c0.x, m01 = m.c1.x, m02 = m.c2.x;
        f32 m10 = m.c0.y, m11 = m.c1.y, m12 = m.c2.y;
        f32 m20 = m.c0.z, m21 = m.c1.z, m22 = m.c2.z;

        f32 inv00 = (m11 * m22 - m12 * m21);
        f32 inv01 = -(m01 * m22 - m02 * m21);
        f32 inv02 = (m01 * m12 - m02 * m11);

        f32 inv10 = -(m10 * m22 - m12 * m20);
        f32 inv11 = (m00 * m22 - m02 * m20);
        f32 inv12 = -(m00 * m12 - m02 * m10);

        f32 inv20 = (m10 * m21 - m11 * m20);
        f32 inv21 = -(m00 * m21 - m01 * m20);
        f32 inv22 = (m00 * m11 - m01 * m10);

        f32 det = m00 * inv00 + m01 * inv10 + m02 * inv20;
        A3D_ASSERT(det != 0.0f);
        f32 invDet = 1.0f / det;

        f32mat3 r;
        r.c0 = f32vec3 {inv00 * invDet, inv10 * invDet, inv20 * invDet};
        r.c1 = f32vec3 {inv01 * invDet, inv11 * invDet, inv21 * invDet};
        r.c2 = f32vec3 {inv02 * invDet, inv12 * invDet, inv22 * invDet};
        return r;
    }

    f32mat4 inverse(const f32mat4& m) {
        // row-major copy of m
        f32 a[4][4] = {{m.c0.x, m.c1.x, m.c2.x, m.c3.x}, // row 0
                       {m.c0.y, m.c1.y, m.c2.y, m.c3.y}, // row 1
                       {m.c0.z, m.c1.z, m.c2.z, m.c3.z}, // row 2
                       {m.c0.w, m.c1.w, m.c2.w, m.c3.w}}; // row 3

        f32 inv[4][4] = {{1.0f, 0.0f, 0.0f, 0.0f},
                         {0.0f, 1.0f, 0.0f, 0.0f},
                         {0.0f, 0.0f, 1.0f, 0.0f},
                         {0.0f, 0.0f, 0.0f, 1.0f}};

        // Gauss-Jordan elimination with partial pivoting
        for (int col = 0; col < 4; ++col) {
            // find pivot row
            int pivot_row = col;
            f32 max_abs = math::abs(a[col][col]);
            for (int r = col + 1; r < 4; ++r) {
                f32 val = math::abs(a[r][col]);
                if (val > max_abs) {
                    max_abs = val;
                    pivot_row = r;
                }
            }

            const f32 eps = 1e-8f;
            A3D_ASSERT(max_abs > eps && "f32mat4 inverse: f32matrix is singular or ill-conditioned");
            // or return f32mat4(1.0f);

            // swap rows in both 'a' and 'inv'
            if (pivot_row != col) {
                math::swap(a[col], a[pivot_row]); // TODO: wrap with a3d::math
                math::swap(inv[col], inv[pivot_row]); // TODO: wrap with a3d::math
            }

            // normalize pivot row
            const f32 pivot = a[col][col];
            const f32 inv_pivot = 1.0f / pivot;
            for (int j = 0; j < 4; ++j) {
                a[col][j] *= inv_pivot;
                inv[col][j] *= inv_pivot;
            }

            // eliminate this column from other rows
            for (int r = 0; r < 4; ++r) {
                if (r == col) {
                    continue;
                }
                const f32 factor = a[r][col];
                const f32 eps = 1e-12f; // can be smaller here
                if (math::abs(factor) < eps) {
                    continue;
                }
                for (int j = 0; j < 4; ++j) {
                    a[r][j] -= factor * a[col][j];
                    inv[r][j] -= factor * inv[col][j];
                }
            }
        }

        // 'inv' is now row-major inverse; convert back to column-major f32mat4
        f32mat4 r(0.0);
        r.c0 = f32vec4 {inv[0][0], inv[1][0], inv[2][0], inv[3][0]};
        r.c1 = f32vec4 {inv[0][1], inv[1][1], inv[2][1], inv[3][1]};
        r.c2 = f32vec4 {inv[0][2], inv[1][2], inv[2][2], inv[3][2]};
        r.c3 = f32vec4 {inv[0][3], inv[1][3], inv[2][3], inv[3][3]};
        return r;
    }

    f32vec3 translation(const f32mat4& m) {
        return f32vec3(m.c3);
    }

    f32mat4 translate(const f32mat4& m, const f32vec3& v) {
        f32mat4 result(m);
        result[3] = m[0] * v[0] + m[1] * v[1] + m[2] * v[2] + m[3];
        return result;
    }

    f32mat4 rotate(const f32mat4& m, f32 angle, const f32vec3& v) {
        const f32 a = angle;
        const f32 c = cos(a);
        const f32 s = sin(a);

        f32vec3 axis(normalize(v));
        f32vec3 temp((f32(1) - c) * axis);

        f32mat4 rotate;
        rotate[0][0] = c + temp[0] * axis[0];
        rotate[0][1] = temp[0] * axis[1] + s * axis[2];
        rotate[0][2] = temp[0] * axis[2] - s * axis[1];

        rotate[1][0] = temp[1] * axis[0] - s * axis[2];
        rotate[1][1] = c + temp[1] * axis[1];
        rotate[1][2] = temp[1] * axis[2] + s * axis[0];

        rotate[2][0] = temp[2] * axis[0] + s * axis[1];
        rotate[2][1] = temp[2] * axis[1] - s * axis[0];
        rotate[2][2] = c + temp[2] * axis[2];

        f32mat4 result;
        result[0] = m[0] * rotate[0][0] + m[1] * rotate[0][1] + m[2] * rotate[0][2];
        result[1] = m[0] * rotate[1][0] + m[1] * rotate[1][1] + m[2] * rotate[1][2];
        result[2] = m[0] * rotate[2][0] + m[1] * rotate[2][1] + m[2] * rotate[2][2];
        result[3] = m[3];
        return result;
    }

    f32mat4 scale(const f32mat4& m, const f32vec3& v) {
        f32mat4 result;
        result[0] = m[0] * v[0];
        result[1] = m[1] * v[1];
        result[2] = m[2] * v[2];
        result[3] = m[3];
        return result;
    }

    f32mat4 scale(const f32mat4& m, f32 s) {
        return scale(m, vec3 {s, s, s});
    }

//	std::string to_string(const f32mat2& m) {
//		std::ostringstream ss;
//		ss << "f32mat2("
//		   << "c0=(" << m.c0.x << ", " << m.c0.y << "), "
//		   << "c1=(" << m.c1.x << ", " << m.c1.y << "))";
//		return ss.str();
//	}
//
//	std::string to_string(const f32mat3& m) {
//		std::ostringstream ss;
//		ss << "f32mat3("
//		   << "c0=(" << m.c0.x << ", " << m.c0.y << ", " << m.c0.z << "), "
//		   << "c1=(" << m.c1.x << ", " << m.c1.y << ", " << m.c1.z << "), "
//		   << "c2=(" << m.c2.x << ", " << m.c2.y << ", " << m.c2.z << "))";
//		return ss.str();
//	}
//
//	std::string to_string(const f32mat4& m) {
//		std::ostringstream ss;
//		ss << "f32mat4("
//		   << "c0=(" << m.c0.x << ", " << m.c0.y << ", " << m.c0.z << ", " << m.c0.w << "), "
//		   << "c1=(" << m.c1.x << ", " << m.c1.y << ", " << m.c1.z << ", " << m.c1.w << "), "
//		   << "c2=(" << m.c2.x << ", " << m.c2.y << ", " << m.c2.z << ", " << m.c2.w << "), "
//		   << "c3=(" << m.c3.x << ", " << m.c3.y << ", " << m.c3.z << ", " << m.c3.w << "))";
//		return ss.str();
//	}

    std::string to_string(const f32mat2& m, unsigned pad) {
        std::ostringstream ss;
        ss << "f32mat2(\n"
           << "  (" << mat_fmtf(m.c0.x, pad) << ", " << mat_fmtf(m.c1.x, pad) << ")\n"
           << "  (" << mat_fmtf(m.c0.y, pad) << ", " << mat_fmtf(m.c1.y, pad) << ")\n"
           << ")";
        return ss.str();
    }

    std::string to_string(const f32mat3& m, unsigned pad) {
        std::ostringstream ss;
        ss << "f32mat3(\n"
           << "  (" << mat_fmtf(m.c0.x, pad) << ", " << mat_fmtf(m.c1.x, pad) << ", " << mat_fmtf(m.c2.x, pad)
           << ")\n"
           << "  (" << mat_fmtf(m.c0.y, pad) << ", " << mat_fmtf(m.c1.y, pad) << ", " << mat_fmtf(m.c2.y, pad)
           << ")\n"
           << "  (" << mat_fmtf(m.c0.z, pad) << ", " << mat_fmtf(m.c1.z, pad) << ", " << mat_fmtf(m.c2.z, pad)
           << ")\n"
           << ")";
        return ss.str();
    }

    std::string to_string(const f32mat4& m, unsigned pad) {
        std::ostringstream ss;
        ss << "f32mat4(\n"
           << "  (" << mat_fmtf(m.c0.x, pad) << ", " << mat_fmtf(m.c1.x, pad) << ", " << mat_fmtf(m.c2.x, pad)
           << ", " << mat_fmtf(m.c3.x, pad) << ")\n"
           << "  (" << mat_fmtf(m.c0.y, pad) << ", " << mat_fmtf(m.c1.y, pad) << ", " << mat_fmtf(m.c2.y, pad)
           << ", " << mat_fmtf(m.c3.y, pad) << ")\n"
           << "  (" << mat_fmtf(m.c0.z, pad) << ", " << mat_fmtf(m.c1.z, pad) << ", " << mat_fmtf(m.c2.z, pad)
           << ", " << mat_fmtf(m.c3.z, pad) << ")\n"
           << "  (" << mat_fmtf(m.c0.w, pad) << ", " << mat_fmtf(m.c1.w, pad) << ", " << mat_fmtf(m.c2.w, pad)
           << ", " << mat_fmtf(m.c3.w, pad) << ")\n"
           << ")";
        return ss.str();
    }

    f32* value_ptr(f32mat2& m) {
        return &m.c0.x;
    }

    const f32* value_ptr(const f32mat2& m) {
        return &m.c0.x;
    }

    f32* value_ptr(f32mat3& m) {
        return &m.c0.x;
    }

    const f32* value_ptr(const f32mat3& m) {
        return &m.c0.x;
    }

    f32* value_ptr(f32mat4& m) {
        return &m.c0.x;
    }

    const f32* value_ptr(const f32mat4& m) {
        return &m.c0.x;
    }

    f32mat2 make_mat2(const f32* ptr) {
        f32mat2 m;
        m.c0 = vec2 {ptr[0], ptr[1]};
        m.c1 = vec2 {ptr[2], ptr[3]};
        return m;
    }

    f32mat3 make_mat3(const f32* ptr) {
        f32mat3 m;
        m.c0 = vec3 {ptr[0], ptr[1], ptr[2]};
        m.c1 = vec3 {ptr[3], ptr[4], ptr[5]};
        m.c2 = vec3 {ptr[6], ptr[7], ptr[8]};
        return m;
    }

    f32mat4 make_mat4(const f32* ptr) {
        f32mat4 m;
        m.c0 = vec4 {ptr[0], ptr[1], ptr[2], ptr[3]};
        m.c1 = vec4 {ptr[4], ptr[5], ptr[6], ptr[7]};
        m.c2 = vec4 {ptr[8], ptr[9], ptr[10], ptr[11]};
        m.c3 = vec4 {ptr[12], ptr[13], ptr[14], ptr[15]};
        return m;
    }

    /// 32-bit Float Quaternion ///

    // f32quat operator-(const f32quat& q) {
    // 	return { -q.w, -q.x, -q.y, -q.z };
    // }
    //
    // f32quat operator+(const f32quat &a, const f32quat &b) {
    // 	return f32quat{a.w + b.w, a.x + b.x, a.y + b.y, a.z + b.z};
    // }

    // convention: result = a * b applies b first, then a (GLM-style)
    f32quat operator*(const f32quat& a, const f32quat& b) {
        return f32quat {a.w * b.w - a.x * b.x - a.y * b.y - a.z * b.z,
                        a.w * b.x + a.x * b.w + a.y * b.z - a.z * b.y,
                        a.w * b.y - a.x * b.z + a.y * b.w + a.z * b.x,
                        a.w * b.z + a.x * b.y - a.y * b.x + a.z * b.w};
    }

    f32quat operator*(const f32quat& q, f32 s) {
        return f32quat {q.w * s, q.x * s, q.y * s, q.z * s};
    }

    f32quat operator*(f32 s, const f32quat& q) {
        return q * s;
    }

    // f32quat& operator+=(f32quat& a, const f32quat& b) {
    // 	a.w += b.w;
    // 	a.x += b.x;
    // 	a.y += b.y;
    // 	a.z += b.z;
    // 	return a;
    // }

    f32quat& operator*=(f32quat& a, const f32quat& b) {
        a = a * b; // use Hamilton product
        return a;
    }

    f32quat& operator*=(f32quat& q, f32 s) {
        q.w *= s;
        q.x *= s;
        q.y *= s;
        q.z *= s;
        return q;
    }

    bool operator==(const f32quat& a, const f32quat& b) {
        return a.w == b.w && a.x == b.x && a.y == b.y && a.z == b.z;
    }

    bool operator!=(const f32quat& a, const f32quat& b) {
        return !(a == b);
    }

    vec3 rotate(const f32quat& q, const vec3& v) {
        // using the "u, s" form: q = (s, u)
        vec3 u {q.x, q.y, q.z};
        f32  s = q.w;

        f32 dot_uv = u.x * v.x + u.y * v.y + u.z * v.z; // dot(u, v)
        f32 dot_uu = u.x * u.x + u.y * u.y + u.z * u.z; // dot(u, u)

        vec3 cross_uv {u.y * v.z - u.z * v.y, u.z * v.x - u.x * v.z, u.x * v.y - u.y * v.x};

        // 2 * dot(u, v) * u
        vec3 term1 {2.0f * dot_uv * u.x, 2.0f * dot_uv * u.y, 2.0f * dot_uv * u.z};

        // (s^2 - dot(u, u)) * v
        f32  s2_minus_uu = s * s - dot_uu;
        vec3 term2 {s2_minus_uu * v.x, s2_minus_uu * v.y, s2_minus_uu * v.z};

        // 2 * s * cross(u, v)
        vec3 term3 {2.0f * s * cross_uv.x, 2.0f * s * cross_uv.y, 2.0f * s * cross_uv.z};

        return vec3 {term1.x + term2.x + term3.x, term1.y + term2.y + term3.y, term1.z + term2.z + term3.z};
    }

    vec3 operator*(const f32quat& q, const vec3& v) {
        return rotate(q, v);
    }

    f32 dot(const f32quat& a, const f32quat& b) {
        return a.w * b.w + a.x * b.x + a.y * b.y + a.z * b.z;
    }

    f32 length(const f32quat& q) {
        return math::sqrt(dot(q, q));
    }

    f32quat normalize(const f32quat& q) {
        f32       len = length(q);
        const f32 eps = 1e-6f;
        if (len < eps) {
            // zero or near-zero f32quaternion: treat as identity
            return quat(1.0f);
        }
        f32 inv = 1.0f / len;
        return f32quat {q.w * inv, q.x * inv, q.y * inv, q.z * inv};
    }

    f32quat conjugate(const f32quat& q) {
        return f32quat {q.w, -q.x, -q.y, -q.z};
    }

    f32quat inverse(const f32quat& q) {
        f32       n2 = dot(q, q);
        const f32 eps = 1e-6f;
        if (n2 < eps) {
            // effectively zero-length f32quat, can't invert sensibly
            return quat(1.0f);
        }
        f32     inv_n2 = 1.0f / n2;
        f32quat c = conjugate(q);
        return f32quat {c.w * inv_n2, c.x * inv_n2, c.y * inv_n2, c.z * inv_n2};
    }

    f32quat quaternion(const f32vec3& axis, f32 angle) {
        // normalize axis to be safe
        f32       len = math::sqrt(axis.x * axis.x + axis.y * axis.y + axis.z * axis.z);
        const f32 eps = 1e-6f;
        if (len < eps) {
            return quat(1.0f);
        }
        f32 inv_len = 1.0f / len;
        f32 half = 0.5f * angle;
        f32 s = math::sin(half);
        f32 c = math::cos(half);

        vec3 n {axis.x * inv_len, axis.y * inv_len, axis.z * inv_len};

        return f32quat {c, n.x * s, n.y * s, n.z * s};
    }

    vec4 axis_angle(const f32quat& q_) {
        f32quat q = normalize(q_);

        const f32 eps = 1e-6f;

        f32 angle = 2.0f * math::acos(math::clamp(q.w, -1.0f, 1.0f));
        f32 s2 = 1.0f - q.w * q.w;
        f32 s = s2 > eps ? math::sqrt(s2) : 0.0f;

        f32vec3 axis;
        if (s < eps) {
            // axis is undefined; pick something stable
            axis = f32vec3 {1.0f, 0.0f, 0.0f};
        }
        else {
            axis = f32vec3 {q.x / s, q.y / s, q.z / s};
        }

        return f32vec4 {axis.x, axis.y, axis.z, angle};
    }

    // convert pitch/yaw/roll to f32quaternion, Y–X–Z order
    // pitch: rotation about +X
    // yaw:   rotation about +Y
    // roll:  rotation about +Z
    f32quat quaternion(const f32vec3& euler_angles) {

        // local axis unit vectors
        f32vec3 axisX {1.0f, 0.0f, 0.0f};
        f32vec3 axisY {0.0f, 1.0f, 0.0f};
        f32vec3 axisZ {0.0f, 0.0f, 1.0f};

        f32quat qPitch = quaternion(axisX, euler_angles.pitch);
        f32quat qYaw = quaternion(axisY, euler_angles.yaw);
        f32quat qRoll = quaternion(axisZ, euler_angles.roll);

        // rpply roll, then pitch, then yaw:
        // R = Ry * Rx * Rz  => q = qYaw * qPitch * qRoll
        f32quat q = qYaw * qPitch * qRoll;
        return normalize(q);
    }

    f32vec3 euler_angles(const f32quat& q) {

        f32mat3 m = mat3_cast(q);

        // row-major aliases from column-major storage
        const f32 r02 = m.c2.x; // row 0, col 2
        const f32 r12 = m.c2.y; // row 1, col 2
        const f32 r22 = m.c2.z; // row 2, col 2

        const f32 r10 = m.c0.y; // row 1, col 0
        const f32 r11 = m.c1.y; // row 1, col 1

        f32vec3 angles;

        // pitch: asin(-r12)
        f32 sinp = -r12;
        if (sinp <= -1.0f) {
            angles.pitch = -math::PI_OVER_2; // -π/2
        }
        else if (sinp >= 1.0f) {
            angles.pitch = math::PI_OVER_2;  //  π/2
        }
        else {
            angles.pitch = math::asin(sinp);
        }

        // yaw and roll from atan2; this is undefined at exact +/- 90° pitch,
        // but numerically you'll still get a consistent choice.
        angles.yaw = math::atan2(r02, r22);
        angles.roll = math::atan2(r10, r11);

        return angles;
    }

    f32quat slerp(const f32quat& a, const f32quat& b, f32 t) {
        // clamp t just in case
        if (t <= 0.0f) {
            return a;
        }
        if (t >= 1.0f) {
            return b;
        }

        f32quat q1 = normalize(a);
        f32quat q2 = normalize(b);

        f32 cosTheta = dot(q1, q2);

        // use shortest path
        if (cosTheta < 0.0f) {
            q2 = f32quat {-q2.w, -q2.x, -q2.y, -q2.z};
            cosTheta = -cosTheta;
        }

        // if very close, fall back to lerp
        const f32 eps = 1e-6f;
        if (cosTheta > 1.0f - eps) {
            f32quat result = (1.0f - t) * q1 + t * q2;
            return normalize(result);
        }

        f32 theta = math::acos(cosTheta);
        f32 sinTheta = math::sin(theta);

        f32 w1 = math::sin((1.0f - t) * theta) / sinTheta;
        f32 w2 = math::sin(t * theta) / sinTheta;

        f32quat result = q1 * w1 + q2 * w2;
        return normalize(result);
    }

    f32mat3 mat3_cast(f32quat const& q) {
        f32 qxx(q.x * q.x);
        f32 qyy(q.y * q.y);
        f32 qzz(q.z * q.z);
        f32 qxz(q.x * q.z);
        f32 qxy(q.x * q.y);
        f32 qyz(q.y * q.z);
        f32 qwx(q.w * q.x);
        f32 qwy(q.w * q.y);
        f32 qwz(q.w * q.z);

        f32mat3 r(1.0);
        r[0][0] = f32(1) - f32(2) * (qyy + qzz);
        r[0][1] = f32(2) * (qxy + qwz);
        r[0][2] = f32(2) * (qxz - qwy);
        r[1][0] = f32(2) * (qxy - qwz);
        r[1][1] = f32(1) - f32(2) * (qxx + qzz);
        r[1][2] = f32(2) * (qyz + qwx);
        r[2][0] = f32(2) * (qxz + qwy);
        r[2][1] = f32(2) * (qyz - qwx);
        r[2][2] = f32(1) - f32(2) * (qxx + qyy);
        return r;
    }

    f32mat4 mat4_cast(f32quat const& q) {
        return mat4(mat3_cast(q));
    }

    std::string to_string(const f32quat& q) {
        std::ostringstream ss;
        ss << "f32quat(" << q.w << ", " << q.x << ", " << q.y << ", " << q.z << ")";
        return ss.str();
    }

    f32* value_ptr(f32quat& q) {
        return &q.w;
    }

    const f32* value_ptr(const f32quat& q) {
        return &q.w;
    }

    f32quat make_quat(const f32* ptr) {
        return f32quat {ptr[0], ptr[1], ptr[2], ptr[3]};
    }

    /// Projection & Camera ///

    f32mat4 perspective(f32 fovy, f32 aspect, f32 z_near, f32 z_far) {
        A3D_ASSERT(math::abs(aspect) > std::numeric_limits<f32>::epsilon() && "aspect must be non-zero");
        f32 const tanHalfFovy = tan(fovy / static_cast<f32>(2.0));
        f32mat4   result(static_cast<f32>(0.0));
        result[0][0] = static_cast<f32>(1.0) / (aspect * tanHalfFovy);
        result[1][1] = static_cast<f32>(1.0) / (tanHalfFovy);
        result[2][2] = -(z_far + z_near) / (z_far - z_near);
        result[2][3] = -static_cast<f32>(1.0);
        result[3][2] = -(static_cast<f32>(2.0) * z_far * z_near) / (z_far - z_near);
        return result;
    }

    f32mat4 ortho(f32 left, f32 right, f32 bottom, f32 top, f32 z_near, f32 z_far) {
        f32mat4 result(1.0f);
        result[0][0] = static_cast<f32>(2.0) / (right - left);
        result[1][1] = static_cast<f32>(2.0) / (top - bottom);
        result[2][2] = -static_cast<f32>(2.0) / (z_far - z_near);
        result[3][0] = -(right + left) / (right - left);
        result[3][1] = -(top + bottom) / (top - bottom);
        result[3][2] = -(z_far + z_near) / (z_far - z_near);
        return result;
    }

    f32mat4 look_at(const f32vec3& eye, const f32vec3& center, const f32vec3& up) {
        f32vec3 const f(normalize(center - eye));
        f32vec3 const s(normalize(cross(f, up)));
        f32vec3 const u(cross(s, f));

        f32mat4 result(1.0f);
        result[0][0] = s.x;
        result[1][0] = s.y;
        result[2][0] = s.z;
        result[0][1] = u.x;
        result[1][1] = u.y;
        result[2][1] = u.z;
        result[0][2] = -f.x;
        result[1][2] = -f.y;
        result[2][2] = -f.z;
        result[3][0] = -dot(s, eye);
        result[3][1] = -dot(u, eye);
        result[3][2] = dot(f, eye);
        return result;
    }

    /// Matrix Decomposition ///

    // decomposes an affine 4x4 matrix into TRS components (translation * rotation * scale).
    //
    // *** important note on negative / mirror scales: ***
    // if the determinant of the upper 3x3 is negative, one axis is effectively flipped.
    // we detect this, negate the corresponding scale component, and flip the matching
    // basis vector before converting to quaternion. this produces a valid rotation
    // but the extracted scale will have a negative component.
    //
    // this is mathematically correct, but some libraries (or artists) may expect
    // all-positive scale + an extra 180° rotation instead. if you see unexpected
    // 180° flips or mirrored models after decompose/recompose, this handedness fix
    // is the most common culprit.
    //
    // [this blurb came from Grok while auditing]
    bool decompose(const f32mat4& m, f32vec3& scale, f32quat& rotation, f32vec3& translation) {

        // 1. translation: last column (assuming standard column-major T*R*S)
        translation = f32vec3 {m.c3.x, m.c3.y, m.c3.z};

        // 2. extract basis vectors from upper 3x3 (columns 0..2)
        f32vec3 col0 {m.c0.x, m.c0.y, m.c0.z};
        f32vec3 col1 {m.c1.x, m.c1.y, m.c1.z};
        f32vec3 col2 {m.c2.x, m.c2.y, m.c2.z};

        // 3. scale is the length of each basis vector
        f32 sx = length(col0);
        f32 sy = length(col1);
        f32 sz = length(col2);

        const f32 eps = 1e-6f;
        if (sx < eps || sy < eps || sz < eps) {
            // degenerate scale, can't get a stable rotation
            scale = f32vec3 {sx, sy, sz};
            rotation = f32quat(1.0f);
            return false;
        }

        scale = f32vec3 {sx, sy, sz};

        // 4. normalize columns to get pure rotation basis
        col0 = col0 / sx;
        col1 = col1 / sy;
        col2 = col2 / sz;

        // 5. build the 3x3 rotation matrix components
        // eows from the (normalized) columns:
        f32 r00 = col0.x, r01 = col1.x, r02 = col2.x;
        f32 r10 = col0.y, r11 = col1.y, r12 = col2.y;
        f32 r20 = col0.z, r21 = col1.z, r22 = col2.z;

        // 6. fix handedness if determinant is negative
        f32 det = r00 * (r11 * r22 - r12 * r21) - r01 * (r10 * r22 - r12 * r20) + r02 * (r10 * r21 - r11 * r20);

        if (det < 0.0f) {
            // flip one axis (X here) to make it a proper rotation
            sx = -sx;
            scale.x = sx;
            col0 = -col0;

            // rebuild rows with flipped X axis
            r00 = col0.x;
            r10 = col0.y;
            r20 = col0.z;
        }

        // 7. convert rotation matrix to quaternion (standard algorithm)
        quat q;
        f32  trace = r00 + r11 + r22;

        if (trace > 0.0f) {
            f32 root = math::sqrt(trace + 1.0f);  // 4 * qw
            q.w = 0.5f * root;
            root = 0.5f / root;
            q.x = (r21 - r12) * root;
            q.y = (r02 - r20) * root;
            q.z = (r10 - r01) * root;
        }
        else {
            if (r00 >= r11 && r00 >= r22) {
                f32 root = math::sqrt(1.0f + r00 - r11 - r22);
                f32 inv = 0.5f / root;
                q.x = 0.5f * root;
                q.y = (r01 + r10) * inv;
                q.z = (r02 + r20) * inv;
                q.w = (r21 - r12) * inv;
            }
            else if (r11 > r22) {
                f32 root = math::sqrt(1.0f + r11 - r00 - r22);
                f32 inv = 0.5f / root;
                q.y = 0.5f * root;
                q.x = (r01 + r10) * inv;
                q.z = (r12 + r21) * inv;
                q.w = (r02 - r20) * inv;
            }
            else {
                f32 root = math::sqrt(1.0f + r22 - r00 - r11);
                f32 inv = 0.5f / root;
                q.z = 0.5f * root;
                q.x = (r02 + r20) * inv;
                q.y = (r12 + r21) * inv;
                q.w = (r10 - r01) * inv;
            }
        }

        rotation = q;

        return true;
    }

    /// Random / Probability ///

    f32 uniform_01() {
        return uniform_01(nullptr);
    }

    f32 uniform_01(std::mt19937* gen) {
        auto& g = pick_random_gen(gen);
        return std::generate_canonical<f32, 24>(g);
        // or (what's the difference again...?)
//		std::uniform_real_distribution<f32> dis(f32(0), f32(1));
//		return dis(pick_random_gen(gen));
    }

    f32 uniform_n11() {
        return uniform_n11(nullptr);
    }

    f32 uniform_n11(std::mt19937* gen) {
        return uniform_01(gen) * f32(2) - f32(1);
    }

    u8 uniform_linear(u8 min, u8 max) {
        return uniform_linear(nullptr, min, max);
    }

    u8 uniform_linear(std::mt19937* gen, u8 min, u8 max) {
        // aoid uniform_int_distribution<u8> quirks; generate in int.
        std::uniform_int_distribution<int> dis(static_cast<int>(min), static_cast<int>(max));
        return static_cast<u8>(dis(pick_random_gen(gen)));
    }

    u32 uniform_linear(u32 min, u32 max) {
        return uniform_linear(nullptr, min, max);
    }

    u32 uniform_linear(std::mt19937* gen, u32 min, u32 max) {
        std::uniform_int_distribution<u32> dis(min, max); // inclusive
        return dis(pick_random_gen(gen));
    }

    i32 uniform_linear(i32 min, i32 max) {
        return uniform_linear(nullptr, min, max);
    }

    i32 uniform_linear(std::mt19937* gen, i32 min, i32 max) {
        std::uniform_int_distribution<i32> dis(min, max); // inclusive
        return dis(pick_random_gen(gen));
    }

    f32 uniform_linear(f32 min, f32 max) {
        return uniform_linear(nullptr, min, max);
    }

    f32 uniform_linear(std::mt19937* gen, f32 min, f32 max) {
        return min + (max - min) * uniform_01(gen);
        // std::uniform_real_distribution<f32> dis(min, max); // [min,max)
        // return dis(pick_random_gen(gen));
    }

    f32vec2 uniform_circular(f32 radius) {
        return uniform_circular(nullptr, radius);
    }

    f32vec2 uniform_circular(std::mt19937* gen, f32 radius) {
        auto& g = pick_random_gen(gen);
        radius = math::abs(radius);

        const f32 theta = TWO_PI * uniform_01(&g);
        return f32vec2 {radius * math::cos(theta), radius * math::sin(theta)};
    }

    f32vec3 uniform_spherical(f32 radius) {
        return uniform_spherical(nullptr, radius);
    }

    f32vec3 uniform_spherical(std::mt19937* gen, f32 radius) {
        auto& g = pick_random_gen(gen);
        radius = math::abs(radius);

        // uniform on sphere surface:
        // z ~ U[-1,1), phi ~ U[0,2pi)
        const f32 z = uniform_n11(&g);
        const f32 phi = TWO_PI * uniform_01(&g);

        const f32 r_xy = math::sqrt(math::max(f32(0), f32(1) - z * z));
        const f32 x = r_xy * math::cos(phi);
        const f32 y = r_xy * math::sin(phi);

        return f32vec3 {radius * x, radius * y, radius * z};
    }

    f32vec2 uniform_disk(f32 radius) {
        return uniform_disk(nullptr, radius);
    }

    f32vec2 uniform_disk(std::mt19937* gen, f32 radius) {
        auto& g = pick_random_gen(gen);
        radius = math::abs(radius);

        // uniform in disk area:
        // r = R*sqrt(u), theta uniform
        const f32 theta = TWO_PI * uniform_01(&g);
        const f32 r = radius * math::sqrt(uniform_01(&g));

        return f32vec2 {r * math::cos(theta), r * math::sin(theta)};
    }

    f32vec3 uniform_ball(f32 radius) {
        return uniform_ball(nullptr, radius);
    }

    f32vec3 uniform_ball(std::mt19937* gen, f32 radius) {
        auto& g = pick_random_gen(gen);
        radius = math::abs(radius);

        // uniform in ball volume:
        // direction uniform on sphere, radius scaled by cbrt(u)
        const f32 z = uniform_n11(&g);
        const f32 phi = TWO_PI * uniform_01(&g);

        const f32 r_xy = math::sqrt(math::max(f32(0), f32(1) - z * z));
        const f32 dx = r_xy * math::cos(phi);
        const f32 dy = r_xy * math::sin(phi);
        const f32 dz = z;

        const f32 r = radius * math::cbrt(uniform_01(&g));

        return f32vec3 {r * dx, r * dy, r * dz};
    }

    f32 gaussian(f32 mean, f32 deviation, f32 min, f32 max) {
        return gaussian(nullptr, mean, deviation, min, max);
    }

    f32 gaussian(std::mt19937* gen, f32 mean, f32 deviation, f32 min, f32 max) {

        if (min > max) {
            math::swap(min, max);
        }
        if (!(deviation > f32(0))) {
            return math::clamp(mean, min, max);
        }

        std::normal_distribution<f32> dis(mean, deviation);
        auto&                         g = pick_random_gen(gen);

        // rejection sampling (usually fast unless range is tiny)
        for (int i = 0; i < 16; ++i) {
            f32 v = dis(g);
            if (v >= min && v <= max) {
                return v;
            }
        }
        // fallback
        return math::clamp(dis(g), min, max);
    }

    bool bernoulli(f32 p) {
        return bernoulli(nullptr, p);
    }

    bool bernoulli(std::mt19937* gen, f32 p) {
        p = math::clamp(p, f32(0), f32(1));
        std::bernoulli_distribution dis(static_cast<double>(p));
        return dis(pick_random_gen(gen));
    }

    /// Easing ///

    f32 saturate(f32 t) {
        return clamp_01(t);
    }

    f32 lerp(f32 a, f32 b, f32 t) {
        return a + (b - a) * t;
    }

    f32 lerp_01(f32 a, f32 b, f32 t) {
        return lerp(a, b, clamp_01(t));
    }

    f32vec2 lerp(const f32vec2& a, const f32vec2& b, f32 t) {
        return a + (b - a) * t;
    }

    f32vec2 lerp_01(const f32vec2& a, const f32vec2& b, f32 t) {
        return lerp(a, b, clamp_01(t));
    }

    f32vec3 lerp(const f32vec3& a, const f32vec3& b, f32 t) {
        return a + (b - a) * t;
    }

    f32vec3 lerp_01(const f32vec3& a, const f32vec3& b, f32 t) {
        return lerp(a, b, clamp_01(t));
    }

    f32vec4 lerp(const f32vec4& a, const f32vec4& b, f32 t) {
        return a + (b - a) * t;
    }

    f32vec4 lerp_01(const f32vec4& a, const f32vec4& b, f32 t) {
        return lerp(a, b, clamp_01(t));
    }

    f32 inverse_lerp(f32 a, f32 b, f32 v) {
        const f32 denom = (b - a);
        if (denom == f32(0)) {
            return f32(0);
        }
        return (v - a) / denom;
    }

    f32 inverse_lerp_01(f32 a, f32 b, f32 v) {
        return clamp_01(inverse_lerp(a, b, v));
    }

    f32 remap(f32 in_a, f32 in_b, f32 out_a, f32 out_b, f32 v) {
        // remap = lerp(outA, outB, inverse_lerp(inA, inB, v))

        const f32 t = inverse_lerp(in_a, in_b, v);
        return lerp(out_a, out_b, t);
    }

    f32 remap_01(f32 in_a, f32 in_b, f32 out_a, f32 out_b, f32 v) {
        // remap_01 clamps normalized t to [0,1]

        const f32 t = inverse_lerp_01(in_a, in_b, v);
        return lerp(out_a, out_b, t);
    }

    f32 step(f32 edge, f32 x) {
        // step(edge,x) = (x < edge) ? 0 : 1

        return (x < edge) ? f32(0) : f32(1);
    }

    f32 smoothstep(f32 t) {
        // smoothstep(t) = t^2 * (3 - 2t)    (expects t in [0,1])

        return t * t * (f32(3) - f32(2) * t);
    }

    f32 smoothstep_01(f32 t) {
        // smoothstep_01(t) = smoothstep(clamp(t,0,1))

        t = clamp_01(t);
        return smoothstep(t);
    }

    f32 smootherstep(f32 t) {
        // smootherstep(t) = t^3 * (t * (t*6 - 15) + 10)   (expects t in [0,1])

        return t * t * t * (t * (t * f32(6) - f32(15)) + f32(10));
    }

    f32 smootherstep_01(f32 t) {
        t = clamp_01(t);
        return smootherstep(t);
    }

    f32 smoothstep_unclamped(f32 edge0, f32 edge1, f32 x) {
        // normalize WITHOUT clamping:
        // t = (x - edge0) / (edge1 - edge0)
        // smoothstep = t^2 * (3 - 2t)

        const f32 denom = (edge1 - edge0);
        if (denom == f32(0)) {
            return f32(0);
        }
        const f32 t = (x - edge0) / denom;
        return smoothstep(t);
    }

    f32 smoothstep(f32 edge0, f32 edge1, f32 x) {
        // classic smoothstep:
        // t = clamp( (x - edge0) / (edge1 - edge0), 0, 1 )
        // f(t) = t^2 * (3 - 2t)

        const f32 denom = (edge1 - edge0);
        if (denom == f32(0)) {
            return f32(0);
        }
        const f32 t = clamp_01((x - edge0) / denom);
        return smoothstep(t);
    }

    f32 smootherstep_unclamped(f32 edge0, f32 edge1, f32 x) {
        // t = (x - edge0) / (edge1 - edge0)
        // f(t) = t^3 * (t * (t*6 - 15) + 10)

        const f32 denom = (edge1 - edge0);
        if (denom == f32(0)) {
            return f32(0);
        }
        const f32 t = (x - edge0) / denom;
        return smootherstep(t);
    }

    f32 smootherstep(f32 edge0, f32 edge1, f32 x) {
        // t = clamp( (x - edge0) / (edge1 - edge0), 0, 1 )
        // f(t) = t^3 * (t * (t*6 - 15) + 10)

        const f32 denom = (edge1 - edge0);
        if (denom == f32(0)) {
            return f32(0);
        }
        const f32 t = clamp_01((x - edge0) / denom);
        return smootherstep(t);
    }

    f32 ease_linear(f32 t) {
        // f(t) = t

        return t;
    }

    f32 ease_linear_01(f32 t) {
        // f(t) = clamp(t,0,1)

        return clamp_01(t);
    }

    f32 ease_in_quadratic(f32 t) {
        // f(t) = t^2

        t = clamp_01(t);
        return t * t;
    }

    f32 ease_out_quadratic(f32 t) {
        // f(t) = 1 - (1 - t)^2

        t = clamp_01(t);
        f32 u = f32(1) - t;
        return f32(1) - u * u;
    }

    f32 ease_in_out_quadratic(f32 t) {
        // f(t) = { 2t^2              		| t < 1/2
        //        { 1 - (-2t+2)^2 / 2     	| otherwise

        t = clamp_01(t);
        if (t < f32(0.5)) {
            return f32(2) * t * t;
        }
        f32 u = f32(-2) * t + f32(2);
        return f32(1) - (u * u) / f32(2);
    }

    f32 ease_in_cubic(f32 t) {
        // f(t) = t^3

        t = clamp_01(t);
        return t * t * t;
    }

    f32 ease_out_cubic(f32 t) {
        // f(t) = 1 - (1 - t)^3

        t = clamp_01(t);
        f32 u = f32(1) - t;
        return f32(1) - u * u * u;
    }

    f32 ease_in_out_cubic(f32 t) {
        // f(t) = { 4t^3                   	| t < 1/2
        //        { 1 - (-2t+2)^3 / 2      	| otherwise

        t = clamp_01(t);
        if (t < f32(0.5)) {
            return f32(4) * t * t * t;
        }
        f32 u = f32(-2) * t + f32(2);
        return f32(1) - (u * u * u) / f32(2);
    }

    f32 ease_in_quartic(f32 t) {
        // f(t) = t^4

        t = clamp_01(t);
        f32 t2 = t * t;
        return t2 * t2;
    }

    f32 ease_out_quartic(f32 t) {
        // f(t) = 1 - (1 - t)^4

        t = clamp_01(t);
        f32 u = f32(1) - t;
        f32 u2 = u * u;
        return f32(1) - (u2 * u2);
    }

    f32 ease_in_out_quartic(f32 t) {
        // f(t) = { 8t^4                    | t < 1/2
        //        { 1 - (-2t+2)^4 / 2       | otherwise

        t = clamp_01(t);
        if (t < f32(0.5)) {
            f32 t2 = t * t;
            return f32(8) * t2 * t2;
        }
        f32 u = f32(-2) * t + f32(2);
        f32 u2 = u * u;
        return f32(1) - (u2 * u2) / f32(2);
    }

    f32 ease_in_quintic(f32 t) {
        // f(t) = t^5

        t = clamp_01(t);
        f32 t2 = t * t;
        return t2 * t2 * t;
    }

    f32 ease_out_quintic(f32 t) {
        // f(t) = 1 - (1 - t)^5

        t = clamp_01(t);
        f32 u = f32(1) - t;
        f32 u2 = u * u;
        return f32(1) - (u2 * u2 * u);
    }

    f32 ease_in_out_quintic(f32 t) {
        // f(t) = { 16t^5                   | t < 1/2
        //        { 1 - (-2t+2)^5 / 2       | otherwise

        t = clamp_01(t);
        if (t < f32(0.5)) {
            f32 t2 = t * t;
            return f32(16) * t2 * t2 * t;
        }
        f32 u = f32(-2) * t + f32(2);
        f32 u2 = u * u;
        return f32(1) - (u2 * u2 * u) / f32(2);
    }

    f32 ease_in_sine(f32 t) {
        // f(t) = 1 - cos( (t*pi) / 2 )

        t = clamp_01(t);
        return f32(1) - math::cos((t * PI) / f32(2));
    }

    f32 ease_out_sine(f32 t) {
        // f(t) = sin( (t*pi) / 2 )

        t = clamp_01(t);
        return math::sin((t * PI) / f32(2));
    }

    f32 ease_in_out_sine(f32 t) {
        // f(t) = -(cos(pi*t) - 1) / 2
        t = clamp_01(t);
        return (f32(1) - math::cos(PI * t)) / f32(2);
    }

    f32 ease_in_circular(f32 t) {
        // f(t) = 1 - sqrt(1 - t^2)

        t = clamp_01(t);
        return f32(1) - math::sqrt(math::max(f32(0), f32(1) - t * t));
    }

    f32 ease_out_circular(f32 t) {
        // f(t) = sqrt(1 - (t - 1)^2)

        t = clamp_01(t);
        f32 u = t - f32(1);
        return math::sqrt(math::max(f32(0), f32(1) - u * u));
    }

    f32 ease_in_out_circular(f32 t) {
        // f(t) = { (1 - sqrt(1 - (2t)^2)) / 2    	| t < 1/2
        //        { (sqrt(1 - (-2t+2)^2) + 1) / 2   | otherwise

        t = clamp_01(t);
        if (t < f32(0.5)) {
            f32 x = f32(2) * t;
            return (f32(1) - math::sqrt(math::max(f32(0), f32(1) - x * x))) / f32(2);
        }
        f32 x = f32(-2) * t + f32(2);
        return (math::sqrt(math::max(f32(0), f32(1) - x * x)) + f32(1)) / f32(2);
    }

    f32 ease_in_exponential(f32 t) {
        // f(t) = { 0                          	| t = 0
        //        { 2^(10t - 10)               	| otherwise

        t = clamp_01(t);
        if (t == f32(0)) {
            return f32(0);
        }
        return math::pow(f32(2), f32(10) * t - f32(10));
    }

    f32 ease_out_exponential(f32 t) {
        // f(t) = { 1                          	| t = 1
        //        { 1 - 2^(-10t)               	| otherwise

        t = clamp_01(t);
        if (t == f32(1)) {
            return f32(1);
        }
        return f32(1) - math::pow(f32(2), -f32(10) * t);
    }

    f32 ease_in_out_exponential(f32 t) {
        // f(t) = { 0                           | t = 0
        //        { 1                           | t = 1
        //        { 2^(20t-10) / 2             	| t < 1/2
        //        { (2 - 2^(-20t+10)) / 2      	| otherwise

        t = clamp_01(t);
        if (t == f32(0)) {
            return f32(0);
        }
        if (t == f32(1)) {
            return f32(1);
        }
        if (t < f32(0.5)) {
            return math::pow(f32(2), f32(20) * t - f32(10)) / f32(2);
        }
        return (f32(2) - math::pow(f32(2), -f32(20) * t + f32(10))) / f32(2);
    }

    f32 ease_in_back(f32 t, f32 overshoot) {
        // Penner-style:
        // c1 = overshoot, c3 = c1 + 1
        // f(t) = c3*t^3 - c1*t^2

        t = clamp_01(t);
        const f32 c1 = overshoot;
        const f32 c3 = c1 + f32(1);
        return c3 * t * t * t - c1 * t * t;
    }

    f32 ease_out_back(f32 t, f32 overshoot) {
        // f(t) = 1 + c3*(t-1)^3 + c1*(t-1)^2

        t = clamp_01(t);
        const f32 c1 = overshoot;
        const f32 c3 = c1 + f32(1);
        f32       u = t - f32(1);
        return f32(1) + c3 * u * u * u + c1 * u * u;
    }

    f32 ease_in_out_back(f32 t, f32 overshoot) {
        // c2 = overshoot * 1.525
        // f(t) = { ( (2t)^2 * ((c2+1)*2t - c2) ) / 2            	| t < 1/2
        //        { ( (2t-2)^2 * ((c2+1)*(2t-2) + c2) + 2 ) / 2 	| otherwise

        t = clamp_01(t);
        const f32 c2 = overshoot * f32(1.525);

        if (t < f32(0.5)) {
            f32 x = f32(2) * t;
            return (x * x * ((c2 + f32(1)) * x - c2)) / f32(2);
        }
        f32 x = f32(2) * t - f32(2);
        return (x * x * ((c2 + f32(1)) * x + c2) + f32(2)) / f32(2);
    }

    f32 ease_in_elastic(f32 t) {
        // f(t) = { 0                                   	| t = 0
        //        { 1                                     	| t = 1
        //        { -2^(10t-10) * sin((10t - 10.75)*c4)  	| otherwise

        t = clamp_01(t);
        if (t == f32(0)) {
            return f32(0);
        }
        if (t == f32(1)) {
            return f32(1);
        }
        const f32 c4 = TWO_PI / f32(3);
        return -math::pow(f32(2), f32(10) * t - f32(10)) * math::sin((f32(10) * t - f32(10.75)) * c4);
    }

    f32 ease_out_elastic(f32 t) {
        // f(t) = { 0                                    	| t = 0
        //        { 1                                    	| t = 1
        //        { 2^(-10t) * sin((10t - 0.75)*c4) + 1    	| otherwise

        t = clamp_01(t);
        if (t == f32(0)) {
            return f32(0);
        }
        if (t == f32(1)) {
            return f32(1);
        }
        const f32 c4 = TWO_PI / f32(3);
        return math::pow(f32(2), -f32(10) * t) * math::sin((f32(10) * t - f32(0.75)) * c4) + f32(1);
    }

    f32 ease_in_out_elastic(f32 t) {
        // f(t) = { 0                                            		| t = 0
        //        { 1                                               	| t = 1
        //        { -(2^(20t-10) * sin((20t - 11.125)*c5)) / 2       	| t < 1/2
        //        {  (2^(-20t+10) * sin((20t - 11.125)*c5)) / 2 + 1   	| otherwise

        t = clamp_01(t);
        if (t == f32(0)) {
            return f32(0);
        }
        if (t == f32(1)) {
            return f32(1);
        }
        const f32 c5 = TWO_PI / f32(4.5);

        if (t < f32(0.5)) {
            return -(math::pow(f32(2), f32(20) * t - f32(10)) * math::sin((f32(20) * t - f32(11.125)) * c5))
                   / f32(2);
        }
        return (math::pow(f32(2), -f32(20) * t + f32(10)) * math::sin((f32(20) * t - f32(11.125)) * c5))
                   / f32(2)
               + f32(1);
    }

    f32 ease_out_bounce(f32 t) {
        // f_out(t) = piecewise(...)

        t = clamp_01(t);
        return ease_out_bounce_impl(t);
    }

    f32 ease_in_bounce(f32 t) {
        // f_in(t) = 1 - f_out(1 - t)

        t = clamp_01(t);
        return f32(1) - ease_out_bounce_impl(f32(1) - t);
    }

    f32 ease_in_out_bounce(f32 t) {
        // f(t) = { (1 - f_out(1 - 2t)) / 2    	| t < 1/2
        //        { (1 + f_out(2t - 1)) / 2   	| otherwise

        t = clamp_01(t);
        if (t < f32(0.5)) {
            return (f32(1) - ease_out_bounce_impl(f32(1) - f32(2) * t)) / f32(2);
        }
        return (f32(1) + ease_out_bounce_impl(f32(2) * t - f32(1))) / f32(2);
    }

    f32quat nlerp(const f32quat& a_in, const f32quat& b_in, f32 t, bool shortest_path) {
        // nlerp(a,b,t) = normalize( (1-t)*a + t*b )

        f32quat a = a_in;
        f32quat b = b_in;

        // shortest path: if dot < 0, flip b (since q and -q are same rotation)
        f32 d = dot(a, b);
        if (shortest_path && d < f32(0)) {
            b = -b;
            d = -d;
        }

        const f32 it = f32(1) - t;
        return normalize((a * it) + (b * t));
    }

    f32quat nlerp_01(const f32quat& a, const f32quat& b, f32 t, bool shortest_path) {
        return nlerp(a, b, clamp_01(t), shortest_path);
    }

    f32quat slerp(const f32quat& a_in, const f32quat& b_in, f32 t, bool shortest_path) {
        // slerp(a,b,t) =
        //   sin((1-t)*Ω)/sin(Ω) * a  +  sin(t*Ω)/sin(Ω) * b
        //
        // where Ω = acos(dot(a,b)) and a,b are unit quaternions.

        f32quat a = normalize(a_in);
        f32quat b = normalize(b_in);

        f32 d = dot(a, b);

        // shortest path: flip b if needed
        if (shortest_path && d < f32(0)) {
            b = -b;
            d = -d;
        }

        // numerical safety
        d = math::clamp(d, f32(-1), f32(1));

        // if very close, fall back to nlerp (avoids division by tiny sin(Ω))
        // threshold ~0.9995 is common.
        if (d > f32(0.9995)) {
            return nlerp(a, b, t, false);
        }

        const f32 omega = math::acos(d);          // Ω
        const f32 sin_omega = math::sin(omega);   // sin(Ω)

        const f32 s0 = math::sin((f32(1) - t) * omega) / sin_omega;
        const f32 s1 = math::sin(t * omega) / sin_omega;

        return (a * s0) + (b * s1);
    }

    f32quat slerp_01(const f32quat& a, const f32quat& b, f32 t, bool shortest_path) {
        return slerp(a, b, clamp_01(t), shortest_path);
    }

    /// Color & Color Spaces ///

    f32 srgb_to_linear(f32 c) {
        // IEC 61966-2-1:
        // linear = c/12.92              		| c <= 0.04045
        // linear = ((c+0.055)/1.055)^2.4  		| otherwise
        c = clamp_01(c);
        if (c <= f32(0.04045)) {
            return c / f32(12.92);
        }
        return math::pow((c + f32(0.055)) / f32(1.055), f32(2.4));
    }

    f32 linear_to_srgb(f32 c) {
        // IEC 61966-2-1:
        // srgb = 12.92 * c               		| c <= 0.0031308
        // srgb = 1.055*c^(1/2.4) - 0.055 		| otherwise
        c = clamp_01(c);
        if (c <= f32(0.0031308)) {
            return c * f32(12.92);
        }
        return f32(1.055) * math::pow(c, f32(1) / f32(2.4)) - f32(0.055);
    }

    f32vec3 srgb_to_linear(const f32vec3& c) {
        return f32vec3 {srgb_to_linear(c.r), srgb_to_linear(c.g), srgb_to_linear(c.b)};
    }

    f32vec3 linear_to_srgb(const f32vec3& c) {
        return f32vec3 {linear_to_srgb(c.r), linear_to_srgb(c.g), linear_to_srgb(c.b)};
    }

    f32vec4 srgb_to_linear(const f32vec4& c) {
        // alpha unchanged
        return f32vec4 {srgb_to_linear(c.r), srgb_to_linear(c.g), srgb_to_linear(c.b), c.a};
    }

    f32vec4 linear_to_srgb(const f32vec4& c) {
        // alpha unchanged
        return f32vec4 {linear_to_srgb(c.r), linear_to_srgb(c.g), linear_to_srgb(c.b), c.a};
    }

    f32vec3 saturate(const f32vec3& c) {
        return f32vec3 {clamp_01(c.r), clamp_01(c.g), clamp_01(c.b)};
    }

    f32vec4 saturate(const f32vec4& c) {
        return f32vec4 {clamp_01(c.r), clamp_01(c.g), clamp_01(c.b), clamp_01(c.a)};
    }

    f32 luminance_rec709(const f32vec3& rgb_linear) {
        return rgb_linear.r * f32(0.2126) + rgb_linear.g * f32(0.7152) + rgb_linear.b * f32(0.0722);
    }

    f32 luminance_rec709(const f32vec4& rgba_linear) {
        return luminance_rec709(f32vec3 {rgba_linear.r, rgba_linear.g, rgba_linear.b});
    }

    f32vec4 premultiply_alpha(const f32vec4& rgba) {
        // rgb' = rgb * a
        return f32vec4 {rgba.r * rgba.a, rgba.g * rgba.a, rgba.b * rgba.a, rgba.a};
    }

    f32vec4 unpremultiply_alpha(const f32vec4& rgba) {
        // rgb' = rgb / a (guard a==0)
        if (rgba.a <= f32(0)) {
            return f32vec4 {f32(0), f32(0), f32(0), rgba.a};
        }
        const f32 inv = f32(1) / rgba.a;
        return f32vec4 {rgba.r * inv, rgba.g * inv, rgba.b * inv, rgba.a};
    }

    f32vec3 lerp_linear_rgb(const f32vec3& a, const f32vec3& b, f32 t) {
        return f32vec3 {a.r + (b.r - a.r) * t, a.g + (b.g - a.g) * t, a.b + (b.b - a.b) * t};
    }

    f32vec4 lerp_linear_rgba(const f32vec4& a, const f32vec4& b, f32 t) {
        return f32vec4 {a.r + (b.r - a.r) * t, a.g + (b.g - a.g) * t, a.b + (b.b - a.b) * t,
                        a.a + (b.a - a.a) * t};
    }

    f32vec3 rgb_to_hsv(const f32vec3& rgb) {
        const f32 r = rgb.r, g = rgb.g, b = rgb.b;
        const f32 mx = math::max(r, math::max(g, b));
        const f32 mn = math::min(r, math::min(g, b));
        const f32 d = mx - mn;

        // v = max channel
        const f32 v = mx;

        // s = d / mx (or 0 if mx==0)
        const f32 s = (mx > f32(0)) ? (d / mx) : f32(0);

        // hue in [0,1)
        f32 h = f32(0);
        if (d > f32(0)) {
            if (mx == r) {
                h = (g - b) / d;
                if (h < f32(0)) {
                    h += f32(6);
                }
            }
            else if (mx == g) {
                h = (b - r) / d + f32(2);
            }
            else { // mx == b
                h = (r - g) / d + f32(4);
            }
            h /= f32(6);
        }

        return f32vec3 {h, s, v}; // (h,s,v)
    }

    f32vec3 hsv_to_rgb(const f32vec3& hsv) {
        f32       h = hsv.x; // hue in [0,1) (wrapped below)
        const f32 s = clamp_01(hsv.y);
        const f32 v = clamp_01(hsv.z);

        // Wrap hue into [0,1)
        h = h - math::floor(h);

        // HSV -> RGB:
        // c = v*s
        // hp = h*6
        // x = c*(1 - |(hp mod 2) - 1|)
        // m = v - c
        const f32 c = v * s;
        const f32 hp = h * f32(6);
        const f32 x = c * (f32(1) - math::abs(math::mod(hp, f32(2)) - f32(1)));
        const f32 m = v - c;

        f32 rp = 0, gp = 0, bp = 0;
        if (hp < f32(1)) {
            rp = c;
            gp = x;
            bp = 0;
        }
        else if (hp < f32(2)) {
            rp = x;
            gp = c;
            bp = 0;
        }
        else if (hp < f32(3)) {
            rp = 0;
            gp = c;
            bp = x;
        }
        else if (hp < f32(4)) {
            rp = 0;
            gp = x;
            bp = c;
        }
        else if (hp < f32(5)) {
            rp = x;
            gp = 0;
            bp = c;
        }
        else {
            rp = c;
            gp = 0;
            bp = x;
        }

        return f32vec3 {rp + m, gp + m, bp + m}; // returned as rgb (use .r/.g/.b)
    }

    f32vec3 rgb_to_hsl(const f32vec3& rgb) {
        const f32 r = rgb.r, g = rgb.g, b = rgb.b;
        const f32 mx = math::max(r, math::max(g, b));
        const f32 mn = math::min(r, math::min(g, b));
        const f32 d = mx - mn;

        // l = (mx + mn)/2
        const f32 l = (mx + mn) / f32(2);

        // s = d / (1 - |2l - 1|) (or 0 if d==0)
        f32 s = f32(0);
        if (d > f32(0)) {
            const f32 denom = f32(1) - math::abs(f32(2) * l - f32(1));
            s = (denom > f32(0)) ? (d / denom) : f32(0);
        }

        // hue in [0,1)
        f32 h = f32(0);
        if (d > f32(0)) {
            if (mx == r) {
                h = (g - b) / d;
                if (h < f32(0)) {
                    h += f32(6);
                }
            }
            else if (mx == g) {
                h = (b - r) / d + f32(2);
            }
            else {
                h = (r - g) / d + f32(4);
            }
            h /= f32(6);
        }

        return f32vec3 {h, s, l}; // (h,s,l)
    }

    f32vec3 hsl_to_rgb(const f32vec3& hsl) {
        f32       h = hsl.x;
        const f32 s = clamp_01(hsl.y);
        const f32 l = clamp_01(hsl.z);

        // wrap hue into [0,1)
        h = h - math::floor(h);

        if (s == f32(0)) {
            // achromatic
            return f32vec3 {l, l, l};
        }

        const f32 q = (l < f32(0.5)) ? (l * (f32(1) + s)) : (l + s - l * s);
        const f32 p = f32(2) * l - q;

        const f32 r = hue2rgb(p, q, h + f32(1.0 / 3.0));
        const f32 g = hue2rgb(p, q, h);
        const f32 b = hue2rgb(p, q, h - f32(1.0 / 3.0));

        return f32vec3 {r, g, b};
    }

    /// Scalar Angles ///

    f32 radians(f32 degrees) {
        return degrees * static_cast<f32>(0.01745329251994329576923690768489);
    }

    f32 degrees(f32 radians) {
        return radians * static_cast<f32>(57.295779513082320876798154814105);
    }

    /// Scalar Trig ///

    f32 sin(f32 n) {
        return std::sin(n);
    }

    f32 cos(f32 n) {
        return std::cos(n);
    }

    f32 tan(f32 n) {
        return std::tan(n);
    }

    f32 asin(f32 n) {
        return std::asin(n);
    }

    f32 acos(f32 n) {
        return std::acos(n);
    }

    f32 atan(f32 n) {
        return std::atan(n);
    }

    f32 atan2(f32 y, f32 x) {
        return std::atan2(y, x);
    }

    f32 sinh(f32 n) {
        return std::sinh(n);
    }

    f32 cosh(f32 n) {
        return std::cosh(n);
    }

    f32 tanh(f32 n) {
        return std::tanh(n);
    }

    f32 asinh(f32 n) {
        return std::asinh(n);
    }

    f32 acosh(f32 n) {
        return std::acosh(n);
    }

    f32 atanh(f32 n) {
        return std::atanh(n);
    }

    /// Scalar Comparison ///

    bool equal(f32 a, f32 b, f32 eps) {
        return math::abs(a - b) <= eps;
    }

    /// Scalar Rounding ///

    f32 ceil(f32 n) {
        return std::ceil(n);
    }

    f32 floor(f32 n) {
        return std::floor(n);
    }

    f32 round(f32 n) {
        return std::round(n);
    }

    /// Scalar Exponentials & Logarithms ///

    f32 exp(f32 n) {
        return std::exp(n);
    }

    f32 exp2(f32 n) {
        return std::exp2(n);
    }

    f32 pow(f32 x, f32 y) {
        return std::pow(x, y);
    }

    f32 log(f32 n) {
        return std::log(n);
    }

    f32 log2(f32 n) {
        return std::log2(n);
    }

    f32 log10(f32 n) {
        return std::log10(n);
    }

    /// Scalar Magnitude ///

    f32 abs(f32 n) {
        return std::abs(n);
    }

    f32 sqrt(f32 n) {
        return std::sqrt(n);
    }

    f32 cbrt(f32 n) {
        return std::cbrt(n);
    }

    /// Scalar Range / Ordering ///

    f32 clamp_01(f32 t) {
        return std::clamp(t, f32(0), f32(1));
    }

    /// Scalar Remainder / Wrap ///

    f32 fmod(f32 x, f32 y) {
        return std::fmod(x, y);
    }

    f32 mod(f32 x, f32 y) {
        f32 r = std::fmod(x, y);
        if (r < 0) {
            r += y;
        }
        return r;
    }

    f32 wrap(f32 x, f32 lo, f32 hi) {
        f32 w = hi - lo;
        if (w == 0) {
            return lo;
        }
        return lo + mod(x - lo, w);
    }

    /// Bitwise / Classification ///

    bool is_nan(f32 n) {
        return std::isnan(n);
    }

    bool is_infinite(f32 n) {
        return std::isinf(n);
    }

    bool is_finite(f32 n) {
        return std::isfinite(n);
    }

    bool sign_bit(f32 n) {
        return std::signbit(n);
    }

    /// Swap Utilities ///

    void swap(f32& a, f32& b) {
        std::swap(a, b);
    }

    /// Private Utilities ///

    std::mt19937& default_random_gen() {
        thread_local std::mt19937 gen([] {
            std::random_device rd;
            std::seed_seq      seq {rd(), rd(), rd(), rd(), rd(), rd(), rd(), rd()};
            return std::mt19937(seq);
        }());
        return gen;
    }

    std::mt19937& pick_random_gen(std::mt19937* gen) {
        return gen ? *gen : default_random_gen();
    }

    f32 ease_out_bounce_impl(f32 t) {
        // Piecewise parabola bounce (Penner)
        const f32 n1 = f32(7.5625);
        const f32 d1 = f32(2.75);

        if (t < f32(1) / d1) {
            return n1 * t * t;
        }
        else if (t < f32(2) / d1) {
            t -= f32(1.5) / d1;
            return n1 * t * t + f32(0.75);
        }
        else if (t < f32(2.5) / d1) {
            t -= f32(2.25) / d1;
            return n1 * t * t + f32(0.9375);
        }
        else {
            t -= f32(2.625) / d1;
            return n1 * t * t + f32(0.984375);
        }
    }

    f32 hue2rgb(f32 p, f32 q, f32 t) {
        // HSL helper
        if (t < f32(0)) {
            t += f32(1);
        }
        if (t > f32(1)) {
            t -= f32(1);
        }
        if (t < f32(1.0 / 6.0)) {
            return p + (q - p) * f32(6) * t;
        }
        if (t < f32(1.0 / 2.0)) {
            return q;
        }
        if (t < f32(2.0 / 3.0)) {
            return p + (q - p) * (f32(2.0 / 3.0) - t) * f32(6);
        }
        return p;
    }

    std::string mat_right_fit(std::string s, int width) {
        if (width <= 0) {
            return s;
        }
        if ((int) s.size() < width) {
            return std::format("{:>{}}", s, width);
        }
        if ((int) s.size() == width) {
            return s;
        }
        return std::string(width, '#'); // overflow marker (guaranteed width)
    }

    std::string mat_fmtf(float v, unsigned width) {
        if (width <= 0) {
            return std::format("{}", v);
        }

        // special values
        if (math::is_nan(v)) {
            return mat_right_fit("nan", width);
        }
        if (math::is_infinite(v)) {
            return mat_right_fit((v < 0) ? "-inf" : "inf", width);
        }

        // avoid "-0.000..."
        if (v == 0.0f) {
            v = 0.0f;
        }

        const bool  neg = math::sign_bit(v);
        const float a = math::abs(v);

        // count integer digits of |v|
        int int_digits = 1;
        if (a >= 1.0f) {
            int_digits = static_cast<int>(math::floor(math::log10(a))) + 1;
        }

        // decimals that can fit if we include '.' (when decimals > 0)
        int max_dec = width - (neg ? 1 : 0) - int_digits - 1;
        max_dec = math::clamp(max_dec, 0, (int) width);

        // try fixed, reducing decimals until it fits
        for (int dec = max_dec; dec >= 0; --dec) {
            std::string s = std::format("{:{}.{}f}", v, width, dec); // dynamic width + precision
            if ((int) s.size() <= width) {
                return mat_right_fit(std::move(s), width);
            }
        }

        // fallback: scientific, try to fit by reducing precision
        for (int prec = math::min(6, (int) width); prec >= 0; --prec) {
            std::string s = std::format("{:{}.{}e}", v, width, prec);
            if ((int) s.size() <= width) {
                return mat_right_fit(std::move(s), width);
            }
        }

        return std::string(width, '#');
    }

    u8 saturate_u8(f32 v) {
        return static_cast<u8>(math::clamp(v, 0.0f, 255.0f));
    }

    f32quat operator-(const f32quat& q) {
        return {-q.w, -q.x, -q.y, -q.z};
    }

    f32quat operator+(const f32quat& a, const f32quat& b) {
        return f32quat {a.w + b.w, a.x + b.x, a.y + b.y, a.z + b.z};
    }

    f32quat& operator+=(f32quat& a, const f32quat& b) {
        a.w += b.w;
        a.x += b.x;
        a.y += b.y;
        a.z += b.z;
        return a;
    }

}
