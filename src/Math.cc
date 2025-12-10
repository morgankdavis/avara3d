//
//  Math.h
//  avara3d
//
//  Created by Morgan Davis on 12/7/25.
//  Copyright © 2025 Morgan K Davis. All rights reserved.
//

#include "a3d/Math.h"

using namespace a3d::math;

vec2::vec2(f32 x_, f32 y_) : x{x_}, y{y_} {}

f32& vec2::operator[](std::size_t i) {
	assert(i < 2);
	return (&x)[i];
}

const f32& vec2::operator[](std::size_t i) const {
	assert(i < 2);
	return (&x)[i];
}

vec3::vec3(f32 x_, f32 y_, f32 z_) : x{x_}, y{y_}, z{z_} {}

f32& vec3::operator[](std::size_t i) {
	assert(i < 3);
	return (&x)[i];
}

const f32& vec3::operator[](std::size_t i) const {
	assert(i < 3);
	return (&x)[i];
}

vec4::vec4(f32 x_, f32 y_, f32 z_, f32 w_) : x{x_}, y{y_}, z{z_}, w{w_} {}

f32& vec4::operator[](std::size_t i) {
	assert(i < 4);
	return (&x)[i];
}

const f32& vec4::operator[](std::size_t i) const {
	assert(i < 4);
	return (&x)[i];
}

ivec2::ivec2(i32 x_, i32 y_):
		x{x_}, y{y_} {}

i32& ivec2::operator[](std::size_t i) {
	assert(i < 2);
	return (&x)[i];
}

const i32& ivec2::operator[](std::size_t i) const {
	assert(i < 2);
	return (&x)[i];
}

ivec3::ivec3(i32 x_, i32 y_, i32 z_):
		x{x_}, y{y_}, z{z_} {}

i32& ivec3::operator[](std::size_t i) {
	assert(i < 3);
	return (&x)[i];
}

const i32& ivec3::operator[](std::size_t i) const {
	assert(i < 3);
	return (&x)[i];
}

ivec4::ivec4(i32 x_, i32 y_, i32 z_, i32 w_):
		x{x_}, y{y_}, z{z_}, w{w_} {}

i32& ivec4::operator[](std::size_t i) {
	assert(i < 4);
	return (&x)[i];
}

const i32& ivec4::operator[](std::size_t i) const {
	assert(i < 4);
	return (&x)[i];
}

mat2::mat2(f32 diag)
		: c0{ diag, 0.0f }
		, c1{ 0.0f, diag } {}

vec2& mat2::operator[](std::size_t i) {
	assert(i < 2);
	return (&c0)[i];
}

const vec2& mat2::operator[](std::size_t i) const {
	assert(i < 2);
	return (&c0)[i];
}

mat3::mat3(f32 diag):
		c0{ diag, 0.0f, 0.0f },
		c1{ 0.0f, diag, 0.0f },
		c2{ 0.0f, 0.0f, diag } {}

vec3& mat3::operator[](std::size_t i) {
	assert(i < 3);
	return (&c0)[i];
}

const vec3& mat3::operator[](std::size_t i) const {
	assert(i < 3);
	return (&c0)[i];
}

mat4::mat4(f32 diag):
		c0{ diag, 0.0f, 0.0f, 0.0f },
		c1{ 0.0f, diag, 0.0f, 0.0f },
		c2{ 0.0f, 0.0f, diag, 0.0f },
		c3{ 0.0f, 0.0f, 0.0f, diag } {}

mat4::mat4(const mat3& m):
		c0{ m.c0.x, m.c0.y, m.c0.z, 0.0f },
		c1{ m.c1.x, m.c1.y, m.c1.z, 0.0f },
		c2{ m.c2.x, m.c2.y, m.c2.z, 0.0f },
		c3{ 0.0f,   0.0f,   0.0f,   1.0f } {}

vec4& mat4::operator[](std::size_t i) {
	assert(i < 4);
	return (&c0)[i];
}

const vec4& mat4::operator[](std::size_t i) const {
	assert(i < 4);
	return (&c0)[i];
}

quat::quat(f32 w_, f32 x_, f32 y_, f32 z_):
		w{w_}, x{x_}, y{y_}, z{z_} {}

vec2 operator+(const vec2& a, const vec2& b) { return vec2{ a.x + b.x, a.y + b.y }; }
vec3 operator+(const vec3& a, const vec3& b) { return vec3{ a.x + b.x, a.y + b.y, a.z + b.z }; }
vec4 operator+(const vec4& a, const vec4& b) { return vec4{ a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w }; }

