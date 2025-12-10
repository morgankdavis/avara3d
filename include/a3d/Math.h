//
//  Math.h
//  avara3d
//
//  Created by Morgan Davis on 12/7/25.
//  Copyright © 2025 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_MATH_H
#define AVARA3D_MATH_H

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <type_traits>

namespace a3d::math {

	using f32 = float;
	using i32 = std::int32_t;

//	class vec2;
//	class vec3;
//	class vec4;
//	class ivec2;
//	class ivec3;
//	class ivec4;

	struct vec2 {
		f32 x, y;
		vec2() = default;
		vec2(f32 x_, f32 y_);
		f32& operator[](std::size_t i);
		const f32& operator[](std::size_t i) const;
	};

	struct vec3 {
		f32 x, y, z;
		vec3() = default;
		vec3(f32 x_, f32 y_, f32 z_);
//		explicit vec2(const vec3& v);
//		explicit vec2(const vec4& v);
		f32& operator[](std::size_t i);
		const f32& operator[](std::size_t i) const;
	};

	struct vec4 {
		f32 x, y, z, w;
		vec4() = default;
		vec4(f32 x_, f32 y_, f32 z_, f32 w_);
		f32& operator[](std::size_t i);
		const f32& operator[](std::size_t i) const;
	};

	struct ivec2 {
		i32 x, y;
		ivec2() = default;
		ivec2(i32 x_, i32 y_);
		i32& operator[](std::size_t i);
		const i32& operator[](std::size_t i) const;
	};

	struct ivec3 {
		i32 x, y, z;
		ivec3() = default;
		ivec3(i32 x_, i32 y_, i32 z_);
		i32& operator[](std::size_t i);
		const i32& operator[](std::size_t i) const;
	};

	struct ivec4 {
		i32 x, y, z, w;
		ivec4() = default;
		ivec4(i32 x_, i32 y_, i32 z_, i32 w_);
		i32& operator[](std::size_t i);
		const i32& operator[](std::size_t i) const;
	};

	struct mat2 {
		vec2 c0, c1;
		mat2() = default;
		explicit mat2(f32 diag);
		vec2& operator[](std::size_t i);
		const vec2& operator[](std::size_t i) const;
	};

	struct mat3 {
		vec3 c0, c1, c2;
		mat3() = default;
		explicit mat3(f32 diag);
		vec3& operator[](std::size_t i);
		const vec3& operator[](std::size_t i) const;
	};

	struct mat4 {
		vec4 c0, c1, c2, c3;
		mat4() = default;
		explicit mat4(f32 diag);
		explicit mat4(const mat3& m);
		vec4& operator[](std::size_t i);
		const vec4& operator[](std::size_t i) const;
	};

	struct quat {
		f32 w, x, y, z;
		quat() = default;
		quat(f32 w_, f32 x_, f32 y_, f32 z_);
	};

	/// Float Vector Operations ///

	vec2 operator+(const vec2& a, const vec2& b);
	vec3 operator+(const vec3& a, const vec3& b);
	vec4 operator+(const vec4& a, const vec4& b);

	vec2 operator-(const vec2& a, const vec2& b);
	vec3 operator-(const vec3& a, const vec3& b);
	vec4 operator-(const vec4& a, const vec4& b);

	vec2 operator*(const vec2& v, f32 s);
	vec3 operator*(const vec3& v, f32 s);
	vec4 operator*(const vec4& v, f32 s);
	vec2 operator*(f32 s, const vec2& v);
	vec3 operator*(f32 s, const vec3& v);
	vec4 operator*(f32 s, const vec4& v);

	vec2 operator/(const vec2& v, f32 s);
	vec3 operator/(const vec3& v, f32 s);
	vec4 operator/(const vec4& v, f32 s);

	f32 dot(const vec2& a, const vec2& b);
	f32 dot(const vec3& a, const vec3& b);
	f32 dot(const vec4& a, const vec4& b);

	f32 length(const vec2& v);
	f32 length(const vec3& v);
	f32 length(const vec4& v);

	vec2 normalize(const vec2& v);
	vec3 normalize(const vec3& v);
	vec4 normalize(const vec4& v);

