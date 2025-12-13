//
//  Math.h
//  avara3d
//
//  Created by Morgan Davis on 12/7/25.
//  Copyright © 2025 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_MATH_H
#define AVARA3D_MATH_H

#include <cstdint>
#include <optional>
#include <random>
#include <string>

namespace a3d::math {

	struct f32vec2;
	struct f32vec3;
	struct f32vec4;
	struct i32vec2;
	struct i32vec3;
	struct i32vec4;
	struct u32vec2;
	struct u32vec3;
	struct u32vec4;
	struct u8vec2;
	struct u8vec3;
	struct u8vec4;
	struct f32mat2;
	struct f32mat3;
	struct f32mat4;
	struct f32quat;

	/// Types ///

	using f32 = float;
	using u8 = std::uint8_t;
	using i32 = std::int32_t;
	using u32 = std::uint32_t;

	using vec2 = f32vec2;
	using vec3 = f32vec3;
	using vec4 = f32vec4;

	using ivec2 = i32vec2;
	using ivec3 = i32vec3;
	using ivec4 = i32vec4;

	using uvec2 = u32vec2;
	using uvec3 = u32vec3;
	using uvec4 = u32vec4;

	using mat2 = f32mat2;
	using mat3 = f32mat3;
	using mat4 = f32mat4;

	using quat = f32quat;

	/// Constants ///

	const f32 F32_COMP_EPS = 1e-6f;

	/// 32-bit Float Vector ///

	struct f32vec2 {
		union {
			struct { f32 x, y; };
			struct { f32 s, t; };
		};
		f32vec2();
		f32vec2(f32 x_, f32 y_);
		explicit f32vec2(f32 n);
		explicit f32vec2(const f32vec3& v);
		explicit f32vec2(const f32vec4& v);
		f32& operator[](std::size_t i);
		const f32& operator[](std::size_t i) const;
	};

	struct f32vec3 {
		union {
			struct { f32 x, y, z; };
			struct { f32 r, g, b; };
			struct { f32 s, t, p; };
			struct { f32 pitch, yaw, roll; };
		};
		f32vec3();
		f32vec3(f32 x_, f32 y_, f32 z_);
		f32vec3(const f32vec2& v, f32 z_);
		explicit f32vec3(f32 n);
		explicit f32vec3(const f32vec2& v);
		explicit f32vec3(const f32vec4& v);
		f32& operator[](std::size_t i);
		const f32& operator[](std::size_t i) const;
	};

	struct f32vec4 {
		union {
			struct { f32 x, y, z, w; };
			struct { f32 r, g, b, a; };
		};
		f32vec4();
		f32vec4(f32 x_, f32 y_, f32 z_, f32 w_);
		f32vec4(const f32vec3& v, f32 w_);
		explicit f32vec4(const f32vec2& v);
		explicit f32vec4(const f32vec3& v);
		f32& operator[](std::size_t i);
		const f32& operator[](std::size_t i) const;
	};

	f32vec2 	operator-(const f32vec2& v);
	f32vec3 	operator-(const f32vec3& v);
	f32vec4 	operator-(const f32vec4& v);

	f32vec2 	operator+(const f32vec2& a, const f32vec2& b);
	f32vec3 	operator+(const f32vec3& a, const f32vec3& b);
	f32vec4 	operator+(const f32vec4& a, const f32vec4& b);

	f32vec2 	operator-(const f32vec2& a, const f32vec2& b);
	f32vec3 	operator-(const f32vec3& a, const f32vec3& b);
	f32vec4 	operator-(const f32vec4& a, const f32vec4& b);

	f32vec2 	operator*(const f32vec2& v, f32 s);
	f32vec3 	operator*(const f32vec3& v, f32 s);
	f32vec4 	operator*(const f32vec4& v, f32 s);
	f32vec2 	operator*(f32 s, const f32vec2& v);
	f32vec3 	operator*(f32 s, const f32vec3& v);
	f32vec4 	operator*(f32 s, const f32vec4& v);

	f32vec2 	operator/(const f32vec2& v, f32 s);
	f32vec3 	operator/(const f32vec3& v, f32 s);
	f32vec4 	operator/(const f32vec4& v, f32 s);