// v - v
vec2 operator-(const vec2& a, const vec2& b) { return vec2{ a.x - b.x, a.y - b.y }; }
vec3 operator-(const vec3& a, const vec3& b) { return vec3{ a.x - b.x, a.y - b.y, a.z - b.z }; }
vec4 operator-(const vec4& a, const vec4& b) { return vec4{ a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w }; }

// v * s
vec2 operator*(const vec2& v, f32 s) { return vec2{ v.x * s, v.y * s }; }
vec3 operator*(const vec3& v, f32 s) { return vec3{ v.x * s, v.y * s, v.z * s }; }
vec4 operator*(const vec4& v, f32 s) { return vec4{ v.x * s, v.y * s, v.z * s, v.w * s }; }
vec2 operator*(f32 s, const vec2& v) { return v * s; }
vec3 operator*(f32 s, const vec3& v) { return v * s; }
vec4 operator*(f32 s, const vec4& v) { return v * s; }

// v / s
vec2 operator/(const vec2& v, f32 s) {
	f32 inv = 1.0f / s;
	return vec2{ v.x * inv, v.y * inv };
}
vec3 operator/(const vec3& v, f32 s) {
	f32 inv = 1.0f / s;
	return vec3{ v.x * inv, v.y * inv, v.z * inv };
}
vec4 operator/(const vec4& v, f32 s) {
	f32 inv = 1.0f / s;
	return vec4{ v.x * inv, v.y * inv, v.z * inv, v.w * inv };
}

// dot
f32 dot(const vec2& a, const vec2& b) {
	return a.x * b.x + a.y * b.y;
}

