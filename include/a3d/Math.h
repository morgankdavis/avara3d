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

	/// Types ///

	using f32 = float;
	using u8 = std::uint8_t;
	using i32 = std::int32_t;
	using u32 = std::uint32_t;

	class vec2;
	class vec3;
	class vec4;
	class ivec2;
	class ivec3;
	class ivec4;
	class uvec2;
	class uvec3;
	class uvec4;
	class u8vec2;
	class u8vec3;
	class u8vec4;
	class mat2;
	class mat3;
	class mat4;

	// using vec2 = f32vec2;
	// using vec3 = f32vec3;
	// using vec4 = f32vec4;

	// using ivec2 = i32vec2
	// using ivec3 = i32vec3
	// using ivec4 = i32vec4

	// using uvec2 = u32vec2
	// using uvec3 = u32vec3
	// using uvec4 = u32vec4

	// using mat2 = f32mat2;
	// using mat3 = f32mat3;
	// using mat4 = f32mat4;
	// using quat = f32quat;

	struct vec2 {
		union {
			struct { f32 x, y; };
			struct { f32 s, t; };
		};
		vec2();
		vec2(f32 x_, f32 y_);
		explicit vec2(const vec3& v);
		explicit vec2(const vec4& v);
		f32& operator[](std::size_t i);
		const f32& operator[](std::size_t i) const;
	};

	struct vec3 {
		union {
			struct { f32 x, y, z; };
			struct { f32 r, g, b; };
			struct { f32 s, t, p; };
			struct { f32 pitch, yaw, roll; };
		};
		vec3();
		vec3(f32 x_, f32 y_, f32 z_);
		vec3(const vec2& v, f32 z_);
		explicit vec3(const vec2& v);
		explicit vec3(const vec4& v);
		f32& operator[](std::size_t i);
		const f32& operator[](std::size_t i) const;
	};

	struct vec4 {
		union {
			struct { f32 x, y, z, w; };
			struct { f32 r, g, b, a; };
		};
		vec4();
		vec4(f32 x_, f32 y_, f32 z_, f32 w_);
		vec4(const vec3& v, f32 w_);
		explicit vec4(const vec2& v);
		explicit vec4(const vec3& v);
		f32& operator[](std::size_t i);
		const f32& operator[](std::size_t i) const;
	};

	struct ivec2 {
		i32 x, y;
		ivec2();
		ivec2(i32 x_, i32 y_);
		explicit ivec2(const ivec3& v);
		explicit ivec2(const ivec4& v);
		i32& operator[](std::size_t i);
		const i32& operator[](std::size_t i) const;
	};

	struct ivec3 {
		i32 x, y, z;
		ivec3();
		ivec3(i32 x_, i32 y_, i32 z_);
		ivec3(const ivec2& v, i32 z_);
		explicit ivec3(const ivec2& v);
		explicit ivec3(const ivec4& v);
		i32& operator[](std::size_t i);
		const i32& operator[](std::size_t i) const;
	};

	struct ivec4 {
		i32 x, y, z, w;
		ivec4();
		ivec4(i32 x_, i32 y_, i32 z_, i32 w_);
		ivec4(const ivec3& v, i32 w_);
		explicit ivec4(const ivec2& v);
		explicit ivec4(const ivec3& v);
		i32& operator[](std::size_t i);
		const i32& operator[](std::size_t i) const;
	};

	struct uvec2 {
		u32 x, y;
		uvec2();
		uvec2(u32 x_, u32 y_);
		explicit uvec2(const uvec3& v);
		explicit uvec2(const uvec4& v);
		u32& operator[](std::size_t i);
		const u32& operator[](std::size_t i) const;
	};

	struct uvec3 {
		u32 x, y, z;
		uvec3();
		uvec3(u32 x_, u32 y_, u32 z_);
		uvec3(const uvec2& c, u32 z_);
		explicit uvec3(const uvec2& v);
		explicit uvec3(const uvec4& v);
		u32& operator[](std::size_t i);
		const u32& operator[](std::size_t i) const;
	};

	struct uvec4 {
		u32 x, y, z, w;
		uvec4();
		uvec4(u32 x_, u32 y_, u32 z_, u32 w_);
		uvec4(const uvec3& v, u32 w_);
		explicit uvec4(const uvec2& v);
		explicit uvec4(const uvec3& v);
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

	struct mat2 {
		vec2 c0, c1;
		mat2();
		explicit mat2(f32 diag);
		vec2& operator[](std::size_t i);
		const vec2& operator[](std::size_t i) const;
	};

	struct mat3 {
		vec3 c0, c1, c2;
		mat3();
		explicit mat3(f32 diag);
		vec3& operator[](std::size_t i);
		const vec3& operator[](std::size_t i) const;
	};

	struct mat4 {
		vec4 c0, c1, c2, c3;
		mat4();
		explicit mat4(f32 diag);
		explicit mat4(const mat3& m);
		vec4& operator[](std::size_t i);
		const vec4& operator[](std::size_t i) const;
	};

	struct quat {
		f32 w, x, y, z;
		quat();
		quat(f32 w_, f32 x_, f32 y_, f32 z_);
		explicit quat(f32 s);
		f32& operator[](std::size_t i);
		const f32& operator[](std::size_t i) const;
	};

	/// 32-bit Float Vector ///

	vec2 		operator-(const vec2& v);
	vec3 		operator-(const vec3& v);
	vec4 		operator-(const vec4& v);

	vec2 		operator+(const vec2& a, const vec2& b);
	vec3 		operator+(const vec3& a, const vec3& b);
	vec4 		operator+(const vec4& a, const vec4& b);

	vec2 		operator-(const vec2& a, const vec2& b);
	vec3 		operator-(const vec3& a, const vec3& b);
	vec4 		operator-(const vec4& a, const vec4& b);

	vec2 		operator*(const vec2& v, f32 s);
	vec3 		operator*(const vec3& v, f32 s);
	vec4 		operator*(const vec4& v, f32 s);
	vec2 		operator*(f32 s, const vec2& v);
	vec3 		operator*(f32 s, const vec3& v);
	vec4 		operator*(f32 s, const vec4& v);

	vec2 		operator/(const vec2& v, f32 s);
	vec3 		operator/(const vec3& v, f32 s);
	vec4 		operator/(const vec4& v, f32 s);

	vec2& 		operator+=(vec2& a, const vec2& b);
	vec3& 		operator+=(vec3& a, const vec3& b);
	vec4& 		operator+=(vec4& a, const vec4& b);

	vec2& 		operator-=(vec2& a, const vec2& b);
	vec3& 		operator-=(vec3& a, const vec3& b);
	vec4& 		operator-=(vec4& a, const vec4& b);

	vec2& 		operator*=(vec2& v, f32 s);
	vec3& 		operator*=(vec3& v, f32 s);
	vec4& 		operator*=(vec4& v, f32 s);

	vec2& 		operator/=(vec2& v, f32 s);
	vec3& 		operator/=(vec3& v, f32 s);
	vec4& 		operator/=(vec4& v, f32 s);

	bool 		operator==(const vec2& a, const vec2& b);
	bool 		operator==(const vec3& a, const vec3& b);
	bool 		operator==(const vec4& a, const vec4& b);

	bool 		operator!=(const vec2& a, const vec2& b);
	bool		operator!=(const vec3& a, const vec3& b);
	bool 		operator!=(const vec4& a, const vec4& b);

	f32 		dot(const vec2& a, const vec2& b);
	f32 		dot(const vec3& a, const vec3& b);
	f32 		dot(const vec4& a, const vec4& b);

	f32 		length(const vec2& v);
	f32 		length(const vec3& v);
	f32 		length(const vec4& v);

	vec2		normalize(const vec2& v);
	vec3 		normalize(const vec3& v);
	vec4	 	normalize(const vec4& v);

	vec3 		cross(const vec3& a, const vec3& b);

	std::string to_string(const vec2& v);
	std::string to_string(const vec3& v);
	std::string to_string(const vec4& v);

	f32* 		value_ptr(vec2& v);
	const f32* 	value_ptr(const vec2& v);
	f32* 		value_ptr(vec3& v);
	const f32* 	value_ptr(const vec3& v);
	f32* 		value_ptr(vec4& v);
	const f32* 	value_ptr(const vec4& v);

	vec2 		make_vec2(const f32* ptr);
	vec3 		make_vec3(const f32* ptr);
	vec4 		make_vec4(const f32* ptr);

	/// Signed 32-bit Integer Vector ///

	ivec2 		operator-(const ivec2& v);
	ivec3 		operator-(const ivec3& v);
	ivec4 		operator-(const ivec4& v);

	ivec2 		operator+(const ivec2& a, const ivec2& b);
	ivec3 		operator+(const ivec3& a, const ivec3& b);
	ivec4 		operator+(const ivec4& a, const ivec4& b);

	ivec2 		operator-(const ivec2& a, const ivec2& b);
	ivec3	 	operator-(const ivec3& a, const ivec3& b);
	ivec4 		operator-(const ivec4& a, const ivec4& b);

	ivec2 		operator*(const ivec2& v, i32 s);
	ivec3 		operator*(const ivec3& v, i32 s);
	ivec4 		operator*(const ivec4& v, i32 s);
	ivec2 		operator*(i32 s, const ivec2& v);
	ivec3 		operator*(i32 s, const ivec3& v);
	ivec4 		operator*(i32 s, const ivec4& v);

	ivec2 		operator/(const ivec2& v, i32 s);
	ivec3		operator/(const ivec3& v, i32 s);
	ivec4 		operator/(const ivec4& v, i32 s);

	ivec2& 		operator+=(ivec2& a, const ivec2& b);
	ivec3& 		operator+=(ivec3& a, const ivec3& b);
	ivec4& 		operator+=(ivec4& a, const ivec4& b);

	ivec2& 		operator-=(ivec2& a, const ivec2& b);
	ivec3& 		operator-=(ivec3& a, const ivec3& b);
	ivec4& 		operator-=(ivec4& a, const ivec4& b);

	ivec2& 		operator*=(ivec2& v, i32 s);
	ivec3& 		operator*=(ivec3& v, i32 s);
	ivec4& 		operator*=(ivec4& v, i32 s);

	ivec2& 		operator/=(ivec2& v, i32 s);
	ivec3& 		operator/=(ivec3& v, i32 s);
	ivec4& 		operator/=(ivec4& v, i32 s);

	bool 		operator==(const ivec2& a, const ivec2& b);
	bool 		operator==(const ivec3& a, const ivec3& b);
	bool 		operator==(const ivec4& a, const ivec4& b);

	bool 		operator!=(const ivec2& a, const ivec2& b);
	bool 		operator!=(const ivec3& a, const ivec3& b);
	bool 		operator!=(const ivec4& a, const ivec4& b);

	i32 		dot(const ivec2& a, const ivec2& b);
	i32 		dot(const ivec3& a, const ivec3& b);
	i32 		dot(const ivec4& a, const ivec4& b);

	std::string to_string(const ivec2& v);
	std::string to_string(const ivec3& v);
	std::string to_string(const ivec4& v);

	i32* 		value_ptr(ivec2& v);
	const i32* 	value_ptr(const ivec2& v);
	i32* 		value_ptr(ivec3& v);
	const i32* 	value_ptr(const ivec3& v);
	i32* 		value_ptr(ivec4& v) ;
	const i32* 	value_ptr(const ivec4& v);

	ivec2 		make_vec2(const i32* ptr);
	ivec3 		make_vec3(const i32* ptr);
	ivec4 		make_vec4(const i32* ptr);

	/// Unsigned 32-bit Integer Vector ///

	uvec2 		operator+(const uvec2& a, const uvec2& b);
	uvec3 		operator+(const uvec3& a, const uvec3& b);
	uvec4 		operator+(const uvec4& a, const uvec4& b);

	uvec2 		operator-(const uvec2& a, const uvec2& b);
	uvec3 		operator-(const uvec3& a, const uvec3& b);
	uvec4 		operator-(const uvec4& a, const uvec4& b);

	uvec2 		operator*(const uvec2& v, u32 s);
	uvec3 		operator*(const uvec3& v, u32 s);
	uvec4 		operator*(const uvec4& v, u32 s);
	uvec2 		operator*(u32 s, const uvec2& v);
	uvec3 		operator*(u32 s, const uvec3& v);
	uvec4 		operator*(u32 s, const uvec4& v);

	uvec2 		operator/(const uvec2& v, u32 s);
	uvec3 		operator/(const uvec3& v, u32 s);
	uvec4 		operator/(const uvec4& v, u32 s);

	uvec2& 		operator+=(uvec2& a, const uvec2& b);
	uvec3& 		operator+=(uvec3& a, const uvec3& b);
	uvec4& 		operator+=(uvec4& a, const uvec4& b);

	uvec2& 		operator-=(uvec2& a, const uvec2& b);
	uvec3& 		operator-=(uvec3& a, const uvec3& b);
	uvec4& 		operator-=(uvec4& a, const uvec4& b);

	uvec2& 		operator*=(uvec2& v, u32 s);
	uvec3& 		operator*=(uvec3& v, u32 s);
	uvec4& 		operator*=(uvec4& v, u32 s);

	uvec2& 		operator/=(uvec2& v, u32 s);
	uvec3& 		operator/=(uvec3& v, u32 s);
	uvec4& 		operator/=(uvec4& v, u32 s);

	bool 		operator==(const uvec2& a, const uvec2& b);
	bool 		operator==(const uvec3& a, const uvec3& b);
	bool 		operator==(const uvec4& a, const uvec4& b);

	bool 		operator!=(const uvec2& a, const uvec2& b);
	bool 		operator!=(const uvec3& a, const uvec3& b);
	bool 		operator!=(const uvec4& a, const uvec4& b);

	u32 		dot(const uvec2& a, const uvec2& b);
	u32 		dot(const uvec3& a, const uvec3& b);
	u32 		dot(const uvec4& a, const uvec4& b);

	std::string to_string(const uvec2& v);
	std::string to_string(const uvec3& v);
	std::string to_string(const uvec4& v);

	u32* 		value_ptr(uvec2& v);
	const u32* 	value_ptr(const uvec2& v);
	u32* 		value_ptr(uvec3& v);
	const u32* 	value_ptr(const uvec3& v);
	u32* 		value_ptr(uvec4& v) ;
	const u32* 	value_ptr(const uvec4& v);

	uvec2 		make_uvec2(const u32* ptr);
	uvec3 		make_uvec3(const u32* ptr);
	uvec4 		make_uvec4(const u32* ptr);

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

	u8vec2     	make_u8vec2(const u8* ptr);
	u8vec3     	make_u8vec3(const u8* ptr);
	u8vec4     	make_u8vec4(const u8* ptr);

	std::string to_string(const u8vec2& v);
	std::string to_string(const u8vec3& v);
	std::string to_string(const u8vec4& v);

	/// 32-bit Float Matrix ///

	mat2 		identity2();
	mat3 		identity3();
	mat4 		identity4();

	mat2 		zero2();
	mat3 		zero3();
	mat4 		zero4();

	vec2 		operator*(const mat2& m, const vec2& v);
	vec3 		operator*(const mat3& m, const vec3& v);
	vec4 		operator*(const mat4& m, const vec4& v);

	mat2 		operator*(const mat2& a, const mat2& b);
	mat3 		operator*(const mat3& a, const mat3& b);
	mat4 		operator*(const mat4& a, const mat4& b);

	mat2& 		operator*=(mat2& a, const mat2& b);
	mat3& 		operator*=(mat3& a, const mat3& b);
	mat4& 		operator*=(mat4& a, const mat4& b);

	bool 		operator==(const mat2& a, const mat2& b);
	bool 		operator!=(const mat2& a, const mat2& b);

	bool 		operator==(const mat3& a, const mat3& b);
	bool		operator!=(const mat3& a, const mat3& b);

	bool 		operator==(const mat4& a, const mat4& b);
	bool 		operator!=(const mat4& a, const mat4& b);

	mat2 		transpose(const mat2& m);
	mat3 		transpose(const mat3& m);
	mat4 		transpose(const mat4& m);

	f32 		determinant(const mat2& m);
	f32			determinant(const mat3& m);
	f32 		determinant(const mat4& m);

	mat2 		inverse(const mat2& m);
	mat3	 	inverse(const mat3& m);
	mat4 		inverse(const mat4& m);

	mat4 		translate(const mat4& m, const vec3& v);
	mat4 		rotate(const mat4& m, f32 angle, const vec3& v);
	mat4 		scale(const mat4& m, const vec3& v);
	mat4 		scale(const mat4& m, f32 s);

	std::string to_string(const mat2& m);
	std::string to_string(const mat3& m);
	std::string to_string(const mat4& m);

	f32* 		value_ptr(mat2& m);
	const f32* 	value_ptr(const mat2& m);
	f32* 		value_ptr(mat3& m);
	const f32* 	value_ptr(const mat3& m);
	f32* 		value_ptr(mat4& m);
	const f32* 	value_ptr(const mat4& m);

	mat2 		make_mat2(const f32* ptr);
	mat3 		make_mat3(const f32* ptr);
	mat4 		make_mat4(const f32* ptr);

	/// 32-bit Float Quaternion ///

	quat 		identity_quat();

	quat 		operator*(const quat& a, const quat& b);

	quat 		operator+(const quat& a, const quat& b);
	quat 		operator*(const quat& q, f32 s);
	quat 		operator*(f32 s, const quat& q);

	quat& 		operator+=(quat& a, const quat& b);
	quat& 		operator*=(quat& q, f32 s);
	quat& 		operator*=(quat& a, const quat& b);

	bool 		operator==(const quat& a, const quat& b);
	bool 		operator!=(const quat& a, const quat& b);

	f32  		dot(const quat& a, const quat& b);
	f32  		length(const quat& q);
	quat 		normalize(const quat& q);
	quat 		conjugate(const quat& q);
	quat		inverse(const quat& q);

	vec3 		rotate(const quat& q, const vec3& v);
	vec3 		operator*(const quat& q, const vec3& v);

	quat 		axis_angle(const vec3& axis, f32 angle);
	vec4 		axis_angle(const quat& q);

	quat 		euler_angles(const vec3& angles); // pitch/yaw/roll to quaternion, y–x–z order
	vec3 		euler_angles(const quat& q); // pitch/yaw/roll to quaternion, y–x–z order

	quat 		slerp(const quat& a, const quat& b, f32 t);

	mat3 		mat3_cast(quat const& q);
	mat4 		mat4_cast(quat const& q);

	std::string to_string(const quat& q);

	f32* 		value_ptr(quat& q) ;
	const f32* 	value_ptr(const quat& q);

	quat 		make_quat(const f32* ptr);

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



	mat4 perspective(f32 fovy, f32 aspect, f32 zNear, f32 zFar); // rh
	mat4 ortho(f32 left, f32 right, f32 bottom, f32 top, f32 zNear, f32 zFar); // rh
	mat4 look_at(const vec3& eye, const vec3& center, const vec3& up); // rh

	bool decompose_trs(const mat4& m,
					   vec3& scale,
					   quat& rotation,
					   vec3& translation);

	// genTypeT mix(genTypeT x, genTypeT y, genTypeU a);
	// vec<L, T, Q> mix(vec<L, T, Q> const& x, vec<L, T, Q> const& y, vec<L, U, Q> const& a);
	// vec<L, T, Q> mix(vec<L, T, Q> const& x, vec<L, T, Q> const& y, U a);

	// EASING

	// RANDOM?

	/// Constants ///

	inline f32 e() 			{ return 2.7182818284590452354; }	// e
	inline f32 log2e() 		{ return 1.4426950408889634074; }	// log_2 e
	inline f32 log10e() 	{ return 0.43429448190325182765; }	// log_10 e
	inline f32 nl2() 		{ return 0.69314718055994530942; }	// log_e 2
	inline f32 nl10() 		{ return 2.30258509299404568402; }	// log_e 10
	inline f32 pi() 		{ return 3.14159265358979323846; }	// pi
	inline f32 pi_over_2()	{ return 1.57079632679489661923; }	// pi/2
	inline f32 pi_over_4() 	{ return 0.78539816339744830962; }	// pi/4

//# define M_1_PI		0.31830988618379067154	// 1/pi
//# define M_2_PI		0.63661977236758134308	// 2/pi
//# define M_2_SQRTPI	1.12837916709551257390	// 2/sqrt(pi)
//# define M_SQRT2	1.41421356237309504880	// sqrt(2)
//# define M_SQRT1_2	0.70710678118654752440	// 1/sqrt(2)
}

#endif //AVARA3D_MATH_H