	f32vec2& 	operator+=(f32vec2& a, const f32vec2& b);
	f32vec3& 	operator+=(f32vec3& a, const f32vec3& b);
	f32vec4& 	operator+=(f32vec4& a, const f32vec4& b);

	f32vec2& 	operator-=(f32vec2& a, const f32vec2& b);
	f32vec3& 	operator-=(f32vec3& a, const f32vec3& b);
	f32vec4& 	operator-=(f32vec4& a, const f32vec4& b);

	f32vec2& 	operator*=(f32vec2& v, f32 s);
	f32vec3& 	operator*=(f32vec3& v, f32 s);
	f32vec4& 	operator*=(f32vec4& v, f32 s);

	f32vec2& 	operator/=(f32vec2& v, f32 s);
	f32vec3& 	operator/=(f32vec3& v, f32 s);
	f32vec4& 	operator/=(f32vec4& v, f32 s);

	bool 		operator==(const f32vec2& a, const f32vec2& b);
	bool 		operator==(const f32vec3& a, const f32vec3& b);
	bool 		operator==(const f32vec4& a, const f32vec4& b);

	bool 		operator!=(const f32vec2& a, const f32vec2& b);
	bool		operator!=(const f32vec3& a, const f32vec3& b);
	bool 		operator!=(const f32vec4& a, const f32vec4& b);

	f32 		min(const f32vec2& v);
	f32 		min(const f32vec3& v);
	f32 		min(const f32vec4& v);

	f32 		max(const f32vec2& v);
	f32 		max(const f32vec3& v);
	f32 		max(const f32vec4& v);

	f32 		dot(const f32vec2& a, const f32vec2& b);
	f32 		dot(const f32vec3& a, const f32vec3& b);
	f32 		dot(const f32vec4& a, const f32vec4& b);

	f32 		length(const f32vec2& v);
	f32 		length(const f32vec3& v);
	f32 		length(const f32vec4& v);

	f32vec2		normalize(const f32vec2& v);
	f32vec3 	normalize(const f32vec3& v);
	f32vec4	 	normalize(const f32vec4& v);

	f32vec3 	cross(const f32vec3& a, const f32vec3& b);

	std::string to_string(const f32vec2& v);
	std::string to_string(const f32vec3& v);
	std::string to_string(const f32vec4& v);

	f32* 		value_ptr(f32vec2& v);
	const f32* 	value_ptr(const f32vec2& v);
	f32* 		value_ptr(f32vec3& v);
	const f32* 	value_ptr(const f32vec3& v);
	f32* 		value_ptr(f32vec4& v);
	const f32* 	value_ptr(const f32vec4& v);

	f32vec2 	make_vec2(const f32* ptr);
	f32vec3 	make_vec3(const f32* ptr);
	f32vec4 	make_vec4(const f32* ptr);

	/// Signed 32-bit Integer Vector ///

	struct i32vec2 {
		i32 x, y;
		i32vec2();
		i32vec2(i32 x_, i32 y_);
		explicit i32vec2(i32 n);
		explicit i32vec2(const i32vec3& v);
		explicit i32vec2(const i32vec4& v);
		i32& operator[](std::size_t i);
		const i32& operator[](std::size_t i) const;
	};

	struct i32vec3 {
		i32 x, y, z;
		i32vec3();
		i32vec3(i32 x_, i32 y_, i32 z_);
		i32vec3(const i32vec2& v, i32 z_);
		explicit i32vec3(i32 n);
		explicit i32vec3(const i32vec2& v);
		explicit i32vec3(const i32vec4& v);
		i32& operator[](std::size_t i);
		const i32& operator[](std::size_t i) const;
	};

	struct i32vec4 {
		i32 x, y, z, w;
		i32vec4();
		i32vec4(i32 x_, i32 y_, i32 z_, i32 w_);
		i32vec4(const i32vec3& v, i32 w_);
		explicit i32vec4(i32 n);
		explicit i32vec4(const i32vec2& v);
		explicit i32vec4(const i32vec3& v);
		i32& operator[](std::size_t i);
		const i32& operator[](std::size_t i) const;
	};

