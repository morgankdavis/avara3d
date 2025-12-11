//
//  Math.cc
//  avara3d
//
//  Created by Morgan Davis on 12/7/25.
//  Copyright © 2025 Morgan K Davis. All rights reserved.
//

#include "a3d/Math.h"

#include <sstream>

namespace a3d::math {

	/// Types ///

	vec2::vec2():
		x{0}, y{0} {}

	vec2::vec2(f32 x_, f32 y_):
			x{x_}, y{y_} {}

	vec2::vec2(const vec3& v):
			x{v.x}, y{v.y} {}

	vec2::vec2(const vec4& v):
			x{v.x}, y{v.y} {}

	f32& vec2::operator[](std::size_t i) {
		assert(i < 2);
		return (&x)[i];
	}

	const f32& vec2::operator[](std::size_t i) const {
		assert(i < 2);
		return (&x)[i];
	}

	vec3::vec3():
			x{0}, y{0}, z{0} {}

	vec3::vec3(f32 x_, f32 y_, f32 z_):
			x{x_}, y{y_}, z{z_} {}

	vec3::vec3(const vec2& v, f32 z_):
		x{v.x}, y{v.y}, z{z_} {}

	vec3::vec3(const vec2& v):
			x{v.x}, y{v.y}, z{0} {}

	vec3::vec3(const vec4& v):
			x{v.x}, y{v.y}, z{v.z} {}

	f32& vec3::operator[](std::size_t i) {
		assert(i < 3);
		return (&x)[i];
	}

	const f32& vec3::operator[](std::size_t i) const {
		assert(i < 3);
		return (&x)[i];
	}

	vec4::vec4():
			x{0}, y{0}, z{0}, w{0} {}

	vec4::vec4(f32 x_, f32 y_, f32 z_, f32 w_):
			x{x_}, y{y_}, z{z_}, w{w_} {}

	vec4::vec4(const vec3& v, f32 w_):
		x{v.x}, y{v.y}, z{v.z}, w{w_} {}

	vec4::vec4(const vec2& v):
			x{v.x}, y{v.y}, z{0}, w{0} {}

	vec4::vec4(const vec3& v):
			x{v.x}, y{v.y}, z{v.z}, w{0} {}

	f32& vec4::operator[](std::size_t i) {
		assert(i < 4);
		return (&x)[i];
	}

	const f32& vec4::operator[](std::size_t i) const {
		assert(i < 4);
		return (&x)[i];
	}

	ivec2::ivec2():
			x{0}, y{0} {}

	ivec2::ivec2(i32 x_, i32 y_):
			x{x_}, y{y_} {}

	ivec2::ivec2(const ivec3& v):
			x{v.x}, y{v.y} {}

	ivec2::ivec2(const ivec4& v):
			x{v.x}, y{v.y} {}

	i32& ivec2::operator[](std::size_t i) {
		assert(i < 2);
		return (&x)[i];
	}

	const i32& ivec2::operator[](std::size_t i) const {
		assert(i < 2);
		return (&x)[i];
	}

	ivec3::ivec3():
			x{0}, y{0}, z{0} {}

	ivec3::ivec3(i32 x_, i32 y_, i32 z_):
			x{x_}, y{y_}, z{z_} {}

	ivec3::ivec3(const ivec2& v, i32 z_):
			x{v.x}, y{v.y}, z{z_} {}

	ivec3::ivec3(const ivec2& v):
			x{v.x}, y{v.y}, z{0} {}

	ivec3::ivec3(const ivec4& v):
			x{v.x}, y{v.y}, z{v.z} {}

	i32& ivec3::operator[](std::size_t i) {
		assert(i < 3);
		return (&x)[i];
	}

	const i32& ivec3::operator[](std::size_t i) const {
		assert(i < 3);
		return (&x)[i];
	}

	ivec4::ivec4():
			x{0}, y{0}, z{0}, w{0} {}

	ivec4::ivec4(i32 x_, i32 y_, i32 z_, i32 w_):
			x{x_}, y{y_}, z{z_}, w{w_} {}

	ivec4::ivec4(const ivec3& v, i32 w_):
			x{v.x}, y{v.y}, z{v.z}, w{w_} {}

	ivec4::ivec4(const ivec2& v):
			x{v.x}, y{v.y}, z{0}, w{0} {}

	ivec4::ivec4(const ivec3& v):
			x{v.x}, y{v.y}, z{v.z}, w{0} {}

	i32& ivec4::operator[](std::size_t i) {
		assert(i < 4);
		return (&x)[i];
	}

	const i32& ivec4::operator[](std::size_t i) const {
		assert(i < 4);
		return (&x)[i];
	}

	uvec2::uvec2():
			x{0}, y{0} {}

	uvec2::uvec2(u32 x_, u32 y_) :
			x{x_}, y{y_} {}

	uvec2::uvec2(const uvec3& v):
			x{v.x}, y{v.y} {}

	uvec2::uvec2(const uvec4& v):
			x{v.x}, y{v.y} {}

	u32& uvec2::operator[](std::size_t i) {
		assert(i < 2);
		return (&x)[i];
	}

	const u32& uvec2::operator[](std::size_t i) const {
		assert(i < 2);
		return (&x)[i];
	}

	uvec3::uvec3():
			x{0}, y{0}, z{0} {}

	uvec3::uvec3(u32 x_, u32 y_, u32 z_):
			x{x_}, y{y_}, z{z_} {}

	uvec3::uvec3(const uvec2& v, u32 z_):
			x{v.x}, y{v.y}, z{z_} {}

	uvec3::uvec3(const uvec2& v):
			x{v.x}, y{v.y}, z{0} {}

	uvec3::uvec3(const uvec4& v):
			x{v.x}, y{v.y}, z{v.z} {}

	u32& uvec3::operator[](std::size_t i) {
		assert(i < 3);
		return (&x)[i];
	}

	const u32& uvec3::operator[](std::size_t i) const {
		assert(i < 3);
		return (&x)[i];
	}

	uvec4::uvec4():
			x{0}, y{0}, z{0}, w{0} {}

	uvec4::uvec4(u32 x_, u32 y_, u32 z_, u32 w_):
			x{x_}, y{y_}, z{z_}, w{w_} {}

	uvec4::uvec4(const uvec3& v, u32 w_):
			x{v.x}, y{v.y}, z{v.z}, w{w_} {}

	uvec4::uvec4(const uvec2& v):
			x{v.x}, y{v.y}, z{0}, w{0} {}

	uvec4::uvec4(const uvec3& v):
			x{v.x}, y{v.y}, z{v.z}, w{0} {}

	u32& uvec4::operator[](std::size_t i) {
		assert(i < 4);
		return (&x)[i];
	}

	const u32& uvec4::operator[](std::size_t i) const {
		assert(i < 4);
		return (&x)[i];
	}

	u8vec2::u8vec2():
			x{0}, y{0} {}

	u8vec2::u8vec2(u8 x_, u8 y_) :
			x{x_}, y{y_} {}

	u8vec2::u8vec2(const u8vec3& v):
			x{v.x}, y{v.y} {}

	u8vec2::u8vec2(const u8vec4& v):
			x{v.x}, y{v.y} {}

	u8& u8vec2::operator[](std::size_t i) {
		assert(i < 2);
		return (&x)[i];
	}

	const u8& u8vec2::operator[](std::size_t i) const {
		assert(i < 2);
		return (&x)[i];
	}

	u8vec3::u8vec3():
			x{0}, y{0}, z{0} {}

	u8vec3::u8vec3(u8 x_, u8 y_, u8 z_):
			x{x_}, y{y_}, z{z_} {}

	u8vec3::u8vec3(const u8vec2& v, u8 z_):
			x{v.x}, y{v.y}, z{z_} {}