f32 dot(const vec3& a, const vec3& b) {
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

f32 dot(const vec4& a, const vec4& b) {
	return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}

// length
f32 length(const vec2& v) {
	return std::sqrt(dot(v, v));
}

f32 length(const vec3& v) {
	return std::sqrt(dot(v, v));
}

f32 length(const vec4& v) {
	return std::sqrt(dot(v, v));
}

// normalize
vec2 normalize(const vec2& v) {
	f32 len = length(v);
	return (len > 0.0f) ? v / len : v;
}
vec3 normalize(const vec3& v) {
	f32 len = length(v);
	return (len > 0.0f) ? v / len : v;
}
vec4 normalize(const vec4& v) {
	f32 len = length(v);
	return (len > 0.0f) ? v / len : v;
}

// cross (vec3 only)
vec3 cross(const vec3& a, const vec3& b) {
	return vec3{ a.y * b.z - a.z * b.y,
				 a.z * b.x - a.x * b.z,
				 a.x * b.y - a.y * b.x };
}

// ----- ivec add -----

ivec2 operator+(const ivec2& a, const ivec2& b) {
	return ivec2{ a.x + b.x, a.y + b.y };
}

ivec3 operator+(const ivec3& a, const ivec3& b) {
	return ivec3{ a.x + b.x, a.y + b.y, a.z + b.z };
}

ivec4 operator+(const ivec4& a, const ivec4& b) {
	return ivec4{ a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w };
}

// ----- ivec subtract -----

ivec2 operator-(const ivec2& a, const ivec2& b) {
	return ivec2{ a.x - b.x, a.y - b.y };
}

ivec3 operator-(const ivec3& a, const ivec3& b) {
	return ivec3{ a.x - b.x, a.y - b.y, a.z - b.z };
}

ivec4 operator-(const ivec4& a, const ivec4& b) {
	return ivec4{ a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w };
}

// ----- ivec * scalar -----

ivec2 operator*(const ivec2& v, i32 s) {
	return ivec2{ v.x * s, v.y * s };
}

ivec3 operator*(const ivec3& v, i32 s) {
	return ivec3{ v.x * s, v.y * s, v.z * s };
}

ivec4 operator*(const ivec4& v, i32 s) {
	return ivec4{ v.x * s, v.y * s, v.z * s, v.w * s };
}

ivec2 operator*(i32 s, const ivec2& v) {
	return v * s;
}

ivec3 operator*(i32 s, const ivec3& v) {
	return v * s;
}

ivec4 operator*(i32 s, const ivec4& v) {
	return v * s;
}

// ----- ivec / scalar (integer division) -----

ivec2 operator/(const ivec2& v, i32 s) {
	// no zero check here; up to you if you want asserts
	return ivec2{ v.x / s, v.y / s };
}

ivec3 operator/(const ivec3& v, i32 s) {
	return ivec3{ v.x / s, v.y / s, v.z / s };
}

ivec4 operator/(const ivec4& v, i32 s) {
	return ivec4{ v.x / s, v.y / s, v.z / s, v.w / s };
}

// ----- ivec dot -----

i32 dot(const ivec2& a, const ivec2& b) {
	return a.x * b.x + a.y * b.y;
}

i32 dot(const ivec3& a, const ivec3& b) {
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

i32 dot(const ivec4& a, const ivec4& b) {
	return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}

// mat * vec
vec2 operator*(const mat2& m, const vec2& v) {
	return vec2{
			m.c0.x * v.x + m.c1.x * v.y,
			m.c0.y * v.x + m.c1.y * v.y
	};
}

vec3 operator*(const mat3& m, const vec3& v) {
	return vec3{
			m.c0.x * v.x + m.c1.x * v.y + m.c2.x * v.z,
			m.c0.y * v.x + m.c1.y * v.y + m.c2.y * v.z,
			m.c0.z * v.x + m.c1.z * v.y + m.c2.z * v.z
	};
}

vec4 operator*(const mat4& m, const vec4& v) {
	return vec4{
			m.c0.x * v.x + m.c1.x * v.y + m.c2.x * v.z + m.c3.x * v.w,
			m.c0.y * v.x + m.c1.y * v.y + m.c2.y * v.z + m.c3.y * v.w,
			m.c0.z * v.x + m.c1.z * v.y + m.c2.z * v.z + m.c3.z * v.w,
			m.c0.w * v.x + m.c1.w * v.y + m.c2.w * v.z + m.c3.w * v.w
	};
}

// mat * mat
mat2 operator*(const mat2& a, const mat2& b) {
	mat2 r(0.0f);
	r.c0 = a * b.c0;
	r.c1 = a * b.c1;
	return r;
}

mat3 operator*(const mat3& a, const mat3& b) {
	mat3 r(0.0f);
	r.c0 = a * b.c0;
	r.c1 = a * b.c1;
	r.c2 = a * b.c2;
	return r;
}

mat4 operator*(const mat4& a, const mat4& b) {
	mat4 r(0.0f);
	r.c0 = a * b.c0;
	r.c1 = a * b.c1;
	r.c2 = a * b.c2;
	r.c3 = a * b.c3;
	return r;
}

// transpose
mat2 transpose(const mat2& m) {
	mat2 r;
	r.c0 = vec2{ m.c0.x, m.c1.x };
	r.c1 = vec2{ m.c0.y, m.c1.y };
	return r;
}

mat3 transpose(const mat3& m) {
	mat3 r;
	r.c0 = vec3{ m.c0.x, m.c1.x, m.c2.x };
	r.c1 = vec3{ m.c0.y, m.c1.y, m.c2.y };
	r.c2 = vec3{ m.c0.z, m.c1.z, m.c2.z };
	return r;
}

mat4 transpose(const mat4& m) {
	mat4 r;
	r.c0 = vec4{ m.c0.x, m.c1.x, m.c2.x, m.c3.x };
	r.c1 = vec4{ m.c0.y, m.c1.y, m.c2.y, m.c3.y };
	r.c2 = vec4{ m.c0.z, m.c1.z, m.c2.z, m.c3.z };
	r.c3 = vec4{ m.c0.w, m.c1.w, m.c2.w, m.c3.w };
	return r;
}

// inverse(mat2)
mat2 inverse(const mat2& m) {
	f32 a = m.c0.x, c = m.c0.y;
	f32 b = m.c1.x, d = m.c1.y;

	f32 det = a * d - b * c;
	assert(det != 0.0f);
	f32 invDet = 1.0f / det;

	mat2 r;
	r.c0 = vec2{  d * invDet, -c * invDet };
	r.c1 = vec2{ -b * invDet,  a * invDet };
	return r;
}

// inverse(mat3) – same as we already derived
mat3 inverse(const mat3& m) {
	f32 m00 = m.c0.x, m01 = m.c1.x, m02 = m.c2.x;
	f32 m10 = m.c0.y, m11 = m.c1.y, m12 = m.c2.y;
	f32 m20 = m.c0.z, m21 = m.c1.z, m22 = m.c2.z;

	f32 inv00 =  (m11 * m22 - m12 * m21);
	f32 inv01 = -(m01 * m22 - m02 * m21);
	f32 inv02 =  (m01 * m12 - m02 * m11);

	f32 inv10 = -(m10 * m22 - m12 * m20);
	f32 inv11 =  (m00 * m22 - m02 * m20);
	f32 inv12 = -(m00 * m12 - m02 * m10);

	f32 inv20 =  (m10 * m21 - m11 * m20);
	f32 inv21 = -(m00 * m21 - m01 * m20);
	f32 inv22 =  (m00 * m11 - m01 * m10);

	f32 det = m00 * inv00 + m01 * inv10 + m02 * inv20;
	assert(det != 0.0f);
	f32 invDet = 1.0f / det;

	mat3 r;
	r.c0 = vec3{ inv00 * invDet, inv10 * invDet, inv20 * invDet };
	r.c1 = vec3{ inv01 * invDet, inv11 * invDet, inv21 * invDet };
	r.c2 = vec3{ inv02 * invDet, inv12 * invDet, inv22 * invDet };
	return r;
}

mat3 inverse(const mat3& m) {

	const f32 m00 = m.c0.x; const f32 m01 = m.c1.x; const f32 m02 = m.c2.x;
	const f32 m10 = m.c0.y; const f32 m11 = m.c1.y; const f32 m12 = m.c2.y;
	const f32 m20 = m.c0.z; const f32 m21 = m.c1.z; const f32 m22 = m.c2.z;

	// cofactors for the adjugate (before dividing by det)
	const f32 inv00 =  (m11 * m22 - m12 * m21);
	const f32 inv01 = -(m01 * m22 - m02 * m21);
	const f32 inv02 =  (m01 * m12 - m02 * m11);

	const f32 inv10 = -(m10 * m22 - m12 * m20);
	const f32 inv11 =  (m00 * m22 - m02 * m20);
	const f32 inv12 = -(m00 * m12 - m02 * m10);

	const f32 inv20 =  (m10 * m21 - m11 * m20);
	const f32 inv21 = -(m00 * m21 - m01 * m20);
	const f32 inv22 =  (m00 * m11 - m01 * m10);

	const f32 det = m00 * inv00 + m01 * inv10 + m02 * inv20;
	assert(det != 0.0f && "mat3 inverse: matrix is singular");

	const f32 invDet = 1.0f / det;

	mat3 r(0.0);
	// remember: columns are (row0, row1, row2)
	r.c0 = { inv00 * invDet, inv10 * invDet, inv20 * invDet };
	r.c1 = { inv01 * invDet, inv11 * invDet, inv21 * invDet };
	r.c2 = { inv02 * invDet, inv12 * invDet, inv22 * invDet };
	return r;
}

mat4 inverse(const mat4& m) {

	// row-major copy of m
	f32 a[4][4] = { { m.c0.x, m.c1.x, m.c2.x, m.c3.x }, // row 0
					{ m.c0.y, m.c1.y, m.c2.y, m.c3.y }, // row 1
					{ m.c0.z, m.c1.z, m.c2.z, m.c3.z }, // row 2
					{ m.c0.w, m.c1.w, m.c2.w, m.c3.w } }; // row 3

	f32 inv[4][4] = { { 1.0f, 0.0f, 0.0f, 0.0f },
					  { 0.0f, 1.0f, 0.0f, 0.0f },
					  { 0.0f, 0.0f, 1.0f, 0.0f },
					  { 0.0f, 0.0f, 0.0f, 1.0f } };

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
			std::swap(a[col],  a[pivot_row]);
			std::swap(inv[col], inv[pivot_row]);
		}

		// normalize pivot row
		const f32 pivot = a[col][col];
		const f32 inv_pivot = 1.0f / pivot;
		for (int j = 0; j < 4; ++j) {
			a[col][j]  *= inv_pivot;
			inv[col][j] *= inv_pivot;
		}

		// eliminate this column from other rows
		for (int r = 0; r < 4; ++r) {
			if (r == col) continue;
			const f32 factor = a[r][col];
			if (factor == 0.0f) continue;
			for (int j = 0; j < 4; ++j) {
				a[r][j]  -= factor * a[col][j];
				inv[r][j] -= factor * inv[col][j];
			}
		}
	}

	// 'inv' is now row-major inverse; convert back to column-major mat4
	mat4 r(0.0);
	r.c0 = vec4{ inv[0][0], inv[1][0], inv[2][0], inv[3][0] };
	r.c1 = vec4{ inv[0][1], inv[1][1], inv[2][1], inv[3][1] };
	r.c2 = vec4{ inv[0][2], inv[1][2], inv[2][2], inv[3][2] };
	r.c3 = vec4{ inv[0][3], inv[1][3], inv[2][3], inv[3][3] };
	return r;
}