	i32vec2 	operator-(const i32vec2& v);
	i32vec3 	operator-(const i32vec3& v);
	i32vec4 	operator-(const i32vec4& v);

	i32vec2 	operator+(const i32vec2& a, const i32vec2& b);
	i32vec3 	operator+(const i32vec3& a, const i32vec3& b);
	i32vec4 	operator+(const i32vec4& a, const i32vec4& b);

	i32vec2 	operator-(const i32vec2& a, const i32vec2& b);
	i32vec3	 	operator-(const i32vec3& a, const i32vec3& b);
	i32vec4 	operator-(const i32vec4& a, const i32vec4& b);

	i32vec2 	operator*(const i32vec2& v, i32 s);
	i32vec3 	operator*(const i32vec3& v, i32 s);
	i32vec4 	operator*(const i32vec4& v, i32 s);
	i32vec2 	operator*(i32 s, const i32vec2& v);
	i32vec3 	operator*(i32 s, const i32vec3& v);
	i32vec4 	operator*(i32 s, const i32vec4& v);

	i32vec2 	operator/(const i32vec2& v, i32 s);
	i32vec3		operator/(const i32vec3& v, i32 s);
	i32vec4 	operator/(const i32vec4& v, i32 s);

	i32vec2& 	operator+=(i32vec2& a, const i32vec2& b);
	i32vec3& 	operator+=(i32vec3& a, const i32vec3& b);
	i32vec4& 	operator+=(i32vec4& a, const i32vec4& b);

	i32vec2& 	operator-=(i32vec2& a, const i32vec2& b);
	i32vec3& 	operator-=(i32vec3& a, const i32vec3& b);
	i32vec4& 	operator-=(i32vec4& a, const i32vec4& b);

	i32vec2& 	operator*=(i32vec2& v, i32 s);
	i32vec3& 	operator*=(i32vec3& v, i32 s);
	i32vec4& 	operator*=(i32vec4& v, i32 s);

	i32vec2& 	operator/=(i32vec2& v, i32 s);
	i32vec3& 	operator/=(i32vec3& v, i32 s);
	i32vec4& 	operator/=(i32vec4& v, i32 s);

	bool 		operator==(const i32vec2& a, const i32vec2& b);
	bool 		operator==(const i32vec3& a, const i32vec3& b);
	bool 		operator==(const i32vec4& a, const i32vec4& b);

	bool 		operator!=(const i32vec2& a, const i32vec2& b);
	bool 		operator!=(const i32vec3& a, const i32vec3& b);
	bool 		operator!=(const i32vec4& a, const i32vec4& b);

	f32 		min(const i32vec2& v);
	f32 		min(const i32vec3& v);
	f32 		min(const i32vec4& v);

	f32			max(const i32vec2& v);
	f32 		max(const i32vec3& v);
	f32 		max(const i32vec4& v);

	i32 		dot(const i32vec2& a, const i32vec2& b);
	i32 		dot(const i32vec3& a, const i32vec3& b);
	i32 		dot(const i32vec4& a, const i32vec4& b);

	std::string to_string(const i32vec2& v);
	std::string to_string(const i32vec3& v);
	std::string to_string(const i32vec4& v);

	i32* 		value_ptr(i32vec2& v);
	const i32* 	value_ptr(const i32vec2& v);
	i32* 		value_ptr(i32vec3& v);
	const i32* 	value_ptr(const i32vec3& v);
	i32* 		value_ptr(i32vec4& v) ;
	const i32* 	value_ptr(const i32vec4& v);

	i32vec2 	make_vec2(const i32* ptr);
	i32vec3 	make_vec3(const i32* ptr);
	i32vec4 	make_vec4(const i32* ptr);

	/// Unsigned 32-bit Integer Vector ///

	struct u32vec2 {
		u32 x, y;
		u32vec2();
		u32vec2(u32 x_, u32 y_);
		explicit u32vec2(u32 n);
		explicit u32vec2(const u32vec3& v);
		explicit u32vec2(const u32vec4& v);
		u32& operator[](std::size_t i);
		const u32& operator[](std::size_t i) const;
	};

