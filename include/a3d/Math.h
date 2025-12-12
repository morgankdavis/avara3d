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
#include <string>

namespace a3d::math {

	class f32vec2;
	class f32vec3;
	class f32vec4;

	class i32vec2;
	class i32vec3;
	class i32vec4;

	class u32vec2;
	class u32vec3;
	class u32vec4;

	class u8vec2;
	class u8vec3;
	class u8vec4;

	class f32mat2;
	class f32mat3;
	class f32mat4;

	class f32quat;

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

	struct f32vec2 {
		union {
			struct { f32 x, y; };
			struct { f32 s, t; };
		};
		f32vec2();
		f32vec2(f32 x_, f32 y_);
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

	struct i32vec2 {
		i32 x, y;
		i32vec2();
		i32vec2(i32 x_, i32 y_);
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
		explicit i32vec4(const i32vec2& v);
		explicit i32vec4(const i32vec3& v);
		i32& operator[](std::size_t i);
		const i32& operator[](std::size_t i) const;
	};

	struct u32vec2 {
		u32 x, y;
		u32vec2();
		u32vec2(u32 x_, u32 y_);
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
		explicit u32vec4(const u32vec2& v);
		explicit u32vec4(const u32vec3& v);
		u32& operator[](std::size_t i);
		const u32& operator[](std::size_t i) const;
	};

	struct u8vec2 {
		u8 x, y;
		u8vec2();
		u8vec2(u8 x_, u8 y_);
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
		explicit u8vec4(const u8vec2& v);
		explicit u8vec4(const u8vec3& v);
		u8& operator[](std::size_t i);
		const u8& operator[](std::size_t i) const;
	};

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

	struct f32quat {
		f32 w, x, y, z;
		f32quat();
		f32quat(f32 w_, f32 x_, f32 y_, f32 z_);
		explicit f32quat(f32 s);
		f32& operator[](std::size_t i);
		const f32& operator[](std::size_t i) const;
	};

	/// 32-bit Float Vector ///

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

	/// 32-bit Float Trig ///

	f32 		radians(f32 degrees);
	f32 		degrees(f32 radians);

	f32	 		sin(f32 num);
	f32 		cos(f32 num);
	f32 		tan(f32 num);
	f32 		asin(f32 num);
	f32 		acos(f32 num);
	f32 		atan(f32 num);
	f32 		atan2(f32 x, f32 y);
	f32 		sinh(f32 num);
	f32 		cosh(f32 num);
	f32 		tanh(f32 num);
	f32 		asinh(f32 num);
	f32 		acosh(f32 num);
	f32 		atanh(f32 num);

	/// 32-bit Float Common ///

	f32 		ceil(f32 num);
	f32 		floor(f32 num);
	f32 		round(f32 num);
	f32 		exp(f32 num);
	f32 		exp2(f32 num);
	f32 		pow(f32 x, f32 y);
	f32 		abs(f32 num);
	f32 		log(f32 num);
	f32 		log2(f32 num);
	f32 		log10(f32 num);
	f32 		sqrt(f32 num);
	f32 		min(f32 a, f32 b);
	f32 		max(f32 a, f32 b);
	f32			clamp(f32 val, f32 low, f32 high);
	void 		swap(f32& a, f32& b);



	mat4 		perspective(f32 fovy, f32 aspect, f32 zNear, f32 zFar); // rh
	mat4 		ortho(f32 left, f32 right, f32 bottom, f32 top, f32 zNear, f32 zFar); // rh
	mat4 		look_at(const vec3& eye, const vec3& center, const vec3& up); // rh

	bool 		decompose_trs(const mat4& m,
							  vec3& scale,
							  quat& rotation,
							  vec3& translation);

	// genTypeT mix(genTypeT x, genTypeT y, genTypeU a);
	// vec<L, T, Q> mix(vec<L, T, Q> const& x, vec<L, T, Q> const& y, vec<L, U, Q> const& a);
	// vec<L, T, Q> mix(vec<L, T, Q> const& x, vec<L, T, Q> const& y, U a);

	// EASING

	// RANDOM?

	/// Constants ///

	inline f32 e() 					{ return 2.7182818284590452354; }	// e
	inline f32 log2e() 				{ return 1.4426950408889634074; }	// log_2 e
	inline f32 log10e() 			{ return 0.43429448190325182765; }	// log_10 e
	inline f32 nl2() 				{ return 0.69314718055994530942; }	// log_e 2
	inline f32 nl10() 				{ return 2.30258509299404568402; }	// log_e 10
	inline f32 pi() 				{ return 3.14159265358979323846; }	// pi
	inline f32 pi_over_2()			{ return 1.57079632679489661923; }	// pi/2
	inline f32 pi_over_4() 			{ return 0.78539816339744830962; }	// pi/4
	inline f32 one_over_pi()		{ return 0.31830988618379067154; }	// 1/pi
	inline f32 two_over_pi()		{ return 0.63661977236758134308; }	// 2/pi
	inline f32 two_over_sqrt_pi()	{ return 1.12837916709551257390; }	// 2/sqrt(pi)
	inline f32 sqrt2()				{ return 1.41421356237309504880; }	// sqrt(2)
	inline f32 one_over_sqrt2()		{ return 0.70710678118654752440; }	// 1/sqrt(2)
}

#endif //AVARA3D_MATH_H