//	mat<4, 4, T, Q> LocalMatrix(mat);
//
//	// Normalize the matrix.
//	if(epsilonEqual(LocalMatrix[3][3], static_cast<T>(0), epsilon<T>()))
//		return false;
//
//	for(length_t i = 0; i < 4; ++i)
//		for(length_t j = 0; j < 4; ++j)
//			LocalMatrix[i][j] /= LocalMatrix[3][3];
//
//	// perspectiveMatrix is used to solve for perspective, but it also provides
//	// an easy way to test for singularity of the upper 3x3 component.
//	mat<4, 4, T, Q> PerspectiveMatrix(LocalMatrix);
//
//	for(length_t i = 0; i < 3; i++)
//		PerspectiveMatrix[i][3] = static_cast<T>(0);
//	PerspectiveMatrix[3][3] = static_cast<T>(1);
//
//	/// TODO: Fixme!
//	if(epsilonEqual(determinant(PerspectiveMatrix), static_cast<T>(0), epsilon<T>()))
//		return false;
//
//	// First, isolate perspective.  This is the messiest.
//	if(
//			epsilonNotEqual(LocalMatrix[0][3], static_cast<T>(0), epsilon<T>()) ||
//			epsilonNotEqual(LocalMatrix[1][3], static_cast<T>(0), epsilon<T>()) ||
//			epsilonNotEqual(LocalMatrix[2][3], static_cast<T>(0), epsilon<T>()))
//	{
//		// rightHandSide is the right hand side of the equation.
//		vec<4, T, Q> RightHandSide;
//		RightHandSide[0] = LocalMatrix[0][3];
//		RightHandSide[1] = LocalMatrix[1][3];
//		RightHandSide[2] = LocalMatrix[2][3];
//		RightHandSide[3] = LocalMatrix[3][3];
//
//		// Solve the equation by inverting PerspectiveMatrix and multiplying
//		// rightHandSide by the inverse.  (This is the easiest way, not
//		// necessarily the best.)
//		mat<4, 4, T, Q> InversePerspectiveMatrix = glm::inverse(PerspectiveMatrix);//   inverse(PerspectiveMatrix, inversePerspectiveMatrix);
//		mat<4, 4, T, Q> TransposedInversePerspectiveMatrix = glm::transpose(InversePerspectiveMatrix);//   transposeMatrix4(inversePerspectiveMatrix, transposedInversePerspectiveMatrix);
//
//		Perspective = TransposedInversePerspectiveMatrix * RightHandSide;
//		//  v4MulPointByMatrix(rightHandSide, transposedInversePerspectiveMatrix, perspectivePoint);
//
//		// Clear the perspective partition
//		LocalMatrix[0][3] = LocalMatrix[1][3] = LocalMatrix[2][3] = static_cast<T>(0);
//		LocalMatrix[3][3] = static_cast<T>(1);
//	}
//	else
//	{
//		// No perspective.
//		Perspective = vec<4, T, Q>(0, 0, 0, 1);
//	}
//
//	// Next take care of translation (easy).
//	Translation = vec<3, T, Q>(LocalMatrix[3]);
//	LocalMatrix[3] = vec<4, T, Q>(0, 0, 0, LocalMatrix[3].w);
//
//	vec<3, T, Q> Row[3], Pdum3;
//
//	// Now get scale and shear.
//	for(length_t i = 0; i < 3; ++i)
//		for(length_t j = 0; j < 3; ++j)
//			Row[i][j] = LocalMatrix[i][j];
//
//	// Compute X scale factor and normalize first row.
//	Scale.x = length(Row[0]);// v3Length(Row[0]);
//
//	Row[0] = detail::scale(Row[0], static_cast<T>(1));
//
//	// Compute XY shear factor and make 2nd row orthogonal to 1st.
//	Skew.z = dot(Row[0], Row[1]);
//	Row[1] = detail::combine(Row[1], Row[0], static_cast<T>(1), -Skew.z);
//
//	// Now, compute Y scale and normalize 2nd row.
//	Scale.y = length(Row[1]);
//	Row[1] = detail::scale(Row[1], static_cast<T>(1));
//	Skew.z /= Scale.y;
//
//	// Compute XZ and YZ shears, orthogonalize 3rd row.
//	Skew.y = glm::dot(Row[0], Row[2]);
//	Row[2] = detail::combine(Row[2], Row[0], static_cast<T>(1), -Skew.y);
//	Skew.x = glm::dot(Row[1], Row[2]);
//	Row[2] = detail::combine(Row[2], Row[1], static_cast<T>(1), -Skew.x);
//
//	// Next, get Z scale and normalize 3rd row.
//	Scale.z = length(Row[2]);
//	Row[2] = detail::scale(Row[2], static_cast<T>(1));
//	Skew.y /= Scale.z;
//	Skew.x /= Scale.z;
//
//	// At this point, the matrix (in rows[]) is orthonormal.
//	// Check for a coordinate system flip.  If the determinant
//	// is -1, then negate the matrix and the scaling factors.
//	Pdum3 = cross(Row[1], Row[2]); // v3Cross(row[1], row[2], Pdum3);
//	if(dot(Row[0], Pdum3) < 0)
//	{
//		for(length_t i = 0; i < 3; i++)
//		{
//			Scale[i] *= static_cast<T>(-1);
//			Row[i] *= static_cast<T>(-1);
//		}
//	}
//
//	// Now, get the rotations out, as described in the gem.
//
//	// FIXME - Add the ability to return either quaternions (which are
//	// easier to recompose with) or Euler angles (rx, ry, rz), which
//	// are easier for authors to deal with. The latter will only be useful
//	// when we fix https://bugs.webkit.org/show_bug.cgi?id=23799, so I
//	// will leave the Euler angle code here for now.
//
//	// ret.rotateY = asin(-Row[0][2]);
//	// if (cos(ret.rotateY) != 0) {
//	//     ret.rotateX = atan2(Row[1][2], Row[2][2]);
//	//     ret.rotateZ = atan2(Row[0][1], Row[0][0]);
//	// } else {
//	//     ret.rotateX = atan2(-Row[2][0], Row[1][1]);
//	//     ret.rotateZ = 0;
//	// }
//
//	int i, j, k = 0;
//	T root, trace = Row[0].x + Row[1].y + Row[2].z;
//	if(trace > static_cast<T>(0))
//	{
//		root = sqrt(trace + static_cast<T>(1.0));
//		Orientation.w = static_cast<T>(0.5) * root;
//		root = static_cast<T>(0.5) / root;
//		Orientation.x = root * (Row[1].z - Row[2].y);
//		Orientation.y = root * (Row[2].x - Row[0].z);
//		Orientation.z = root * (Row[0].y - Row[1].x);
//	} // End if > 0
//	else
//	{
//		static int Next[3] = {1, 2, 0};
//		i = 0;
//		if(Row[1].y > Row[0].x) i = 1;
//		if(Row[2].z > Row[i][i]) i = 2;
//		j = Next[i];
//		k = Next[j];
//
//		root = sqrt(Row[i][i] - Row[j][j] - Row[k][k] + static_cast<T>(1.0));
//
//		Orientation[i] = static_cast<T>(0.5) * root;
//		root = static_cast<T>(0.5) / root;
//		Orientation[j] = root * (Row[i][j] + Row[j][i]);
//		Orientation[k] = root * (Row[i][k] + Row[k][i]);
//		Orientation.w = root * (Row[j][k] - Row[k][j]);
//	} // End if <= 0
//
//	return true;
//}