	struct u32vec3 {
		u32 x, y, z;
		u32vec3();
		u32vec3(u32 x_, u32 y_, u32 z_);
		u32vec3(const u32vec2& c, u32 z_);
		explicit u32vec3(u32 n);
		explicit u32vec3(const u32vec2& v);
		explicit u32vec3(const u32vec4& v);
		u32& operator[](std::size_t i);
		const u32& operator[](std::size_t i) const;
	};

	struct u32vec4 {
		u32 x, y, z, w;
		u32vec4();
		u32vec4(u32 x_, u32 y_, u32 z_, u32 w_);
		u32vec4(const u32vec3& v, u32 w_);
		explicit u32vec4(u32 n);
		explicit u32vec4(const u32vec2& v);
		explicit u32vec4(const u32vec3& v);
		u32& operator[](std::size_t i);
		const u32& operator[](std::size_t i) const;
	};

	u32vec2 	operator+(const u32vec2& a, const u32vec2& b);
	u32vec3 	operator+(const u32vec3& a, const u32vec3& b);
	u32vec4 	operator+(const u32vec4& a, const u32vec4& b);

	u32vec2 	operator-(const u32vec2& a, const u32vec2& b);
	u32vec3 	operator-(const u32vec3& a, const u32vec3& b);
	u32vec4 	operator-(const u32vec4& a, const u32vec4& b);

	u32vec2 	operator*(const u32vec2& v, u32 s);
	u32vec3 	operator*(const u32vec3& v, u32 s);
	u32vec4 	operator*(const u32vec4& v, u32 s);
	u32vec2 	operator*(u32 s, const u32vec2& v);
	u32vec3 	operator*(u32 s, const u32vec3& v);
	u32vec4 	operator*(u32 s, const u32vec4& v);

	u32vec2 	operator/(const u32vec2& v, u32 s);
	u32vec3 	operator/(const u32vec3& v, u32 s);
	u32vec4 	operator/(const u32vec4& v, u32 s);

	u32vec2& 	operator+=(u32vec2& a, const u32vec2& b);
	u32vec3& 	operator+=(u32vec3& a, const u32vec3& b);
	u32vec4& 	operator+=(u32vec4& a, const u32vec4& b);

	u32vec2& 	operator-=(u32vec2& a, const u32vec2& b);
	u32vec3& 	operator-=(u32vec3& a, const u32vec3& b);
	u32vec4& 	operator-=(u32vec4& a, const u32vec4& b);

	u32vec2& 	operator*=(u32vec2& v, u32 s);
	u32vec3& 	operator*=(u32vec3& v, u32 s);
	u32vec4& 	operator*=(u32vec4& v, u32 s);

	u32vec2& 	operator/=(u32vec2& v, u32 s);
	u32vec3& 	operator/=(u32vec3& v, u32 s);
	u32vec4& 	operator/=(u32vec4& v, u32 s);

	bool 		operator==(const u32vec2& a, const u32vec2& b);
	bool 		operator==(const u32vec3& a, const u32vec3& b);
	bool 		operator==(const u32vec4& a, const u32vec4& b);

	bool 		operator!=(const u32vec2& a, const u32vec2& b);
	bool 		operator!=(const u32vec3& a, const u32vec3& b);
	bool 		operator!=(const u32vec4& a, const u32vec4& b);

	f32 		min(const u32vec2& v);
	f32 		min(const u32vec3& v);
	f32 		min(const u32vec4& v);

	f32 		max(const u32vec2& v);
	f32 		max(const u32vec3& v);
	f32 		max(const u32vec4& v);

	u32 		dot(const u32vec2& a, const u32vec2& b);
	u32 		dot(const u32vec3& a, const u32vec3& b);
	u32 		dot(const u32vec4& a, const u32vec4& b);

	std::string to_string(const u32vec2& v);
	std::string to_string(const u32vec3& v);
	std::string to_string(const u32vec4& v);

	u32* 		value_ptr(u32vec2& v);
	const u32* 	value_ptr(const u32vec2& v);
	u32* 		value_ptr(u32vec3& v);
	const u32* 	value_ptr(const u32vec3& v);
	u32* 		value_ptr(u32vec4& v) ;
	const u32* 	value_ptr(const u32vec4& v);

