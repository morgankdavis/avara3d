//
//  Math.h
//  avara3d
//
//  Created by Morgan Davis on 12/7/25.
//  Copyright © 2025 Morgan K Davis. All rights reserved.
//

#include "a3d/Math.h"

#include <utility>

using namespace a3d;
using namespace a3d::math;

vec2 math::operator*(const mat2& m, const vec2& v) {

	return { m.c0.x * v.x + m.c1.x * v.y,
			 m.c0.y * v.x + m.c1.y * v.y };
}

vec3 math::operator*(const mat3& m, const vec3& v) {

	return { m.c0.x * v.x + m.c1.x * v.y + m.c2.x * v.z,
			 m.c0.y * v.x + m.c1.y * v.y + m.c2.y * v.z,
			 m.c0.z * v.x + m.c1.z * v.y + m.c2.z * v.z };
}

vec4 math::operator*(const mat4& m, const vec4& v) {

	return { m.c0.x * v.x + m.c1.x * v.y + m.c2.x * v.z + m.c3.x * v.w,
			 m.c0.y * v.x + m.c1.y * v.y + m.c2.y * v.z + m.c3.y * v.w,
			 m.c0.z * v.x + m.c1.z * v.y + m.c2.z * v.z + m.c3.z * v.w,
			 m.c0.w * v.x + m.c1.w * v.y + m.c2.w * v.z + m.c3.w * v.w };
}

mat2 math::operator*(const mat2& a, const mat2& b) {

	mat2 r(0.0f);
	r.c0 = a * b.c0;
	r.c1 = a * b.c1;
	return r;
}

mat3 math::operator*(const mat3& a, const mat3& b) {

	mat3 r(0.0f);
	r.c0 = a * b.c0;
	r.c1 = a * b.c1;
	r.c2 = a * b.c2;
	return r;
}

mat4 math::operator*(const mat4& a, const mat4& b) {

	mat4 r(0.0f);
	r.c0 = a * b.c0;
	r.c1 = a * b.c1;
	r.c2 = a * b.c2;
	r.c3 = a * b.c3;
	return r;
}

// mat2 transpose
mat2 math::transpose(const mat2& m) {

	// rows become columns
	// row 0 = (m00, m01) -> c0.x, c1.x
	// row 1 = (m10, m11) -> c0.y, c1.y
	mat2 r(0.0);
	r.c0 = vec2{ m.c0.x, m.c1.x };
	r.c1 = vec2{ m.c0.y, m.c1.y };
	return r;
}

// mat3 transpose
mat3 math::transpose(const mat3& m) {

	mat3 r(0.0);
	// row 0 -> first column
	r.c0 = vec3{ m.c0.x, m.c1.x, m.c2.x };
	// row 1 -> second column
	r.c1 = vec3{ m.c0.y, m.c1.y, m.c2.y };
	// row 2 -> third column
	r.c2 = vec3{ m.c0.z, m.c1.z, m.c2.z };
	return r;
}

// mat4 transpose
mat4 math::transpose(const mat4& m) {

	mat4 r(0.0);
	r.c0 = vec4{ m.c0.x, m.c1.x, m.c2.x, m.c3.x };
	r.c1 = vec4{ m.c0.y, m.c1.y, m.c2.y, m.c3.y };
	r.c2 = vec4{ m.c0.z, m.c1.z, m.c2.z, m.c3.z };
	r.c3 = vec4{ m.c0.w, m.c1.w, m.c2.w, m.c3.w };
	return r;
}

mat4 math::translate(const mat4& m, const vec3& v) {

	mat4 r(m);
	r[3] = m[0] * v[0] + m[1] * v[1] + m[2] * v[2] + m[3];
	return r;
}

mat2 math::inverse(const mat2& m) {

	const f32 a = m.c0.x;
	const f32 c = m.c0.y;
	const f32 b = m.c1.x;
	const f32 d = m.c1.y;

	const f32 det = a * d - b * c;
	// you can decide what to do if det == 0 (assert, return identity, etc.)
	// here I'll just assert:
	assert(det != 0.0f && "mat2 inverse: matrix is singular");

	const f32 invDet = 1.0f / det;

	mat2 r(0.0);
	r.c0 = {  d * invDet, -c * invDet };
	r.c1 = { -b * invDet,  a * invDet };
	return r;
}