quat identity_quat() {
	return quat{ 1.0f, 0.0f, 0.0f, 0.0f };
}

f32 dot(const quat& a, const quat& b) {
	return a.w * b.w + a.x * b.x + a.y * b.y + a.z * b.z;
}

f32 length(const quat& q) {
	return std::sqrt(dot(q, q));
}

quat normalize(const quat& q) {
	f32 len = length(q);
	if (len == 0.0f) {
		// Return identity if length is zero; tweak if you prefer different behavior
		return identity_quat();
	}
	f32 inv = 1.0f / len;
	return quat{ q.w * inv, q.x * inv, q.y * inv, q.z * inv };
}

quat conjugate(const quat& q) {
	return quat{ q.w, -q.x, -q.y, -q.z };
}

quat inverse(const quat& q) {
	f32 n2 = dot(q, q);
	if (n2 == 0.0f) {
		return identity_quat();
	}
	f32 inv_n2 = 1.0f / n2;
	quat c = conjugate(q);
	return quat{ c.w * inv_n2, c.x * inv_n2, c.y * inv_n2, c.z * inv_n2 };
}

// ----- component-wise + and scalar * (handy for slerp) -----

quat operator+(const quat& a, const quat& b) {
	return quat{ a.w + b.w, a.x + b.x, a.y + b.y, a.z + b.z };
}

