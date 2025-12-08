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

	struct vec2;
	struct vec3;
	struct vec4;

	namespace detail {

		template <typename Derived, typename T, std::size_t N>
		struct vec {
			T& operator[](std::size_t i) {
				assert(i < N);
				return (&static_cast<Derived*>(this)->x)[i];
			}

			const T& operator[](std::size_t i) const {
				assert(i < N);
				return (&static_cast<const Derived*>(this)->x)[i];
			}
		};

		template <typename Derived, typename ColT, std::size_t NCols>
		struct mat {
			ColT& operator[](std::size_t i) {
				assert(i < NCols);
				return (&static_cast<Derived*>(this)->c0)[i];
			}

			const ColT& operator[](std::size_t i) const {
				assert(i < NCols);
				return (&static_cast<const Derived*>(this)->c0)[i];
			}
		};

		template <typename T> struct is_vec : std::false_type {};
		template <> struct is_vec<vec2> : std::true_type {};
		template <> struct is_vec<vec3> : std::true_type {};
		template <> struct is_vec<vec4> : std::true_type {};

		template <typename T>
		constexpr bool is_vec_v = is_vec<T>::value;

		template <typename T> struct vec_size;
		template <> struct vec_size<vec2> : std::integral_constant<std::size_t, 2> {};
		template <> struct vec_size<vec3> : std::integral_constant<std::size_t, 3> {};
		template <> struct vec_size<vec4> : std::integral_constant<std::size_t, 4> {};

		template <typename T>
		constexpr std::size_t vec_size_v = vec_size<T>::value;

		template <typename V>
		f32* data(V& v) {
			static_assert(is_vec_v<V>, "math::detail::data only valid for vec2/3/4");
			return &v.x;
		}

		template <typename V>
		const f32* data(const V& v) {
			static_assert(is_vec_v<V>, "math::detail::data only valid for vec2/3/4");
			return &v.x;
		}
	}

	struct vec2 : detail::vec<vec2, f32, 2> {
		f32 x, y;

		vec2() = default;
		vec2(f32 x_, f32 y_) : x{x_}, y{y_} {}
	};

	struct vec3 : detail::vec<vec3, f32, 3> {
		f32 x, y, z;

		vec3() = default;
		vec3(f32 x_, f32 y_, f32 z_) : x{x_}, y{y_}, z{z_} {}
	};

	struct vec4 : detail::vec<vec4, f32, 4> {
		f32 x, y, z, w;

		vec4() = default;
		vec4(f32 x_, f32 y_, f32 z_, f32 w_) : x{x_}, y{y_}, z{z_}, w{w_} {}
	};

	struct ivec2 : detail::vec<ivec2, i32, 2> {
		i32 x, y;

		ivec2() = default;
		ivec2(i32 x_, i32 y_) : x{x_}, y{y_} {}
	};

	struct ivec3 : detail::vec<ivec3, i32, 3> {
		i32 x, y, z;

		ivec3() = default;
		ivec3(i32 x_, i32 y_, i32 z_) : x{x_}, y{y_}, z{z_} {}
	};

	struct ivec4 : detail::vec<ivec4, i32, 4> {
		i32 x, y, z, w;

		ivec4() = default;
		ivec4(i32 x_, i32 y_, i32 z_, i32 w_) : x{x_}, y{y_}, z{z_}, w{w_} {}
	};

	struct mat2 : detail::mat<mat2, vec2, 2> {
		vec2 c0, c1;

		mat2() = default;

		explicit mat2(f32 diag):
				c0{ diag, 0.0f },
				c1{ 0.0f, diag } {}
	};

	struct mat3 : detail::mat<mat3, vec3, 3> {
		vec3 c0, c1, c2;

		mat3() = default;

		explicit mat3(f32 diag):
				c0{ diag, 0.0f, 0.0f },
				c1{ 0.0f, diag, 0.0f },
				c2{ 0.0f, 0.0f, diag } {}
	};

	struct mat4 : detail::mat<mat4, vec4, 4> {
		vec4 c0, c1, c2, c3;

		mat4() = default;

		explicit mat4(f32 diag):
				c0{ diag, 0.0f, 0.0f, 0.0f },
				c1{ 0.0f, diag, 0.0f, 0.0f },
				c2{ 0.0f, 0.0f, diag, 0.0f },
				c3{ 0.0f, 0.0f, 0.0f, diag } {}

		explicit mat4(const mat3& m):
				c0{ m.c0.x, m.c0.y, m.c0.z, 0.0f },
				c1{ m.c1.x, m.c1.y, m.c1.z, 0.0f },
				c2{ m.c2.x, m.c2.y, m.c2.z, 0.0f },
				c3{ 0.0f,   0.0f,   0.0f,   1.0f } {}
	};

	struct quat { f32 w, x, y, z; };

	vec2 operator*(const mat2& m, const vec2& v);
	vec3 operator*(const mat3& m, const vec3& v);
	vec4 operator*(const mat4& m, const vec4& v);

	mat2 operator*(const mat2& a, const mat2& b);
	mat3 operator*(const mat3& a, const mat3& b);
	mat4 operator*(const mat4& a, const mat4& b);

	mat2 transpose(const mat2& m);
	mat3 transpose(const mat3& m);
	mat4 transpose(const mat4& m);

	mat2 inverse(const mat2& m);
	mat3 inverse(const mat3& m);
	mat4 inverse(const mat4& m);

	//f32 determinant(mat2 const& m);
	//f32 determinant(mat3 const& m);
	//f32 determinant(mat4 const& m);

	// v + v
	template <typename V, std::enable_if_t<detail::is_vec_v<V>, int> = 0>
	V operator+(const V& a, const V& b) {
		V r{};
		const auto* pa = detail::data(a);
		const auto* pb = detail::data(b);
		auto* pr = detail::data(r);
		for (std::size_t i = 0; i < detail::vec_size_v<V>; ++i)
			pr[i] = pa[i] + pb[i];
		return r;
	}

	// v - v
	template <typename V, std::enable_if_t<detail::is_vec_v<V>, int> = 0>
	V operator-(const V& a, const V& b) {
		V r{};
		const auto* pa = detail::data(a);
		const auto* pb = detail::data(b);
		auto* pr = detail::data(r);
		for (std::size_t i = 0; i < detail::vec_size_v<V>; ++i)
			pr[i] = pa[i] - pb[i];
		return r;
	}

	// v * scalar
	template <typename V, typename S,
			std::enable_if_t<detail::is_vec_v<V> && std::is_arithmetic_v<S>, int> = 0>
	V operator*(const V& v, S s) {
		V r{};
		const auto* pv = detail::data(v);
		auto* pr = detail::data(r);
		const f32 sf = static_cast<f32>(s);
		for (std::size_t i = 0; i < detail::vec_size_v<V>; ++i)
			pr[i] = pv[i] * sf;
		return r;
	}

	// scalar * v
	template <typename V, typename S,
	        std::enable_if_t<detail::is_vec_v<V> && std::is_arithmetic_v<S>, int> = 0>
	V operator*(S s, const V& v) {
		return v * s;
	}

	// v / scalar
	template <typename V, typename S,
			std::enable_if_t<detail::is_vec_v<V> && std::is_arithmetic_v<S>, int> = 0>
	V operator/(const V& v, S s) {
		V r{};
		const auto* pv = detail::data(v);
		auto* pr = detail::data(r);
		const f32 inv = static_cast<f32>(1.0f / s);
		for (std::size_t i = 0; i < detail::vec_size_v<V>; ++i)
			pr[i] = pv[i] * inv;
		return r;
	}

	// (optional) compound ops
	template <typename V, std::enable_if_t<detail::is_vec_v<V>, int> = 0>
	V& operator+=(V& a, const V& b) {
		auto* pa = detail::data(a);
		const auto* pb = detail::data(b);
		for (std::size_t i = 0; i < detail::vec_size_v<V>; ++i)
			pa[i] += pb[i];
		return a;
	}

	template <typename V, std::enable_if_t<detail::is_vec_v<V>, int> = 0>
	V& operator-=(V& a, const V& b) {
		auto* pa = detail::data(a);
		const auto* pb = detail::data(b);
		for (std::size_t i = 0; i < detail::vec_size_v<V>; ++i)
			pa[i] -= pb[i];
		return a;
	}

	template <typename V, typename S,
	        std::enable_if_t<detail::is_vec_v<V> && std::is_arithmetic_v<S>, int> = 0>
	V& operator*=(V& v, S s) {
		auto* pv = detail::data(v);
		const f32 sf = static_cast<f32>(s);
		for (std::size_t i = 0; i < detail::vec_size_v<V>; ++i)
			pv[i] *= sf;
		return v;
	}

	template <typename V, typename S,
	        std::enable_if_t<detail::is_vec_v<V> && std::is_arithmetic_v<S>, int> = 0>
	V& operator/=(V& v, S s) {
		auto* pv = detail::data(v);
		const f32 inv = static_cast<f32>(1.0f / s);
		for (std::size_t i = 0; i < detail::vec_size_v<V>; ++i)
			pv[i] *= inv;
		return v;
	}

	template <typename V, std::enable_if_t<detail::is_vec_v<V>, int> = 0>
	f32 dot(const V& a, const V& b) {
		const auto* pa = detail::data(a);
		const auto* pb = detail::data(b);
		f32 s = 0.0f;
		for (std::size_t i = 0; i < detail::vec_size_v<V>; ++i)
			s += pa[i] * pb[i];
		return s;
	}

	vec3 cross(const vec3& a, const vec3& b) {
		return vec3{ a.y * b.z - a.z * b.y,
					 a.z * b.x - a.x * b.z,
					 a.x * b.y - a.y * b.x };
	}

	template <typename V, std::enable_if_t<detail::is_vec_v<V>, int> = 0>
	f32 length(const V& v) {
		return std::sqrt(dot(v, v));
	}

	template <typename V, std::enable_if_t<detail::is_vec_v<V>, int> = 0>
	V normalize(const V& v) {
		f32 len = length(v);
		return (len > 0.0f) ? v / len : v;
	}

	mat4 translate(const mat4& m, const vec3& v);
	mat4 rotate(const mat4& m, f32 angle, const vec3& v);
	mat4 scale(const mat4& m, const vec3& v);

	mat4 perspective(f32 fovy, f32 aspect, f32 zNear, f32 zFar);
	mat4 ortho(f32 left, f32 right, f32 bottom, f32 top, f32 zNear, f32 zFar);

	bool decompose(const mat3& mat,
				   vec3& scale,
				   quat& orientation,
				   vec3& translation,
				   vec3& skew,
				   vec4& perspective);

	mat3 mat3_cast(quat const& q);
	mat4 mat4_cast(quat const& q);

	template <typename V>
	auto value_ptr(V& v) -> decltype(&v.x) {
		static_assert(std::is_standard_layout_v<V>,
					  "value_ptr(vec): needs standard-layout type");
		return &v.x;
	}

	template <typename V>
	auto value_ptr(const V& v) -> decltype(&v.x) {
		return &v.x;
	}

	template <typename M>
	auto value_ptr(M& m) -> decltype(&m.c0.x) {
		static_assert(std::is_standard_layout_v<M>,
					  "value_ptr(mat): needs standard-layout type");
		return &m.c0.x;
	}

	template <typename M>
	auto value_ptr(const M& m) -> decltype(&m.c0.x) {
		return &m.c0.x;
	}

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

	// genTypeT mix(genTypeT x, genTypeT y, genTypeU a);
	// vec<L, T, Q> mix(vec<L, T, Q> const& x, vec<L, T, Q> const& y, vec<L, U, Q> const& a);
	// vec<L, T, Q> mix(vec<L, T, Q> const& x, vec<L, T, Q> const& y, U a);