	u32vec2 	make_vec2(const u32* ptr);
	u32vec3 	make_vec3(const u32* ptr);
	u32vec4 	make_vec4(const u32* ptr);

	/// Unsigned 8-bit Integer Vector ///

	struct u8vec2 {
		u8 x, y;
		u8vec2();
		u8vec2(u8 x_, u8 y_);
		explicit u8vec2(u8 n);
		explicit u8vec2(const u8vec3& v);
		explicit u8vec2(const u8vec4& v);
		u8& operator[](std::size_t i);
		const u8& operator[](std::size_t i) const;
	};

	struct u8vec3 {
		union {
			struct { u8 x, y, z; };
			struct { u8 r, g, b; };
		};
		u8vec3();
		u8vec3(u8 x_, u8 y_, u8 z_);
		u8vec3(const u8vec2& v, u8 z_);
		explicit u8vec3(u8 n);
		explicit u8vec3(const u8vec2& v);
		explicit u8vec3(const u8vec4& v);
		u8& operator[](std::size_t i);
		const u8& operator[](std::size_t i) const;
	};

	struct u8vec4 {
		union {
			struct { u8 x, y, z, w; };
			struct { u8 r, g, b, a; };
		};
		u8vec4();
		u8vec4(u8 x_, u8 y_, u8 z_, u8 w_);
		u8vec4(const u8vec3& z, u8 w_);
		explicit u8vec4(u8 n);
		explicit u8vec4(const u8vec2& v);
		explicit u8vec4(const u8vec3& v);
		u8& operator[](std::size_t i);
		const u8& operator[](std::size_t i) const;
	};

	u8vec2     	operator+(const u8vec2& a, const u8vec2& b);
	u8vec3     	operator+(const u8vec3& a, const u8vec3& b);
	u8vec4     	operator+(const u8vec4& a, const u8vec4& b);

	u8vec2     	operator-(const u8vec2& a, const u8vec2& b);
	u8vec3     	operator-(const u8vec3& a, const u8vec3& b);
	u8vec4     	operator-(const u8vec4& a, const u8vec4& b);

	u8vec2     	operator*(const u8vec2& v, f32 s);
	u8vec3     	operator*(const u8vec3& v, f32 s);
	u8vec4     	operator*(const u8vec4& v, f32 s);

	u8vec2     	operator*(f32 s, const u8vec2& v);
	u8vec3     	operator*(f32 s, const u8vec3& v);
	u8vec4     	operator*(f32 s, const u8vec4& v);

	u8vec2     	operator/(const u8vec2& v, f32 s);
	u8vec3     	operator/(const u8vec3& v, f32 s);
	u8vec4     	operator/(const u8vec4& v, f32 s);

	u8vec2&    	operator+=(u8vec2& a, const u8vec2& b);
	u8vec3&    	operator+=(u8vec3& a, const u8vec3& b);
	u8vec4&    	operator+=(u8vec4& a, const u8vec4& b);

	u8vec2&    	operator-=(u8vec2& a, const u8vec2& b);
	u8vec3&    	operator-=(u8vec3& a, const u8vec3& b);
	u8vec4&    	operator-=(u8vec4& a, const u8vec4& b);

	u8vec2&    	operator*=(u8vec2& v, f32 s);
	u8vec3&    	operator*=(u8vec3& v, f32 s);
	u8vec4&    	operator*=(u8vec4& v, f32 s);

	u8vec2&    	operator/=(u8vec2& v, f32 s);
	u8vec3&    	operator/=(u8vec3& v, f32 s);
	u8vec4&    	operator/=(u8vec4& v, f32 s);

	bool 		operator==(const u8vec2& a, const u8vec2& b);
	bool 		operator==(const u8vec3& a, const u8vec3& b);
	bool 		operator==(const u8vec4& a, const u8vec4& b);

	bool 		operator!=(const u8vec2& a, const u8vec2& b);
	bool 		operator!=(const u8vec3& a, const u8vec3& b);
	bool 		operator!=(const u8vec4& a, const u8vec4& b);