	u8vec3::u8vec3(const u8vec2& v):
			x{v.x}, y{v.y}, z{0} {}

	u8vec3::u8vec3(const u8vec4& v):
			x{v.x}, y{v.y}, z{v.z} {}

	u8& u8vec3::operator[](std::size_t i) {
		assert(i < 3);
		return (&x)[i];
	}

	const u8& u8vec3::operator[](std::size_t i) const {
		assert(i < 3);
		return (&x)[i];
	}

	u8vec4::u8vec4():
			x{0}, y{0}, z{0}, w{0} {}

	u8vec4::u8vec4(u8 x_, u8 y_, u8 z_, u8 w_):
			x{x_}, y{y_}, z{z_}, w{w_} {}

	u8vec4::u8vec4(const u8vec3& v, u8 w_):
			x{v.x}, y{v.y}, z{v.z}, w{w_} {}

	u8vec4::u8vec4(const u8vec2& v):
			x{v.x}, y{v.y}, z{0}, w{0} {}

	u8vec4::u8vec4(const u8vec3& v):
			x{v.x}, y{v.y}, z{v.z}, w{0} {}

	u8& u8vec4::operator[](std::size_t i) {
		assert(i < 4);
		return (&x)[i];
	}

	const u8& u8vec4::operator[](std::size_t i) const {
		assert(i < 4);
		return (&x)[i];
	}

	mat2::mat2():
			c0{1,0},
			c1{0,1} {}

	mat2::mat2(f32 diag)
			: c0{diag, 0.0f}, c1{0.0f, diag} {}

	vec2& mat2::operator[](std::size_t i) {
		assert(i < 2);
		return (&c0)[i];
	}

	const vec2& mat2::operator[](std::size_t i) const {
		assert(i < 2);
		return (&c0)[i];
	}

	mat3::mat3():
			c0{1,0,0},
			c1{0,1,0},
			c2{0,0,1} {}

	mat3::mat3(f32 diag) :
			c0{diag, 0.0f, 0.0f},
			c1{0.0f, diag, 0.0f},
			c2{0.0f, 0.0f, diag} {}

	vec3& mat3::operator[](std::size_t i) {
		assert(i < 3);
		return (&c0)[i];
	}

	const vec3& mat3::operator[](std::size_t i) const {
		assert(i < 3);
		return (&c0)[i];
	}

	mat4::mat4():
			c0{1,0,0,0},
			c1{0,1,0,0},
			c2{0,0,1,0},
			c3{0,0,0,1} {}

	mat4::mat4(f32 diag) :
			c0{diag, 0.0f, 0.0f, 0.0f},
			c1{0.0f, diag, 0.0f, 0.0f},
			c2{0.0f, 0.0f, diag, 0.0f},
			c3{0.0f, 0.0f, 0.0f, diag} {}

	mat4::mat4(const mat3 &m) :
			c0{m.c0.x, m.c0.y, m.c0.z, 0.0f},
			c1{m.c1.x, m.c1.y, m.c1.z, 0.0f},
			c2{m.c2.x, m.c2.y, m.c2.z, 0.0f},
			c3{0.0f, 0.0f, 0.0f, 1.0f} {}

	vec4& mat4::operator[](std::size_t i) {
		assert(i < 4);
		return (&c0)[i];
	}

	const vec4& mat4::operator[](std::size_t i) const {
		assert(i < 4);
		return (&c0)[i];
	}

	quat::quat():
			w{1.0f}, x{0.0f}, y{0.0f}, z{0.0f} {}

	quat::quat(f32 w_, f32 x_, f32 y_, f32 z_) :
			w{w_}, x{x_}, y{y_}, z{z_} {}

	quat::quat(f32 s):
			w{s}, x{0.0f}, y{0.0f}, z{0.0f} {}

	f32& quat::operator[](std::size_t i) {
		assert(i < 4);
		return (&w)[i];
	}

	const f32& quat::operator[](std::size_t i) const {
		assert(i < 4);
		return (&w)[i];
	}

	/// 32-bit Float Vector ///

	vec2 operator-(const vec2& v) {
		return { -v.x, -v.y };
	}

	vec3 operator-(const vec3& v) {
		return { -v.x, -v.y, -v.z };
	}

	vec4 operator-(const vec4& v) {
		return { -v.x, -v.y, -v.z, -v.w };
	}

	vec2 operator+(const vec2 &a, const vec2 &b) {
		return vec2{a.x + b.x, a.y + b.y};
	}

	vec3 operator+(const vec3 &a, const vec3 &b) {
		return vec3{a.x + b.x, a.y + b.y, a.z + b.z};
	}

	vec4 operator+(const vec4 &a, const vec4 &b) {
		return vec4{a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w};
	}

	vec2 operator-(const vec2 &a, const vec2 &b) {
		return vec2{a.x - b.x, a.y - b.y};
	}

	vec3 operator-(const vec3 &a, const vec3 &b) {
		return vec3{a.x - b.x, a.y - b.y, a.z - b.z};
	}

	vec4 operator-(const vec4 &a, const vec4 &b) {
		return vec4{a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w};
	}

	vec2 operator*(const vec2 &v, f32 s) {
		return vec2{v.x * s, v.y * s};
	}

	vec3 operator*(const vec3 &v, f32 s) {
		return vec3{v.x * s, v.y * s, v.z * s};
	}

	vec4 operator*(const vec4 &v, f32 s) {
		return vec4{v.x * s, v.y * s, v.z * s, v.w * s};
	}

	vec2 operator*(f32 s, const vec2 &v) {
		return v * s;
	}

	vec3 operator*(f32 s, const vec3 &v) {
		return v * s;
	}

	vec4 operator*(f32 s, const vec4 &v) {
		return v * s;
	}

	vec2 operator/(const vec2 &v, f32 s) {
		f32 inv = 1.0f / s;
		return vec2{v.x * inv, v.y * inv};
	}

	vec3 operator/(const vec3 &v, f32 s) {
		f32 inv = 1.0f / s;
		return vec3{v.x * inv, v.y * inv, v.z * inv};
	}

	vec4 operator/(const vec4 &v, f32 s) {
		f32 inv = 1.0f / s;
		return vec4{v.x * inv, v.y * inv, v.z * inv, v.w * inv};
	}

	vec2& operator+=(vec2& a, const vec2& b) {
		a.x += b.x;
		a.y += b.y;
		return a;
	}

	vec3& operator+=(vec3& a, const vec3& b) {
		a.x += b.x;
		a.y += b.y;
		a.z += b.z;
		return a;
	}

	vec4& operator+=(vec4& a, const vec4& b) {
		a.x += b.x;
		a.y += b.y;
		a.z += b.z;
		a.w += b.w;
		return a;
	}

	vec2& operator-=(vec2& a, const vec2& b) {
		a.x -= b.x;
		a.y -= b.y;
		return a;
	}

	vec3& operator-=(vec3& a, const vec3& b) {
		a.x -= b.x;
		a.y -= b.y;
		a.z -= b.z;
		return a;
	}

	vec4& operator-=(vec4& a, const vec4& b) {
		a.x -= b.x;
		a.y -= b.y;
		a.z -= b.z;
		a.w -= b.w;
		return a;
	}

	vec2& operator*=(vec2& v, f32 s) {
		v.x *= s;
		v.y *= s;
		return v;
	}

	vec3& operator*=(vec3& v, f32 s) {
		v.x *= s;
		v.y *= s;
		v.z *= s;
		return v;
	}

	vec4& operator*=(vec4& v, f32 s) {
		v.x *= s;
		v.y *= s;
		v.z *= s;
		v.w *= s;
		return v;
	}

	vec2& operator/=(vec2& v, f32 s) {
		f32 inv = 1.0f / s;
		v.x *= inv;
		v.y *= inv;
		return v;
	}