quat operator*(const quat& q, f32 s) {
	return quat{ q.w * s, q.x * s, q.y * s, q.z * s };
}

quat operator*(f32 s, const quat& q) {
	return q * s;
}

// ----- Hamilton product (rotation composition) -----

// Convention: result = a * b applies b first, then a (GLM-style)
quat operator*(const quat& a, const quat& b) {
	return quat{ a.w * b.w - a.x * b.x - a.y * b.y - a.z * b.z,
				 a.w * b.x + a.x * b.w + a.y * b.z - a.z * b.y,
				 a.w * b.y - a.x * b.z + a.y * b.w + a.z * b.x,
				 a.w * b.z + a.x * b.y - a.y * b.x + a.z * b.w };
}

// ----- rotate vec3 by quat (q * v * q^-1, optimized form) -----

vec3 rotate(const quat& q, const vec3& v) {
	// Using the "u, s" form: q = (s, u)
	vec3 u{ q.x, q.y, q.z };
	f32 s = q.w;

	f32 dot_uv = u.x * v.x + u.y * v.y + u.z * v.z;      // dot(u, v)
	f32 dot_uu = u.x * u.x + u.y * u.y + u.z * u.z;      // dot(u, u)

	vec3 cross_uv{ u.y * v.z - u.z * v.y,
				   u.z * v.x - u.x * v.z,
				   u.x * v.y - u.y * v.x };

	// 2 * dot(u, v) * u
	vec3 term1{ 2.0f * dot_uv * u.x,
				2.0f * dot_uv * u.y,
				2.0f * dot_uv * u.z };

	// (s^2 - dot(u, u)) * v
	f32 s2_minus_uu = s * s - dot_uu;
	vec3 term2{ s2_minus_uu * v.x,
				s2_minus_uu * v.y,
				s2_minus_uu * v.z };

	// 2 * s * cross(u, v)
	vec3 term3{ 2.0f * s * cross_uv.x,
				2.0f * s * cross_uv.y,
				2.0f * s * cross_uv.z };

	return vec3{ term1.x + term2.x + term3.x,
				 term1.y + term2.y + term3.y,
				 term1.z + term2.z + term3.z };
}