	f32			min(const u8vec2& v);
	f32 		min(const u8vec3& v);
	f32 		min(const u8vec4& v);

	f32 		max(const u8vec2& v);
	f32 		max(const u8vec3& v);
	f32 		max(const u8vec4& v);

	u8*       	value_ptr(u8vec2& v);
	const u8*  	value_ptr(const u8vec2& v);

	u8*        	value_ptr(u8vec3& v);
	const u8*  	value_ptr(const u8vec3& v);

	u8*        	value_ptr(u8vec4& v);
	const u8*  	value_ptr(const u8vec4& v);

	u8vec2     	make_vec2(const u8* ptr);
	u8vec3     	make_vec3(const u8* ptr);
	u8vec4     	make_vec4(const u8* ptr);

	std::string to_string(const u8vec2& v);
	std::string to_string(const u8vec3& v);
	std::string to_string(const u8vec4& v);

	/// 32-bit Float Matrix ///

	struct f32mat2 {
		vec2 c0, c1;
		f32mat2();
		explicit f32mat2(f32 diag);
		vec2& operator[](std::size_t i);
		const vec2& operator[](std::size_t i) const;
	};

	struct f32mat3 {
		vec3 c0, c1, c2;
		f32mat3();
		explicit f32mat3(f32 diag);
		vec3& operator[](std::size_t i);
		const vec3& operator[](std::size_t i) const;
	};

	struct f32mat4 {
		vec4 c0, c1, c2, c3;
		f32mat4();
		explicit f32mat4(f32 diag);
		explicit f32mat4(const f32mat3& m);
		vec4& operator[](std::size_t i);
		const vec4& operator[](std::size_t i) const;
	};

	f32mat2 	identity2();
	f32mat3 	identity3();
	f32mat4 	identity4();

	f32mat2 	zero2();
	f32mat3 	zero3();
	f32mat4 	zero4();

	vec2 		operator*(const f32mat2& m, const vec2& v);
	vec3 		operator*(const f32mat3& m, const vec3& v);
	vec4 		operator*(const f32mat4& m, const vec4& v);

	f32mat2 	operator*(const f32mat2& a, const f32mat2& b);
	f32mat3 	operator*(const f32mat3& a, const f32mat3& b);
	f32mat4 	operator*(const f32mat4& a, const f32mat4& b);

	f32mat2& 	operator*=(f32mat2& a, const f32mat2& b);
	f32mat3& 	operator*=(f32mat3& a, const f32mat3& b);
	f32mat4& 	operator*=(f32mat4& a, const f32mat4& b);

	bool 		operator==(const f32mat2& a, const f32mat2& b);
	bool 		operator!=(const f32mat2& a, const f32mat2& b);

	bool 		operator==(const f32mat3& a, const f32mat3& b);
	bool		operator!=(const f32mat3& a, const f32mat3& b);

	bool 		operator==(const f32mat4& a, const f32mat4& b);
	bool 		operator!=(const f32mat4& a, const f32mat4& b);

	f32mat2 	transpose(const f32mat2& m);
	f32mat3 	transpose(const f32mat3& m);
	f32mat4 	transpose(const f32mat4& m);

	f32 		determinant(const f32mat2& m);
	f32			determinant(const f32mat3& m);
	f32 		determinant(const f32mat4& m);

	f32mat2 	inverse(const f32mat2& m);
	f32mat3	 	inverse(const f32mat3& m);
	f32mat4 	inverse(const f32mat4& m);

	f32mat4 	translate(const f32mat4& m, const vec3& v);
	f32mat4 	rotate(const f32mat4& m, f32 angle, const vec3& v);
	f32mat4 	scale(const f32mat4& m, const vec3& v);
	f32mat4 	scale(const f32mat4& m, f32 s);

	std::string to_string(const f32mat2& m, unsigned pad = 10);
	std::string to_string(const f32mat3& m, unsigned pad = 10);
	std::string to_string(const f32mat4& m, unsigned pad = 10);

	f32* 		value_ptr(f32mat2& m);
	const f32* 	value_ptr(const f32mat2& m);
	f32* 		value_ptr(f32mat3& m);
	const f32* 	value_ptr(const f32mat3& m);
	f32* 		value_ptr(f32mat4& m);
	const f32* 	value_ptr(const f32mat4& m);