	vec3& operator/=(vec3& v, f32 s) {
		f32 inv = 1.0f / s;
		v.x *= inv;
		v.y *= inv;
		v.z *= inv;
		return v;
	}

	vec4& operator/=(vec4& v, f32 s) {
		f32 inv = 1.0f / s;
		v.x *= inv;
		v.y *= inv;
		v.z *= inv;
		v.w *= inv;
		return v;
	}

	f32 dot(const vec2 &a, const vec2 &b) {
		return a.x * b.x + a.y * b.y;
	}

	f32 dot(const vec3 &a, const vec3 &b) {
		return a.x * b.x + a.y * b.y + a.z * b.z;
	}

	f32 dot(const vec4 &a, const vec4 &b) {
		return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
	}

	f32 length(const vec2 &v) {
		return sqrt(dot(v, v));
	}

	f32 length(const vec3 &v) {
		return sqrt(dot(v, v));
	}

	f32 length(const vec4 &v) {
		return sqrt(dot(v, v));
	}

	vec2 normalize(const vec2 &v) {
		f32 len = length(v);
		assert(len != 0.0f && "math::normalize called on zero-length vec2");
		return (len > 0.0f) ? v / len : v;
	}

	vec3 normalize(const vec3 &v) {
		f32 len = length(v);
		assert(len != 0.0f && "math::normalize called on zero-length vec3");
		return (len > 0.0f) ? v / len : v;
	}

	vec4 normalize(const vec4 &v) {
		f32 len = length(v);
		assert(len != 0.0f && "math::normalize called on zero-length vec4");
		return (len > 0.0f) ? v / len : v;
	}

	vec3 cross(const vec3 &a, const vec3 &b) {
		return vec3{a.y * b.z - a.z * b.y,
					a.z * b.x - a.x * b.z,
					a.x * b.y - a.y * b.x};
	}

	std::string to_string(const vec2& v) {
		std::ostringstream ss;
		ss << "vec2(" << v.x << ", " << v.y << ")";
		return ss.str();
	}

	std::string to_string(const vec3& v) {
		std::ostringstream ss;
		ss << "vec3(" << v.x << ", " << v.y << ", " << v.z << ")";
		return ss.str();
	}

	std::string to_string(const vec4& v) {
		std::ostringstream ss;
		ss << "vec4(" << v.x << ", " << v.y << ", " << v.z << ", " << v.w << ")";
		return ss.str();
	}

	f32* value_ptr(vec2 &v) {
		return &v.x;
	}

	const f32* value_ptr(const vec2 &v) {
		return &v.x;
	}

	f32* value_ptr(vec3 &v) {
		return &v.x;
	}

	const f32* value_ptr(const vec3 &v) {
		return &v.x;
	}

	f32* value_ptr(vec4 &v) {
		return &v.x;
	}

	const f32* value_ptr(const vec4 &v) {
		return &v.x;
	}

	vec2 make_vec2(const f32* ptr) {
		return vec2{ ptr[0], ptr[1] };
	}

	vec3 make_vec3(const f32* ptr) {
		return vec3{ ptr[0], ptr[1], ptr[2] };
	}

	vec4 make_vec4(const f32* ptr) {
		return vec4{ ptr[0], ptr[1], ptr[2], ptr[3] };
	}

	/// Signed 32-bit Integer Vector ///

	ivec2 operator-(const ivec2& v) {
		return { -v.x, -v.y };
	}

	ivec3 operator-(const ivec3& v) {
		return { -v.x, -v.y, -v.z };
	}

	ivec4 operator-(const ivec4& v) {
		return { -v.x, -v.y, -v.z, -v.w };
	}

	ivec2 operator+(const ivec2 &a, const ivec2 &b) {
		return ivec2{a.x + b.x, a.y + b.y};
	}

	ivec3 operator+(const ivec3 &a, const ivec3 &b) {
		return ivec3{a.x + b.x, a.y + b.y, a.z + b.z};
	}

	ivec4 operator+(const ivec4 &a, const ivec4 &b) {
		return ivec4{a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w};
	}

	ivec2 operator-(const ivec2 &a, const ivec2 &b) {
		return ivec2{a.x - b.x, a.y - b.y};
	}

	ivec3 operator-(const ivec3 &a, const ivec3 &b) {
		return ivec3{a.x - b.x, a.y - b.y, a.z - b.z};
	}

	ivec4 operator-(const ivec4 &a, const ivec4 &b) {
		return ivec4{a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w};
	}

	ivec2 operator*(const ivec2 &v, i32 s) {
		return ivec2{v.x * s, v.y * s};
	}

	ivec3 operator*(const ivec3 &v, i32 s) {
		return ivec3{v.x * s, v.y * s, v.z * s};
	}

	ivec4 operator*(const ivec4 &v, i32 s) {
		return ivec4{v.x * s, v.y * s, v.z * s, v.w * s};
	}

	ivec2 operator*(i32 s, const ivec2 &v) {
		return v * s;
	}

	ivec3 operator*(i32 s, const ivec3 &v) {
		return v * s;
	}

	ivec4 operator*(i32 s, const ivec4 &v) {
		return v * s;
	}

	ivec2 operator/(const ivec2 &v, i32 s) {
		// no zero check -- assert?
		return ivec2{v.x / s, v.y / s};
	}

	ivec3 operator/(const ivec3 &v, i32 s) {
		return ivec3{v.x / s, v.y / s, v.z / s};
	}

	ivec4 operator/(const ivec4 &v, i32 s) {
		return ivec4{v.x / s, v.y / s, v.z / s, v.w / s};
	}

	ivec2& operator+=(ivec2& a, const ivec2& b) {
		a.x += b.x;
		a.y += b.y;
		return a;
	}

	ivec3& operator+=(ivec3& a, const ivec3& b) {
		a.x += b.x;
		a.y += b.y;
		a.z += b.z;
		return a;
	}

	ivec4& operator+=(ivec4& a, const ivec4& b) {
		a.x += b.x;
		a.y += b.y;
		a.z += b.z;
		a.w += b.w;
		return a;
	}

	ivec2& operator-=(ivec2& a, const ivec2& b) {
		a.x -= b.x;
		a.y -= b.y;
		return a;
	}

	ivec3& operator-=(ivec3& a, const ivec3& b) {
		a.x -= b.x;
		a.y -= b.y;
		a.z -= b.z;
		return a;
	}

	ivec4& operator-=(ivec4& a, const ivec4& b) {
		a.x -= b.x;
		a.y -= b.y;
		a.z -= b.z;
		a.w -= b.w;
		return a;
	}

	ivec2& operator*=(ivec2& v, i32 s) {
		v.x *= s;
		v.y *= s;
		return v;
	}

	ivec3& operator*=(ivec3& v, i32 s) {
		v.x *= s;
		v.y *= s;
		v.z *= s;
		return v;
	}

	ivec4& operator*=(ivec4& v, i32 s) {
		v.x *= s;
		v.y *= s;
		v.z *= s;
		v.w *= s;
		return v;
	}

	ivec2& operator/=(ivec2& v, i32 s) {
		v.x /= s;
		v.y /= s;
		return v;
	}

	ivec3& operator/=(ivec3& v, i32 s) {
		v.x /= s;
		v.y /= s;
		v.z /= s;
		return v;
	}

	ivec4& operator/=(ivec4& v, i32 s) {
		v.x /= s;
		v.y /= s;
		v.z /= s;
		v.w /= s;
		return v;
	}

	i32 dot(const ivec2 &a, const ivec2 &b) {
		return a.x * b.x + a.y * b.y;
	}

	i32 dot(const ivec3 &a, const ivec3 &b) {
		return a.x * b.x + a.y * b.y + a.z * b.z;
	}

	i32 dot(const ivec4 &a, const ivec4 &b) {
		return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
	}