// ----- angle-axis (like glm::angleAxis) -----

quat angle_axis(f32 angle_radians, const vec3& axis) {
	// Normalize axis to be safe
	f32 len = std::sqrt(axis.x * axis.x + axis.y * axis.y + axis.z * axis.z);
	if (len == 0.0f) {
		return identity_quat();
	}
	f32 inv_len = 1.0f / len;
	f32 half = 0.5f * angle_radians;
	f32 s = std::sin(half);
	f32 c = std::cos(half);

	vec3 n{ axis.x * inv_len,
			axis.y * inv_len,
			axis.z * inv_len };

	return quat{ c, n.x * s, n.y * s, n.z * s };
}

// ----- slerp (glm::slerp style) -----

quat slerp(const quat& a, const quat& b, f32 t) {
	// Clamp t just in case
	if (t <= 0.0f) return a;
	if (t >= 1.0f) return b;

	quat q1 = normalize(a);
	quat q2 = normalize(b);

	f32 cosTheta = dot(q1, q2);

	// Use shortest path
	if (cosTheta < 0.0f) {
		q2 = quat{ -q2.w, -q2.x, -q2.y, -q2.z };
		cosTheta = -cosTheta;
	}

	// If very close, fall back to lerp
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

mat3 mat3_cast(quat const& q) {

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

mat4 mat4_cast(quat const& q) {
	return mat4(mat3_cast(q));
}

f32* value_ptr(quat& q) {
	return &q.w;
}

const f32* value_ptr(const quat& q) {
	return &q.w;
}

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

f32	clamp(f32 val, f32 low, f32 high) {
	return std::clamp(val, low, high);
}

void swap(f32& a, f32& b) {
	std::swap(a, b);
}
	