	f32mat2 	make_mat2(const f32* ptr);
	f32mat3 	make_mat3(const f32* ptr);
	f32mat4 	make_mat4(const f32* ptr);

	/// 32-bit Float Quaternion ///

	struct f32quat {
		f32 w, x, y, z;
		f32quat();
		f32quat(f32 w_, f32 x_, f32 y_, f32 z_);
		explicit f32quat(f32 s);
		f32& operator[](std::size_t i);
		const f32& operator[](std::size_t i) const;
	};

	f32quat 	identity_quat();

	f32quat 	operator*(const f32quat& a, const f32quat& b);

	f32quat 	operator+(const f32quat& a, const f32quat& b);
	f32quat 	operator*(const f32quat& q, f32 s);
	f32quat 	operator*(f32 s, const f32quat& q);

	f32quat& 	operator+=(f32quat& a, const f32quat& b);
	f32quat& 	operator*=(f32quat& q, f32 s);
	f32quat& 	operator*=(f32quat& a, const f32quat& b);

	bool 		operator==(const f32quat& a, const f32quat& b);
	bool 		operator!=(const f32quat& a, const f32quat& b);

	f32  		dot(const f32quat& a, const f32quat& b);
	f32  		length(const f32quat& q);
	f32quat 	normalize(const f32quat& q);
	f32quat 	conjugate(const f32quat& q);
	f32quat		inverse(const f32quat& q);

	vec3 		rotate(const f32quat& q, const vec3& v);
	vec3 		operator*(const f32quat& q, const vec3& v);

	f32quat 	quaternion(const vec3& axis, f32 angle);
	vec4 		axis_angle(const f32quat& q);

	f32quat 	quaternion(const vec3& eulerAngles); // pitch/yaw/roll to f32quaternion, y–x–z order
	vec3 		euler_angles(const f32quat& q); // pitch/yaw/roll to f32quaternion, y–x–z order

	f32quat 	slerp(const f32quat& a, const f32quat& b, f32 t);

	mat3 		mat3_cast(f32quat const& q);
	mat4 		mat4_cast(f32quat const& q);

	std::string to_string(const f32quat& q);

	f32* 		value_ptr(f32quat& q) ;
	const f32* 	value_ptr(const f32quat& q);

	f32quat 	make_quat(const f32* ptr);

	/// Projection & Camera ///

	mat4 		perspective(f32 fovy, f32 aspect, f32 zNear, f32 zFar); // rh
	mat4 		ortho(f32 left, f32 right, f32 bottom, f32 top, f32 zNear, f32 zFar); // rh
	mat4 		look_at(const vec3& eye, const vec3& center, const vec3& up); // rh

	/// Matrix Decomposition ///

	bool 		decompose(const mat4& m,
						  vec3& scale,
						  quat& rotation,
						  vec3& translation);

	/// Scalar Angles ///

	f32 		radians(f32 degrees);
	f32 		degrees(f32 radians);

	/// Scalar Trig ///

	f32	 		sin(f32 n);
	f32 		cos(f32 n);
	f32 		tan(f32 n);
	f32 		asin(f32 n);
	f32 		acos(f32 n);
	f32 		atan(f32 n);
	f32 		atan2(f32 x, f32 y);
	f32 		sinh(f32 n);
	f32 		cosh(f32 n);
	f32 		tanh(f32 n);
	f32 		asinh(f32 n);
	f32 		acosh(f32 n);
	f32 		atanh(f32 n);

	/// Scalar Comparison ///

	bool		equal(f32 a, f32 b, f32 eps = F32_COMP_EPS);

	/// Scalar Rounding ///

	f32 		ceil(f32 n);
	f32 		floor(f32 n);
	f32 		round(f32 n);

	/// Scalar Exponentials & Logarithms ///

	f32 		exp(f32 n);
	f32 		exp2(f32 n);
	f32 		pow(f32 x, f32 y);
	f32 		log(f32 n);
	f32 		log2(f32 n);
	f32 		log10(f32 n);

	/// Scalar Magnitude ///