	std::string to_string(const ivec2& v) {
		std::ostringstream ss;
		ss << "ivec2(" << v.x << ", " << v.y << ")";
		return ss.str();
	}

	std::string to_string(const ivec3& v) {
		std::ostringstream ss;
		ss << "ivec3(" << v.x << ", " << v.y << ", " << v.z << ")";
		return ss.str();
	}

	std::string to_string(const ivec4& v) {
		std::ostringstream ss;
		ss << "ivec4(" << v.x << ", " << v.y << ", " << v.z << ", " << v.w << ")";
		return ss.str();
	}

	i32* value_ptr(ivec2 &v) {
		return &v.x;
	}

	const i32* value_ptr(const ivec2 &v) {
		return &v.x;
	}

	i32* value_ptr(ivec3 &v) {
		return &v.x;
	}

	const i32* value_ptr(const ivec3 &v) {
		return &v.x;
	}

	i32* value_ptr(ivec4 &v) {
		return &v.x;
	}

	const i32* value_ptr(const ivec4 &v) {
		return &v.x;
	}

	ivec2 make_ivec2(const i32* ptr) {
		return ivec2{ ptr[0], ptr[1] };
	}

	ivec3 make_ivec3(const i32* ptr) {
		return ivec3{ ptr[0], ptr[1], ptr[2] };
	}

	ivec4 make_ivec4(const i32* ptr) {
		return ivec4{ ptr[0], ptr[1], ptr[2], ptr[3] };
	}

	/// Unsigned 32-bit Integer Vector ///

	uvec2 operator+(const uvec2 &a, const uvec2 &b) {
		return uvec2{a.x + b.x, a.y + b.y};
	}

	uvec3 operator+(const uvec3 &a, const uvec3 &b) {
		return uvec3{a.x + b.x, a.y + b.y, a.z + b.z};
	}

	uvec4 operator+(const uvec4 &a, const uvec4 &b) {
		return uvec4{a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w};
	}

	uvec2 operator-(const uvec2 &a, const uvec2 &b) {
		return uvec2{a.x - b.x, a.y - b.y};
	}

	uvec3 operator-(const uvec3 &a, const uvec3 &b) {
		return uvec3{a.x - b.x, a.y - b.y, a.z - b.z};
	}

	uvec4 operator-(const uvec4 &a, const uvec4 &b) {
		return uvec4{a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w};
	}

	uvec2 operator*(const uvec2 &v, u32 s) {
		return uvec2{v.x * s, v.y * s};
	}

	uvec3 operator*(const uvec3 &v, u32 s) {
		return uvec3{v.x * s, v.y * s, v.z * s};
	}

	uvec4 operator*(const uvec4 &v, u32 s) {
		return uvec4{v.x * s, v.y * s, v.z * s, v.w * s};
	}

	uvec2 operator*(u32 s, const uvec2 &v) {
		return v * s;
	}

	uvec3 operator*(u32 s, const uvec3 &v) {
		return v * s;
	}

	uvec4 operator*(u32 s, const uvec4 &v) {
		return v * s;
	}

	uvec2 operator/(const uvec2 &v, u32 s) {
		// no zero check -- assert?
		return uvec2{v.x / s, v.y / s};
	}

	uvec3 operator/(const uvec3 &v, u32 s) {
		return uvec3{v.x / s, v.y / s, v.z / s};
	}

	uvec4 operator/(const uvec4 &v, u32 s) {
		return uvec4{v.x / s, v.y / s, v.z / s, v.w / s};
	}

	uvec2& operator+=(uvec2& a, const uvec2& b) {
		a.x += b.x;
		a.y += b.y;
		return a;
	}

	uvec3& operator+=(uvec3& a, const uvec3& b) {
		a.x += b.x;
		a.y += b.y;
		a.z += b.z;
		return a;
	}

	uvec4& operator+=(uvec4& a, const uvec4& b) {
		a.x += b.x;
		a.y += b.y;
		a.z += b.z;
		a.w += b.w;
		return a;
	}

	uvec2& operator-=(uvec2& a, const uvec2& b) {
		a.x -= b.x;
		a.y -= b.y;
		return a;
	}

	uvec3& operator-=(uvec3& a, const uvec3& b) {
		a.x -= b.x;
		a.y -= b.y;
		a.z -= b.z;
		return a;
	}

	uvec4& operator-=(uvec4& a, const uvec4& b) {
		a.x -= b.x;
		a.y -= b.y;
		a.z -= b.z;
		a.w -= b.w;
		return a;
	}

	uvec2& operator*=(uvec2& v, u32 s) {
		v.x *= s;
		v.y *= s;
		return v;
	}

	uvec3& operator*=(uvec3& v, u32 s) {
		v.x *= s;
		v.y *= s;
		v.z *= s;
		return v;
	}

	uvec4& operator*=(uvec4& v, u32 s) {
		v.x *= s;
		v.y *= s;
		v.z *= s;
		v.w *= s;
		return v;
	}

	uvec2& operator/=(uvec2& v, u32 s) {
		v.x /= s;
		v.y /= s;
		return v;
	}

	uvec3& operator/=(uvec3& v, u32 s) {
		v.x /= s;
		v.y /= s;
		v.z /= s;
		return v;
	}

	uvec4& operator/=(uvec4& v, u32 s) {
		v.x /= s;
		v.y /= s;
		v.z /= s;
		v.w /= s;
		return v;
	}

	u32 dot(const uvec2 &a, const uvec2 &b) {
		return a.x * b.x + a.y * b.y;
	}

	u32 dot(const uvec3 &a, const uvec3 &b) {
		return a.x * b.x + a.y * b.y + a.z * b.z;
	}

	u32 dot(const uvec4 &a, const uvec4 &b) {
		return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
	}

	std::string to_string(const uvec2& v) {
		std::ostringstream ss;
		ss << "uvec2(" << v.x << ", " << v.y << ")";
		return ss.str();
	}

	std::string to_string(const uvec3& v) {
		std::ostringstream ss;
		ss << "uvec3(" << v.x << ", " << v.y << ", " << v.z << ")";
		return ss.str();
	}

	std::string to_string(const uvec4& v) {
		std::ostringstream ss;
		ss << "uvec4(" << v.x << ", " << v.y << ", " << v.z << ", " << v.w << ")";
		return ss.str();
	}

	u32* value_ptr(uvec2 &v) {
		return &v.x;
	}

	const u32* value_ptr(const uvec2 &v) {
		return &v.x;
	}

	u32* value_ptr(uvec3 &v) {
		return &v.x;
	}

	const u32* value_ptr(const uvec3 &v) {
		return &v.x;
	}

	u32* value_ptr(uvec4 &v) {
		return &v.x;
	}

	const u32* value_ptr(const uvec4 &v) {
		return &v.x;
	}

	uvec2 make_uvec2(const u32* ptr) {
		return uvec2{ ptr[0], ptr[1] };
	}

	uvec3 make_uvec3(const u32* ptr) {
		return uvec3{ ptr[0], ptr[1], ptr[2] };
	}

	uvec4 make_uvec4(const u32* ptr) {
		return uvec4{ ptr[0], ptr[1], ptr[2], ptr[3] };
	}

	/// Unsigned 8-bit Integer Vector ///

	std::string to_string(const u8vec2& v) {
		std::ostringstream ss;
		ss << "u8vec2(" << static_cast<unsigned>(v.x)
		   << ", " << static_cast<unsigned>(v.y) << ")";
		return ss.str();
	}

	std::string to_string(const u8vec3& v) {
		std::ostringstream ss;
		ss << "u8vec3(" << static_cast<unsigned>(v.x)
		   << ", " << static_cast<unsigned>(v.y)
		   << ", " << static_cast<unsigned>(v.z) << ")";
		return ss.str();
	}