//	mat<4, 4, T, Q> translate(vec<3, T, Q> const& v);
//	mat<4, 4, T, Q> rotate(T angle, vec<3, T, Q> const& v);
//	mat<4, 4, T, Q> scale(vec<3, T, Q> const& v);




//	genType length(genType x)
//	T length(vec<L, T, Q> const& v);
//	genType distance(genType const& p0, genType const& p1);
//	T distance(vec<L, T, Q> const& p0, vec<L, T, Q> const& p1);
//	T dot(T x, T y);
//	T dot(vec<L, T, Q> const& x, vec<L, T, Q> const& y);
//	vec<3, T, Q> cross(vec<3, T, Q> const& x, vec<3, T, Q> const& y);
//	vec<L, T, Q> normalize(vec<L, T, Q> const& x);



//	vec<3, T, Q> eulerAngles(qua<T, Q> const& x);
//	T roll(qua<T, Q> const& q);
//	T pitch(qua<T, Q> const& q);
//	T yaw(qua<T, Q> const& q);
//	mat<3, 3, T, Q> mat3_cast(qua<T, Q> const& q);
//	mat<4, 4, T, Q> mat4_cast(qua<T, Q> const& q);
//	qua<T, Q> quat_cast(mat<3, 3, T, Q> const& m);
//	quat_cast(mat<4, 4, T, Q> const& m4);
//	quatLookAt(vec<3, T, Q> const& direction, vec<3, T, Q> const& up);



//vec<3, T, Q> cross(vec<3, T, Q> const& v, qua<T, Q> const& q);
//vec<3, T, Q> cross(qua<T, Q> const& q, vec<3, T, Q> const& v);
//vec<3, T, Q> rotate(qua<T, Q> const& q, vec<3, T, Q> const& v);
//vec<4, T, Q> rotate(qua<T, Q> const& q, vec<4, T, Q> const& v);





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
}

#endif //AVARA3D_MATH_H