	f32 		abs(f32 n);
	f32 		sqrt(f32 n);
	f32 		cbrt(f32 n);

	/// Scalar Range / Ordering ///

	f32 		min(f32 a, f32 b);
	f32 		max(f32 a, f32 b);
	f32			clamp(f32 val, f32 low, f32 high);

	/// Bitwise / Classification ///

	bool 		is_nan(f32 n);
	bool 		is_inf(f32 n);
	bool 		sign_bit(f32 n);

	/// Swap Utilities ///

	void 		swap(f32& a, f32& b);
	template <class T2, std::size_t N>
	void 		swap(T2 (&a)[N], T2 (&b)[N]) {
		for (std::size_t i = 0; i < N; ++i) std::swap(a[i], b[i]); }

	/// Random ///

	f32 		uniform_01(); // [0, 1)
	f32 		uniform_01(std::mt19937* gen); // [0, 1)

	f32 		uniform_n11(); // [-1, 1)
	f32 		uniform_n11(std::mt19937* gen); // [-1, 1)

	u8 			uniform_linear(u8 min, u8 max); // [min, max]
	u8 			uniform_linear(std::mt19937* gen, u8 min, u8 max); // [min, max]

	u32 		uniform_linear(u32 min, u32 max); // [min, max]
	u32 		uniform_linear(std::mt19937* gen, u32 min, u32 max); // [min, max]

	i32 		uniform_linear(i32 min, i32 max); // [min, max]
	i32 		uniform_linear(std::mt19937* gen, i32 min, i32 max); // [min, max]

	f32 		uniform_linear(f32 min, f32 max); // [min, max)
	f32 		uniform_linear(std::mt19937* gen, f32 min, f32 max); // [min, max)

	f32vec2 	uniform_circular(f32 radius);
	f32vec2 	uniform_circular(std::mt19937* gen, f32 radius);

	f32vec3		uniform_spherical(f32 radius);
	f32vec3		uniform_spherical(std::mt19937* gen, f32 radius);

	f32vec2		uniform_disk(f32 radius);
	f32vec2		uniform_disk(std::mt19937* gen, f32 radius);

	f32vec3		uniform_ball(f32 radius);
	f32vec3		uniform_ball(std::mt19937* gen, f32 radius);

	f32			gaussian(f32 mean, f32 deviation, f32 min, f32 max);
	f32			gaussian(std::mt19937* gen, f32 mean, f32 deviation, f32 min, f32 max);

	bool 		bernoulli(f32 p);
	bool 		bernoulli(std::mt19937* gen, f32 p);

	// TODO: easing

	/// Constants ///

	inline constexpr f32 e() 				{ return f32(2.7182818284590452354); }	// e
	inline constexpr f32 log2e() 			{ return f32(1.4426950408889634074); }	// log_2 e
	inline constexpr f32 log10e() 			{ return f32(0.43429448190325182765); }	// log_10 e
	inline constexpr f32 nl2() 				{ return f32(0.69314718055994530942); }	// log_e 2
	inline constexpr f32 nl10() 			{ return f32(2.30258509299404568402); }	// log_e 10
	inline constexpr f32 pi() 				{ return f32(3.14159265358979323846); }	// pi
	inline constexpr f32 two_pi() 			{ return f32(6.2831853071795864769); }	// pi*2
	inline constexpr f32 pi_over_2()		{ return f32(1.57079632679489661923); }	// pi/2
	inline constexpr f32 pi_over_4() 		{ return f32(0.78539816339744830962); }	// pi/4
	inline constexpr f32 one_over_pi()		{ return f32(0.31830988618379067154); }	// 1/pi
	inline constexpr f32 two_over_pi()		{ return f32(0.63661977236758134308); }	// 2/pi
	inline constexpr f32 two_over_sqrt_pi()	{ return f32(1.12837916709551257390); }	// 2/sqrt(pi)
	inline constexpr f32 sqrt2()			{ return f32(1.41421356237309504880); }	// sqrt(2)
	inline constexpr f32 one_over_sqrt2()	{ return f32(0.70710678118654752440); }	// 1/sqrt(2)
}

#endif //AVARA3D_MATH_H