	std::string to_string(const u8vec4& v) {
		std::ostringstream ss;
		ss << "u8vec4(" << static_cast<unsigned>(v.x)
		   << ", " << static_cast<unsigned>(v.y)
		   << ", " << static_cast<unsigned>(v.z)
		   << ", " << static_cast<unsigned>(v.w) << ")";
		return ss.str();
	}

	/// 32-bit Float Matrix ///

	mat2 identity2() {
		return mat2(1.0f);
	}

	mat3 identity3() {
		return mat3(1.0f);
	}

	mat4 identity4() {
		return mat4(1.0f);
	}

	mat2 zero2() {
		return mat2(0.0f);
	}

	mat3 zero3() {
		return mat3(0.0f);
	}

	mat4 zero4() {
		return mat4(0.0f);
	}

	vec2 operator*(const mat2 &m, const vec2 &v) {
		return vec2{m.c0.x * v.x + m.c1.x * v.y,
					m.c0.y * v.x + m.c1.y * v.y};
	}

	vec3 operator*(const mat3 &m, const vec3 &v) {
		return vec3{m.c0.x * v.x + m.c1.x * v.y + m.c2.x * v.z,
					m.c0.y * v.x + m.c1.y * v.y + m.c2.y * v.z,
					m.c0.z * v.x + m.c1.z * v.y + m.c2.z * v.z};
	}

	vec4 operator*(const mat4 &m, const vec4 &v) {
		return vec4{m.c0.x * v.x + m.c1.x * v.y + m.c2.x * v.z + m.c3.x * v.w,
					m.c0.y * v.x + m.c1.y * v.y + m.c2.y * v.z + m.c3.y * v.w,
					m.c0.z * v.x + m.c1.z * v.y + m.c2.z * v.z + m.c3.z * v.w,
					m.c0.w * v.x + m.c1.w * v.y + m.c2.w * v.z + m.c3.w * v.w};
	}

	mat2 operator*(const mat2 &a, const mat2 &b) {
		mat2 r(0.0f);
		r.c0 = a * b.c0;
		r.c1 = a * b.c1;
		return r;
	}

	mat3 operator*(const mat3 &a, const mat3 &b) {
		mat3 r(0.0f);
		r.c0 = a * b.c0;
		r.c1 = a * b.c1;
		r.c2 = a * b.c2;
		return r;
	}

	mat4 operator*(const mat4 &a, const mat4 &b) {
		mat4 r(0.0f);
		r.c0 = a * b.c0;
		r.c1 = a * b.c1;
		r.c2 = a * b.c2;
		r.c3 = a * b.c3;
		return r;
	}

	mat2& operator*=(mat2& a, const mat2& b) {
		a = a * b;
		return a;
	}

	mat3& operator*=(mat3& a, const mat3& b) {
		a = a * b;
		return a;
	}

	mat4& operator*=(mat4& a, const mat4& b) {
		a = a * b;
		return a;
	}

	mat2 transpose(const mat2 &m) {
		mat2 r;
		r.c0 = vec2{m.c0.x, m.c1.x};
		r.c1 = vec2{m.c0.y, m.c1.y};
		return r;
	}

	mat3 transpose(const mat3 &m) {
		mat3 r;
		r.c0 = vec3{m.c0.x, m.c1.x, m.c2.x};
		r.c1 = vec3{m.c0.y, m.c1.y, m.c2.y};
		r.c2 = vec3{m.c0.z, m.c1.z, m.c2.z};
		return r;
	}

	mat4 transpose(const mat4 &m) {
		mat4 r;
		r.c0 = vec4{m.c0.x, m.c1.x, m.c2.x, m.c3.x};
		r.c1 = vec4{m.c0.y, m.c1.y, m.c2.y, m.c3.y};
		r.c2 = vec4{m.c0.z, m.c1.z, m.c2.z, m.c3.z};
		r.c3 = vec4{m.c0.w, m.c1.w, m.c2.w, m.c3.w};
		return r;
	}

	f32 determinant(const mat2& m) {
		return m[0][0] * m[1][1] - m[1][0] * m[0][1];;
	}

	f32 determinant(const mat3& m) {
		return
				+ m[0][0] * (m[1][1] * m[2][2] - m[2][1] * m[1][2])
				- m[1][0] * (m[0][1] * m[2][2] - m[2][1] * m[0][2])
				+ m[2][0] * (m[0][1] * m[1][2] - m[1][1] * m[0][2]);
	}

	f32 determinant(const mat4& m) {
		f32 subFactor00 = m[2][2] * m[3][3] - m[3][2] * m[2][3];
		f32 subFactor01 = m[2][1] * m[3][3] - m[3][1] * m[2][3];
		f32 subFactor02 = m[2][1] * m[3][2] - m[3][1] * m[2][2];
		f32 subFactor03 = m[2][0] * m[3][3] - m[3][0] * m[2][3];
		f32 subFactor04 = m[2][0] * m[3][2] - m[3][0] * m[2][2];
		f32 subFactor05 = m[2][0] * m[3][1] - m[3][0] * m[2][1];

		vec4 detCof(
				+ (m[1][1] * subFactor00 - m[1][2] * subFactor01 + m[1][3] * subFactor02),
				- (m[1][0] * subFactor00 - m[1][2] * subFactor03 + m[1][3] * subFactor04),
				+ (m[1][0] * subFactor01 - m[1][1] * subFactor03 + m[1][3] * subFactor05),
				- (m[1][0] * subFactor02 - m[1][1] * subFactor04 + m[1][2] * subFactor05));

		return
				m[0][0] * detCof[0] + m[0][1] * detCof[1] +
				m[0][2] * detCof[2] + m[0][3] * detCof[3];
	}

	mat2 inverse(const mat2 &m) {
		f32 a = m.c0.x, c = m.c0.y;
		f32 b = m.c1.x, d = m.c1.y;

		f32 det = a * d - b * c;
		assert(det != 0.0f);
		f32 invDet = 1.0f / det;

		mat2 r;
		r.c0 = vec2{d * invDet, -c * invDet};
		r.c1 = vec2{-b * invDet, a * invDet};
		return r;
	}

	mat3 inverse(const mat3 &m) {
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
		assert(det != 0.0f);
		f32 invDet = 1.0f / det;

		mat3 r;
		r.c0 = vec3{inv00 * invDet, inv10 * invDet, inv20 * invDet};
		r.c1 = vec3{inv01 * invDet, inv11 * invDet, inv21 * invDet};
		r.c2 = vec3{inv02 * invDet, inv12 * invDet, inv22 * invDet};
		return r;
	}

	mat4 inverse(const mat4 &m) {

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
			f32 max_abs = std::fabs(a[col][col]);
			for (int r = col + 1; r < 4; ++r) {
				f32 val = std::fabs(a[r][col]);
				if (val > max_abs) {
					max_abs = val;
					pivot_row = r;
				}
			}

			// singular?
			assert(max_abs != 0.0f && "mat4 inverse: matrix is singular");

			// swap rows in both 'a' and 'inv'
			if (pivot_row != col) {
				std::swap(a[col], a[pivot_row]);
				std::swap(inv[col], inv[pivot_row]);
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
				if (r == col) continue;
				const f32 factor = a[r][col];
				if (factor == 0.0f) continue;
				for (int j = 0; j < 4; ++j) {
					a[r][j] -= factor * a[col][j];
					inv[r][j] -= factor * inv[col][j];
				}
			}
		}