mat3 math::inverse(const mat3& m) {

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

mat4 math::inverse(const mat4& m) {

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

mat4 math::rotate(const mat4& m, f32 angle, const vec3& v) {

	const f32 a = angle;
	const f32 c = cos(a);
	const f32 s = sin(a);

	vec3 axis(normalize(v));
	vec3 temp((f32(1) - c) * axis);

	mat4 rot(0.0);
	rot[0][0] = c + temp[0] * axis[0];
	rot[0][1] = temp[0] * axis[1] + s * axis[2];
	rot[0][2] = temp[0] * axis[2] - s * axis[1];
	rot[1][0] = temp[1] * axis[0] - s * axis[2];
	rot[1][1] = c + temp[1] * axis[1];
	rot[1][2] = temp[1] * axis[2] + s * axis[0];
	rot[2][0] = temp[2] * axis[0] + s * axis[1];
	rot[2][1] = temp[2] * axis[1] - s * axis[0];
	rot[2][2] = c + temp[2] * axis[2];

	mat4 r(0.0);
	r[0] = m[0] * rot[0][0] + m[1] * rot[0][1] + m[2] * rot[0][2];
	r[1] = m[0] * rot[1][0] + m[1] * rot[1][1] + m[2] * rot[1][2];
	r[2] = m[0] * rot[2][0] + m[1] * rot[2][1] + m[2] * rot[2][2];
	r[3] = m[3];
	return r;
}

mat4 math::scale(const mat4& m, const vec3& v) {

	mat4 r(0.0);
	r[0] = m[0] * v[0];
	r[1] = m[1] * v[1];
	r[2] = m[2] * v[2];
	r[3] = m[3];
	return r;
}

mat4 math::perspective(f32 fovy, f32 aspect, f32 zNear, f32 zFar) {
	assert(std::abs(aspect - std::numeric_limits<f32>::epsilon()) > static_cast<f32>(0));

	const f32 tanHalfFovy = tan(fovy / static_cast<f32>(2));

	mat4 r(0.0));
	r[0][0] = static_cast<f32>(1) / (aspect * tanHalfFovy);
	r[1][1] = static_cast<f32>(1) / (tanHalfFovy);
	r[2][2] = -(zFar + zNear) / (zFar - zNear);
	r[2][3] = -static_cast<f32>(1);
	r[3][2] = -(static_cast<f32>(2) * zFar * zNear) / (zFar - zNear);
	return r;
}

mat4 math::ortho(f32 left, f32 right, f32 bottom, f32 top, f32 zNear, f32 zFar) {

	mat4 r(1.0);
	r[0][0] = static_cast<f32>(2) / (right - left);
	r[1][1] = static_cast<f32>(2) / (top - bottom);
	r[2][2] = -static_cast<f32>(2) / (zFar - zNear);
	r[3][0] = -(right + left) / (right - left);
	r[3][1] = -(top + bottom) / (top - bottom);
	r[3][2] = -(zFar + zNear) / (zFar - zNear);
	return r;
}

bool math::decompose(const mat3& mat, // ModelMatrix
					 vec3& scale,
					 quat& orientation,
					 vec3& translation,
					 vec3& skew,
					 vec4& perspective) {

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

	return true;
}

mat3 math::mat3_cast(quat const& q) {

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

mat4 math::mat4_cast(quat const& q) {
	return mat4(mat3_cast(q));
}

f32 math::radians(f32 degrees) {
	return degrees * static_cast<f32>(0.01745329251994329576923690768489);
}

f32 math::degrees(f32 radians) {
	return radians * static_cast<f32>(57.295779513082320876798154814105);
}

f32 math::sin(f32 num) {
	return std::sin(num);
}

f32 math::cos(f32 num) {
	return std::cos(num);
}
f32 math::tan(f32 num) {
	return std::tan(num);
}

f32 math::asin(f32 num) {
	return std::asin(num);
}

f32 math::acos(f32 num) {
	return std::acos(num);
}

f32 math::atan(f32 num) {
	return std::atan(num);
}

f32 math::sinh(f32 num) {
	return std::sinh(num);
}

f32 math::cosh(f32 num) {
	return std::cosh(num);
}

f32 math::tanh(f32 num) {
	return std::tanh(num);
}

f32 math::asinh(f32 num) {
	return std::asinh(num);
}

f32 math::acosh(f32 num) {
	return std::acosh(num);
}

f32 math::atanh(f32 num) {
	return std::atanh(num);
}

f32 math::ceil(f32 num) {
	return std::ceil(num);
}

f32 math::floor(f32 num) {
	return std::floor(num);
}

f32 math::round(f32 num) {
	return std::round(num);
}

f32 math::exp(f32 num) {
	return std::exp(num);
}

f32 math::pow(f32 x, f32 y) {
	return std::pow(x, y);
}

f32 math::abs(f32 num) {
	return std::abs(num);
}

f32 math::log(f32 num) {
	return std::log(num);
}

f32 math::log10(f32 num) {
	return std::log10(num);
}

f32 math::sqrt(f32 num) {
	return std::sqrt(num);
}

f32 math::min(f32 a, f32 b) {
	return std::min(a, b);
}

f32 math::max(f32 a, f32 b) {
	return std::max(a, b);
}

f32	math::clamp(f32 val, f32 low, f32 high) {
	return std::clamp(val, low, high);
}

void math::swap(f32& a, f32& b) {
	std::swap(a, b);
}