	vec3 cross(const vec3& a, const vec3& b);

	f32* value_ptr(vec2& v);
	const f32* value_ptr(const vec2& v);
	f32* value_ptr(vec3& v);
	const f32* value_ptr(const vec3& v);
	f32* value_ptr(vec4& v);
	const f32* value_ptr(const vec4& v);

	/// Integer Vector Operations ///

	ivec2 operator+(const ivec2& a, const ivec2& b);
	ivec3 operator+(const ivec3& a, const ivec3& b);
	ivec4 operator+(const ivec4& a, const ivec4& b);

	ivec2 operator-(const ivec2& a, const ivec2& b);
	ivec3 operator-(const ivec3& a, const ivec3& b);
	ivec4 operator-(const ivec4& a, const ivec4& b);

	ivec2 operator*(const ivec2& v, i32 s);
	ivec3 operator*(const ivec3& v, i32 s);
	ivec4 operator*(const ivec4& v, i32 s);
	ivec2 operator*(i32 s, const ivec2& v);
	ivec3 operator*(i32 s, const ivec3& v);
	ivec4 operator*(i32 s, const ivec4& v);

	ivec2 operator/(const ivec2& v, i32 s);
	ivec3 operator/(const ivec3& v, i32 s);
	ivec4 operator/(const ivec4& v, i32 s);

	i32 dot(const ivec2& a, const ivec2& b);
	i32 dot(const ivec3& a, const ivec3& b);
	i32 dot(const ivec4& a, const ivec4& b);

	i32* value_ptr(ivec2& v);
	const i32* value_ptr(const ivec2& v);
	i32* value_ptr(ivec3& v);
	const i32* value_ptr(const ivec3& v);
	i32* value_ptr(ivec4& v) ;
	const i32* value_ptr(const ivec4& v);

	/// Matrix Operations ///

	mat2 identity2();
	mat3 identity3();
	mat4 identity4();

	mat2 zero2();
	mat3 zero3();
	mat4 zero4();

	f32* value_ptr(mat2& m);
	const f32* value_ptr(const mat2& m);
	f32* value_ptr(mat3& m);
	const f32* value_ptr(const mat3& m);
	f32* value_ptr(mat4& m);
	const f32* value_ptr(const mat4& m);

	vec2 operator*(const mat2& m, const vec2& v);
	vec3 operator*(const mat3& m, const vec3& v);
	vec4 operator*(const mat4& m, const vec4& v);

	mat2 operator*(const mat2& a, const mat2& b);
	mat3 operator*(const mat3& a, const mat3& b);
	mat4 operator*(const mat4& a, const mat4& b);

	mat2 transpose(const mat2& m);
	mat3 transpose(const mat3& m);
	mat4 transpose(const mat4& m);

	f32 determinant(const mat2& m);
	f32 determinant(const mat3& m);
	f32 determinant(const mat4& m);

	mat2 inverse(const mat2& m);
	mat3 inverse(const mat3& m);
	mat4 inverse(const mat4& m);

	/// Quaternion Operations ///

	quat identity_quat();

	f32  dot(const quat& a, const quat& b);
	f32  length(const quat& q);
	quat normalize(const quat& q);
	quat conjugate(const quat& q);
	quat inverse(const quat& q);

	quat operator*(const quat& a, const quat& b);

	quat operator+(const quat& a, const quat& b);
	quat operator*(const quat& q, f32 s);
	quat operator*(f32 s, const quat& q);

	vec3 rotate(const quat& q, const vec3& v);
	vec3 operator*(const quat& q, const vec3& v);

	quat angle_axis(f32 angle, const vec3& axis);

	quat slerp(const quat& a, const quat& b, f32 t);

	mat3 mat3_cast(quat const& q);
	mat4 mat4_cast(quat const& q);

	f32* value_ptr(quat& q) ;
	const f32* value_ptr(const quat& q);

	/// Meh ///

	f32 radians(f32 degrees);
	f32 degrees(f32 radians);