		// 'inv' is now row-major inverse; convert back to column-major mat4
		mat4 r(0.0);
		r.c0 = vec4{inv[0][0], inv[1][0], inv[2][0], inv[3][0]};
		r.c1 = vec4{inv[0][1], inv[1][1], inv[2][1], inv[3][1]};
		r.c2 = vec4{inv[0][2], inv[1][2], inv[2][2], inv[3][2]};
		r.c3 = vec4{inv[0][3], inv[1][3], inv[2][3], inv[3][3]};
		return r;
	}

	mat4 translate(const mat4& m, const vec3& v) {
		mat4 result(m);
		result[3] = m[0] * v[0] + m[1] * v[1] + m[2] * v[2] + m[3];
		return result;
	}

	mat4 rotate(const mat4& m, f32 angle, const vec3& v) {
		const f32 a = angle;
		const f32 c = cos(a);
		const f32 s = sin(a);

		vec3 axis(normalize(v));
		vec3 temp((f32(1) - c) * axis);

		mat4 rotate;
		rotate[0][0] = c + temp[0] * axis[0];
		rotate[0][1] = temp[0] * axis[1] + s * axis[2];
		rotate[0][2] = temp[0] * axis[2] - s * axis[1];

		rotate[1][0] = temp[1] * axis[0] - s * axis[2];
		rotate[1][1] = c + temp[1] * axis[1];
		rotate[1][2] = temp[1] * axis[2] + s * axis[0];

		rotate[2][0] = temp[2] * axis[0] + s * axis[1];
		rotate[2][1] = temp[2] * axis[1] - s * axis[0];
		rotate[2][2] = c + temp[2] * axis[2];

		mat4 result;
		result[0] = m[0] * rotate[0][0] + m[1] * rotate[0][1] + m[2] * rotate[0][2];
		result[1] = m[0] * rotate[1][0] + m[1] * rotate[1][1] + m[2] * rotate[1][2];
		result[2] = m[0] * rotate[2][0] + m[1] * rotate[2][1] + m[2] * rotate[2][2];
		result[3] = m[3];
		return result;
	}

	mat4 scale(const mat4& m, const vec3& v){
		mat4 result;
		result[0] = m[0] * v[0];
		result[1] = m[1] * v[1];
		result[2] = m[2] * v[2];
		result[3] = m[3];
		return result;
	}

	mat4 scale(const mat4& m, f32 s) {
		return scale(m, vec3{s, s, s});
	}

	std::string to_string(const mat2& m) {
		std::ostringstream ss;
		ss << "mat2("
		   << "c0=(" << m.c0.x << ", " << m.c0.y << "), "
		   << "c1=(" << m.c1.x << ", " << m.c1.y << "))";
		return ss.str();
	}

	std::string to_string(const mat3& m) {
		std::ostringstream ss;
		ss << "mat3("
		   << "c0=(" << m.c0.x << ", " << m.c0.y << ", " << m.c0.z << "), "
		   << "c1=(" << m.c1.x << ", " << m.c1.y << ", " << m.c1.z << "), "
		   << "c2=(" << m.c2.x << ", " << m.c2.y << ", " << m.c2.z << "))";
		return ss.str();
	}

	std::string to_string(const mat4& m) {
		std::ostringstream ss;
		ss << "mat4("
		   << "c0=(" << m.c0.x << ", " << m.c0.y << ", " << m.c0.z << ", " << m.c0.w << "), "
		   << "c1=(" << m.c1.x << ", " << m.c1.y << ", " << m.c1.z << ", " << m.c1.w << "), "
		   << "c2=(" << m.c2.x << ", " << m.c2.y << ", " << m.c2.z << ", " << m.c2.w << "), "
		   << "c3=(" << m.c3.x << ", " << m.c3.y << ", " << m.c3.z << ", " << m.c3.w << "))";
		return ss.str();
	}

	f32* value_ptr(mat2 &m) {
		return &m.c0.x;
	}

	const f32* value_ptr(const mat2 &m) {
		return &m.c0.x;
	}

	f32* value_ptr(mat3 &m) {
		return &m.c0.x;
	}

	const f32* value_ptr(const mat3 &m) {
		return &m.c0.x;
	}

	f32* value_ptr(mat4 &m) {
		return &m.c0.x;
	}

	const f32* value_ptr(const mat4 &m) {
		return &m.c0.x;
	}

	mat2 make_mat2(const f32* ptr) {
		mat2 m;
		m.c0 = vec2{ ptr[0], ptr[1] };
		m.c1 = vec2{ ptr[2], ptr[3] };
		return m;
	}

	mat3 make_mat3(const f32* ptr) {
		mat3 m;
		m.c0 = vec3{ ptr[0], ptr[1], ptr[2] };
		m.c1 = vec3{ ptr[3], ptr[4], ptr[5] };
		m.c2 = vec3{ ptr[6], ptr[7], ptr[8] };
		return m;
	}

	mat4 make_mat4(const f32* ptr) {
		mat4 m;
		m.c0 = vec4{ ptr[0],  ptr[1],  ptr[2],  ptr[3]  };
		m.c1 = vec4{ ptr[4],  ptr[5],  ptr[6],  ptr[7]  };
		m.c2 = vec4{ ptr[8],  ptr[9],  ptr[10], ptr[11] };
		m.c3 = vec4{ ptr[12], ptr[13], ptr[14], ptr[15] };
		return m;
	}

	/// 32-bit Float Quaternion ///

	quat identity_quat() {
		return quat{1.0f, 0.0f, 0.0f, 0.0f};
	}

	f32 dot(const quat &a, const quat &b) {
		return a.w * b.w + a.x * b.x + a.y * b.y + a.z * b.z;
	}

	f32 length(const quat &q) {
		return std::sqrt(dot(q, q));
	}

	quat normalize(const quat &q) {
		f32 len = length(q);
		if (len == 0.0f) {
			// return identity if length is zero -- different behavior?
			return identity_quat();
		}
		f32 inv = 1.0f / len;
		return quat{q.w * inv, q.x * inv, q.y * inv, q.z * inv};
	}

	quat conjugate(const quat &q) {
		return quat{q.w, -q.x, -q.y, -q.z};
	}

	quat inverse(const quat &q) {
		f32 n2 = dot(q, q);
		if (n2 == 0.0f) {
			return identity_quat();
		}
		f32 inv_n2 = 1.0f / n2;
		quat c = conjugate(q);
		return quat{c.w * inv_n2, c.x * inv_n2, c.y * inv_n2, c.z * inv_n2};
	}

	quat operator+(const quat &a, const quat &b) {
		return quat{a.w + b.w, a.x + b.x, a.y + b.y, a.z + b.z};
	}

	quat operator*(const quat &q, f32 s) {
		return quat{q.w * s, q.x * s, q.y * s, q.z * s};
	}

	quat operator*(f32 s, const quat &q) {
		return q * s;
	}

	quat& operator+=(quat& a, const quat& b) {
		a.w += b.w;
		a.x += b.x;
		a.y += b.y;
		a.z += b.z;
		return a;
	}

	quat& operator*=(quat& q, f32 s) {
		q.w *= s;
		q.x *= s;
		q.y *= s;
		q.z *= s;
		return q;
	}

	quat& operator*=(quat& a, const quat& b) {
		a = a * b; // use your Hamilton product
		return a;
	}

	// convention: result = a * b applies b first, then a (GLM-style)
	quat operator*(const quat &a, const quat &b) {
		return quat{a.w * b.w - a.x * b.x - a.y * b.y - a.z * b.z,
					a.w * b.x + a.x * b.w + a.y * b.z - a.z * b.y,
					a.w * b.y - a.x * b.z + a.y * b.w + a.z * b.x,
					a.w * b.z + a.x * b.y - a.y * b.x + a.z * b.w};
	}

	vec3 rotate(const quat &q, const vec3 &v) {
		// using the "u, s" form: q = (s, u)
		vec3 u{q.x, q.y, q.z};
		f32 s = q.w;

		f32 dot_uv = u.x * v.x + u.y * v.y + u.z * v.z; // dot(u, v)
		f32 dot_uu = u.x * u.x + u.y * u.y + u.z * u.z; // dot(u, u)

		vec3 cross_uv{u.y * v.z - u.z * v.y,
					  u.z * v.x - u.x * v.z,
					  u.x * v.y - u.y * v.x};

		// 2 * dot(u, v) * u
		vec3 term1{2.0f * dot_uv * u.x,
				   2.0f * dot_uv * u.y,
				   2.0f * dot_uv * u.z};

		// (s^2 - dot(u, u)) * v
		f32 s2_minus_uu = s * s - dot_uu;
		vec3 term2{s2_minus_uu * v.x,
				   s2_minus_uu * v.y,
				   s2_minus_uu * v.z};

		// 2 * s * cross(u, v)
		vec3 term3{2.0f * s * cross_uv.x,
				   2.0f * s * cross_uv.y,
				   2.0f * s * cross_uv.z};

		return vec3{term1.x + term2.x + term3.x,
					term1.y + term2.y + term3.y,
					term1.z + term2.z + term3.z};
	}

	vec3 operator*(const quat &q, const vec3 &v) {
		return rotate(q, v);
	}

	quat angle_axis(f32 angle, const vec3 &axis) {
		// normalize axis to be safe
		f32 len = std::sqrt(axis.x * axis.x + axis.y * axis.y + axis.z * axis.z);
		if (len == 0.0f) {
			return identity_quat();
		}
		f32 inv_len = 1.0f / len;
		f32 half = 0.5f * angle;
		f32 s = std::sin(half);
		f32 c = std::cos(half);

		vec3 n{axis.x * inv_len,
			   axis.y * inv_len,
			   axis.z * inv_len};

		return quat{c, n.x * s, n.y * s, n.z * s};
	}

	quat slerp(const quat &a, const quat &b, f32 t) {
		// clamp t just in case
		if (t <= 0.0f) return a;
		if (t >= 1.0f) return b;

		quat q1 = normalize(a);
		quat q2 = normalize(b);

		f32 cosTheta = dot(q1, q2);

		// use shortest path
		if (cosTheta < 0.0f) {
			q2 = quat{-q2.w, -q2.x, -q2.y, -q2.z};
			cosTheta = -cosTheta;
		}

		// if very close, fall back to lerp
		const f32 eps = 1e-6f;
		if (cosTheta > 1.0f - eps) {
			quat result = (1.0f - t) * q1 + t * q2;
			return normalize(result);
		}

		f32 theta = std::acos(cosTheta);
		f32 sinTheta = std::sin(theta);

		f32 w1 = std::sin((1.0f - t) * theta) / sinTheta;
		f32 w2 = std::sin(t * theta) / sinTheta;

		quat result = q1 * w1 + q2 * w2;
		return normalize(result);
	}

	mat3 mat3_cast(quat const &q) {

		f32 qxx(q.x * q.x);
		f32 qyy(q.y * q.y);
		f32 qzz(q.z * q.z);
		f32 qxz(q.x * q.z);
		f32 qxy(q.x * q.y);
		f32 qyz(q.y * q.z);
		f32 qwx(q.w * q.x);
		f32 qwy(q.w * q.y);
		f32 qwz(q.w * q.z);

		mat3 r(1.0);
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

	mat4 mat4_cast(quat const &q) {
		return mat4(mat3_cast(q));
	}

	std::string to_string(const quat& q) {
		std::ostringstream ss;
		ss << "quat(" << q.w << ", " << q.x << ", " << q.y << ", " << q.z << ")";
		return ss.str();
	}

	f32* value_ptr(quat &q) {
		return &q.w;
	}

	const f32* value_ptr(const quat &q) {
		return &q.w;
	}

	quat make_quat(const f32* ptr) {
		return quat{ ptr[0], ptr[1], ptr[2], ptr[3] };
	}

	/// 32-bit Float Trig ///

	f32 radians(f32 degrees) {
		return degrees * static_cast<f32>(0.01745329251994329576923690768489);
	}

	f32 degrees(f32 radians) {
		return radians * static_cast<f32>(57.295779513082320876798154814105);
	}

	f32 sin(f32 num) {
		return std::sin(num);
	}

	f32 cos(f32 num) {
		return std::cos(num);
	}

	f32 tan(f32 num) {
		return std::tan(num);
	}

	f32 asin(f32 num) {
		return std::asin(num);
	}

	f32 acos(f32 num) {
		return std::acos(num);
	}

	f32 atan(f32 num) {
		return std::atan(num);
	}

	f32 atan2(f32 x, f32 y) {
		return std::atan2(x, y);
	}

	f32 sinh(f32 num) {
		return std::sinh(num);
	}

	f32 cosh(f32 num) {
		return std::cosh(num);
	}

	f32 tanh(f32 num) {
		return std::tanh(num);
	}

	f32 asinh(f32 num) {
		return std::asinh(num);
	}

	f32 acosh(f32 num) {
		return std::acosh(num);
	}

	f32 atanh(f32 num) {
		return std::atanh(num);
	}

	/// 32-bit Float Utilities ///

	f32 ceil(f32 num) {
		return std::ceil(num);
	}

	f32 floor(f32 num) {
		return std::floor(num);
	}

	f32 round(f32 num) {
		return std::round(num);
	}

	f32 exp(f32 num) {
		return std::exp(num);
	}

	f32 pow(f32 x, f32 y) {
		return std::pow(x, y);
	}

	f32 abs(f32 num) {
		return std::abs(num);
	}

	f32 log(f32 num) {
		return std::log(num);
	}

	f32 log10(f32 num) {
		return std::log10(num);
	}

	f32 sqrt(f32 num) {
		return std::sqrt(num);
	}

	f32 min(f32 a, f32 b) {
		return std::min(a, b);
	}

	f32 max(f32 a, f32 b) {
		return std::max(a, b);
	}

	f32 clamp(f32 val, f32 low, f32 high) {
		return std::clamp(val, low, high);
	}

	void swap(f32 &a, f32 &b) {
		std::swap(a, b);
	}

	mat4 perspective(f32 fovy, f32 aspect, f32 zNear, f32 zFar) {
		assert(math::abs(aspect) > std::numeric_limits<f32>::epsilon() &&
			   "aspect must be non-zero");
		f32 const tanHalfFovy = tan(fovy / static_cast<f32>(2));
		mat4 result(static_cast<f32>(0));
		result[0][0] = static_cast<f32>(1) / (aspect * tanHalfFovy);
		result[1][1] = static_cast<f32>(1) / (tanHalfFovy);
		result[2][2] = - (zFar + zNear) / (zFar - zNear);
		result[2][3] = - static_cast<f32>(1);
		result[3][2] = - (static_cast<f32>(2) * zFar * zNear) / (zFar - zNear);
		return result;
	}

	mat4 ortho(f32 left, f32 right, f32 bottom, f32 top, f32 zNear, f32 zFar) {
		mat4 result(1);
		result[0][0] = static_cast<f32>(2) / (right - left);
		result[1][1] = static_cast<f32>(2) / (top - bottom);
		result[2][2] = - static_cast<f32>(2) / (zFar - zNear);
		result[3][0] = - (right + left) / (right - left);
		result[3][1] = - (top + bottom) / (top - bottom);
		result[3][2] = - (zFar + zNear) / (zFar - zNear);
		return result;
	}

	mat4 look_at(const vec3& eye, const vec3& center, const vec3& up) {
		vec3 const f(normalize(center - eye));
		vec3 const s(normalize(cross(f, up)));
		vec3 const u(cross(s, f));

		mat4 result(1);
		result[0][0] = s.x;
		result[1][0] = s.y;
		result[2][0] = s.z;
		result[0][1] = u.x;
		result[1][1] = u.y;
		result[2][1] = u.z;
		result[0][2] =-f.x;
		result[1][2] =-f.y;
		result[2][2] =-f.z;
		result[3][0] =-dot(s, eye);
		result[3][1] =-dot(u, eye);
		result[3][2] = dot(f, eye);
		return result;
	}

	namespace decomposedetail {

		f32 epsilon() {
			return std::numeric_limits<f32>::epsilon();
		}

		bool epsilonEqual(const float &x,
						  const float &y,
						  const float &epsilon) {
			return abs(x - y) < epsilon;
		}

		bool epsilonNotEqual(const float &x, const float &y, const float &epsilon) {
			return abs(x - y) >= epsilon;
		}

		vec3 combine(vec3 const &a,
					 vec3 const &b,
					 f32 ascl, f32 bscl) {
			return (a * ascl) + (b * bscl);
		}

		vec3 scale(vec3 const &v, f32 desiredLength) {
			return v * desiredLength / length(v);
		}
	}

	bool decompose(const mat4& modelMatrix,
				   vec3& scale,
				   quat& orientation,
				   vec3& translation,
				   vec3& skew,
				   vec4& perspective) {

		mat4 localMatrix(modelMatrix);

		// normalize the matrix.
		if(decomposedetail::epsilonEqual(localMatrix[3][3],
										 static_cast<f32>(0),
										 decomposedetail::epsilon()))
			return false;

		for(std::size_t i = 0; i < 4; ++i)
			for(std::size_t j = 0; j < 4; ++j)
				localMatrix[i][j] /= localMatrix[3][3];

		// perspectiveMatrix is used to solve for perspective, but it also provides
		// an easy way to test for singularity of the upper 3x3 component.
		mat4 perspectiveMatrix(localMatrix);

		for(std::size_t i = 0; i < 3; i++)
			perspectiveMatrix[i][3] = static_cast<f32>(0);
		perspectiveMatrix[3][3] = static_cast<f32>(1);

		// TODO: Fixme!
		if(decomposedetail::epsilonEqual(determinant(perspectiveMatrix),
										 static_cast<f32>(0),
										 decomposedetail::epsilon()))
			return false;

		// first, isolate perspective.  this is the messiest.
		if(decomposedetail::epsilonNotEqual(localMatrix[0][3],
											static_cast<f32>(0),
											decomposedetail::epsilon()) ||
		   decomposedetail::epsilonNotEqual(localMatrix[1][3],
											static_cast<f32>(0),
											decomposedetail::epsilon()) ||
		   decomposedetail::epsilonNotEqual(localMatrix[2][3],
											static_cast<f32>(0),
											decomposedetail::epsilon())) {
			// rightHandSide is the right hand side of the equation.
			vec4 rightHandSide;
			rightHandSide[0] = localMatrix[0][3];
			rightHandSide[1] = localMatrix[1][3];
			rightHandSide[2] = localMatrix[2][3];
			rightHandSide[3] = localMatrix[3][3];

			// solve the equation by inverting perspectiveMatrix and multiplying
			// rightHandSide by the inverse.  (this is the easiest way, not
			// necessarily the best.)
			mat4 inversePerspectiveMatrix = inverse(perspectiveMatrix);//   inverse(PerspectiveMatrix, inversePerspectiveMatrix);
			mat4 transposedInversePerspectiveMatrix = transpose(inversePerspectiveMatrix);//   transposeMatrix4(inversePerspectiveMatrix, transposedInversePerspectiveMatrix);

			perspective = transposedInversePerspectiveMatrix * rightHandSide;
			//  v4MulPointByMatrix(rightHandSide, transposedInversePerspectiveMatrix, perspectivePoint);

			// clear the perspective partition
			localMatrix[0][3] = localMatrix[1][3] = localMatrix[2][3] = static_cast<f32>(0);
			localMatrix[3][3] = static_cast<f32>(1);
		}
		else {
		// no perspective.
			perspective = vec4(0, 0, 0, 1);
		}

		// next take care of translation (easy).
		translation = vec3(localMatrix[3]);
		localMatrix[3] = vec4(0, 0, 0, localMatrix[3].w);

		vec3 row[3], pdum3;

		// now get scale and shear.
		for(std::size_t i = 0; i < 3; ++i)
			for(std::size_t j = 0; j < 3; ++j)
				row[i][j] = localMatrix[i][j];

		// compute X scale factor and normalize first row.
		scale.x = length(row[0]);// v3Length(Row[0]);

		row[0] = decomposedetail::scale(row[0], static_cast<f32>(1));

		// compute XY shear factor and make 2nd row orthogonal to 1st.
		skew.z = dot(row[0], row[1]);
		row[1] = decomposedetail::combine(row[1], row[0], static_cast<f32>(1), -skew.z);

		// now, compute Y scale and normalize 2nd row.
		scale.y = length(row[1]);
		row[1] = decomposedetail::scale(row[1], static_cast<f32>(1));
		skew.z /= scale.y;

		// compute XZ and YZ shears, orthogonalize 3rd row.
		skew.y = dot(row[0], row[2]);
		row[2] = decomposedetail::combine(row[2], row[0], static_cast<f32>(1), -skew.y);
		skew.x = dot(row[1], row[2]);
		row[2] = decomposedetail::combine(row[2], row[1], static_cast<f32>(1), -skew.x);

		// next, get Z scale and normalize 3rd row.
		scale.z = length(row[2]);
		row[2] = decomposedetail::scale(row[2], static_cast<f32>(1));
		skew.y /= scale.z;
		skew.x /= scale.z;

		// at this point, the matrix (in rows[]) is orthonormal.
		// check for a coordinate system flip.  If the determinant
		// is -1, then negate the matrix and the scaling factors.
		pdum3 = cross(row[1], row[2]); // v3Cross(row[1], row[2], Pdum3);
		if(dot(row[0], pdum3) < 0) {
			for(std::size_t i = 0; i < 3; i++) {
				scale[i] *= static_cast<f32>(-1);
				row[i] *= static_cast<f32>(-1);
			}
		}

		// now, get the rotations out, as described in the gem.

		// FIXME - add the ability to return either quaternions (which are
		// easier to recompose with) or Euler angles (rx, ry, rz), which
		// are easier for authors to deal with. The latter will only be useful
		// when we fix https://bugs.webkit.org/show_bug.cgi?id=23799, so I
		// will leave the Euler angle code here for now.

		// ret.rotateY = asin(-Row[0][2]);
		// if (cos(ret.rotateY) != 0) {
		//     ret.rotateX = atan2(Row[1][2], Row[2][2]);
		//     ret.rotateZ = atan2(Row[0][1], Row[0][0]);
		// } else {
		//     ret.rotateX = atan2(-Row[2][0], Row[1][1]);
		//     ret.rotateZ = 0;
		// }

		int i, j, k = 0;
		f32 root, trace = row[0].x + row[1].y + row[2].z;
		if(trace > static_cast<f32>(0)) {
			root = sqrt(trace + static_cast<f32>(1.0));
			orientation.w = static_cast<f32>(0.5) * root;
			root = static_cast<f32>(0.5) / root;
			orientation.x = root * (row[1].z - row[2].y);
			orientation.y = root * (row[2].x - row[0].z);
			orientation.z = root * (row[0].y - row[1].x);
		}
		else {
			static int next[3] = {1, 2, 0};
			i = 0;
			if(row[1].y > row[0].x) i = 1;
			if(row[2].z > row[i][i]) i = 2;
			j = next[i];
			k = next[j];

			root = sqrt(row[i][i] - row[j][j] - row[k][k] + static_cast<f32>(1.0));

			orientation[i] = static_cast<f32>(0.5) * root;
			root = static_cast<f32>(0.5) / root;
			orientation[j] = root * (row[i][j] + row[j][i]);
			orientation[k] = root * (row[i][k] + row[k][i]);
			orientation.w = root * (row[j][k] - row[k][j]);
		}

		return true;
	}
}