	f32 sin(f32 num);
	f32 cos(f32 num);
	f32 tan(f32 num);
	f32 asin(f32 num);
	f32 acos(f32 num);
	f32 atan(f32 num);
	f32 sinh(f32 num);
	f32 cosh(f32 num);
	f32 tanh(f32 num);
	f32 asinh(f32 num);
	f32 acosh(f32 num);
	f32 atanh(f32 num);

	f32 ceil(f32 num);
	f32 floor(f32 num);
	f32 round(f32 num);
	f32 exp(f32 num);
	f32 pow(f32 x, f32 y);
	f32 abs(f32 num);
	f32 log(f32 num);
	f32 log10(f32 num);
	f32 sqrt(f32 num);
	f32 min(f32 a, f32 b);
	f32 max(f32 a, f32 b);
	f32	clamp(f32 val, f32 low, f32 high);
	void swap(f32& a, f32& b);



	f32 epsilon();



	// genTypeT mix(genTypeT x, genTypeT y, genTypeU a);
	// vec<L, T, Q> mix(vec<L, T, Q> const& x, vec<L, T, Q> const& y, vec<L, U, Q> const& a);
	// vec<L, T, Q> mix(vec<L, T, Q> const& x, vec<L, T, Q> const& y, U a);




//	vec<2, T, Q> make_vec2(vec<1, T, Q> const& v);
//	vec<2, T, Q> make_vec2(vec<2, T, Q> const& v);
//	vec<2, T, Q> make_vec2(vec<3, T, Q> const& v);
//	vec<2, T, Q> make_vec2(vec<4, T, Q> const& v);
//	vec<3, T, Q> make_vec3(vec<1, T, Q> const& v);
//	vec<3, T, Q> make_vec3(vec<2, T, Q> const& v);
//	vec<3, T, Q> make_vec3(vec<3, T, Q> const& v);
//	vec<3, T, Q> make_vec3(vec<4, T, Q> const& v);
//	vec<4, T, Q> make_vec4(vec<1, T, Q> const& v);
//	vec<4, T, Q> make_vec4(vec<2, T, Q> const& v);
//	vec<4, T, Q> make_vec4(vec<3, T, Q> const& v);
//	vec<4, T, Q> make_vec4(vec<4, T, Q> const& v);
//	vec<2, T, defaultp> make_vec2(T const * const ptr);
//	vec<3, T, defaultp> make_vec3(T const * const ptr);
//	vec<4, T, defaultp> make_vec4(T const * const ptr);
//	mat<2, 2, T, defaultp> make_mat2x2(T const * const ptr);
//	mat<3, 3, T, defaultp> make_mat3x3(T const * const ptr);
//	mat<4, 4, T, defaultp> make_mat4x4(T const * const ptr);
//	mat<2, 2, T, defaultp> make_mat2(T const * const ptr);
//	mat<3, 3, T, defaultp> make_mat3(T const * const ptr);
//	mat<4, 4, T, defaultp> make_mat4(T const * const ptr);
//	qua<T, defaultp> make_quat(T const * const ptr);





// ivec operations?

// EASING

// RANDOM?


/*
 	lookAt()

	mat4_cast

	# define M_E		2.7182818284590452354	// e
	# define M_LOG2E	1.4426950408889634074	// log_2 e
	# define M_LOG10E	0.43429448190325182765	// log_10 e
	# define M_LN2		0.69314718055994530942	// log_e 2
	# define M_LN10		2.30258509299404568402	// log_e 10
	# define M_PI		3.14159265358979323846	// pi
	# define M_PI_2		1.57079632679489661923	// pi/2
	# define M_PI_4		0.78539816339744830962	// pi/4
	# define M_1_PI		0.31830988618379067154	// 1/pi
	# define M_2_PI		0.63661977236758134308	// 2/pi
	# define M_2_SQRTPI	1.12837916709551257390	// 2/sqrt(pi)
	# define M_SQRT2	1.41421356237309504880	// sqrt(2)
	# define M_SQRT1_2	0.70710678118654752440	// 1/sqrt(2)
*/

	bool decompose(const mat4& ModelMatrix,
				   vec3 Scale,
				   quat& Orientation,
				   vec3& Translation,
				   vec3& Skew,
				   vec4& Perspective);
}

#endif //AVARA3D_MATH_H
