//
//  BulletUtilities.cc
//	avara3d
//
//  Created by Morgan Davis on 12/15/23.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#include "a3d/physics/bullet/BulletUtilities.h"

#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/matrix_decompose.hpp"
#include "LinearMath/btQuaternion.h"
#include "LinearMath/btTransform.h"
#include "LinearMath/btVector3.h"

#include "a3d/Utilities.h"


using namespace a3d;
using namespace a3d::utils;
using namespace glm;


vec3 a3d::GLMVec3FromBTVector3(const btVector3& from) {
	return {from.x(), from.y(), from.z()};
}

vec4 a3d::GLMVec4FromBTVector4(const btVector4& from) {
	return vec4{from.x(), from.y(), from.z(), from.w()};
}

mat4 a3d::GLMMat4FromBTTransform(const btTransform& from) {
	mat4 glmMat;
	from.getOpenGLMatrix(value_ptr(glmMat));
	return glmMat;
}

btVector3 a3d::BTVector3FromGLMVec3(const vec3& from) {
	return {from.x, from.y, from.z};
}

btVector4 a3d::BTVector4FromGLMVec4(const vec4& from) {
	return {from.x, from.y, from.z, from.w};
}

btQuaternion a3d::BTQuaternionFromGLMQuat(const quat& from) {

	return {from.x, from.y, from.z, from.w};

}

btTransform a3d::BTTransformFromGLMMat4(const mat4& from) {

	// this version (probably) does not strip scale & sheer

	btTransform bulletTransform;
	bulletTransform.setIdentity();
	bulletTransform.setFromOpenGLMatrix(value_ptr(from));
	return bulletTransform;


//	// THIS VERSION STRIPS (hopefully!) scale & sheer
//
//	btTransform bulletTransform;
//	bulletTransform.setIdentity();
//
//	vec3 scale;
//	quat orientation;
//	vec3 translation;
//	vec3 skew;
//	vec4 perspective;
//
//	decompose(from,
//			  scale,
//			  orientation,
//			  translation,
//			  skew,
//			  perspective);
//
//	bulletTransform.setOrigin(BTVector3FromGLMVec3(translation));
//	bulletTransform.setRotation(BTQuaternionFromGLMQuat(orientation));
//
//	return bulletTransform;
}

mat4 a3d::TransformByRemovingScale(const mat4& m, bool& scaled) {
	// TODO: optimize

	vec3 scale;
	quat orientation;
	vec3 translation;
	vec3 skew;
	vec4 perspective;

	decompose(m,
			  scale,
			  orientation,
			  translation,
			  skew,
			  perspective);

	scaled = !Equal(scale, {1, 1, 1});
	if (scaled) return translate(mat4(1.0), translation) * mat4_cast(orientation) * mat4(1.0);
	else return m;
}

//btTransform& a3d::BTIdentityTransform() {
//	static auto identityTransform = btTransform();
//	identityTransform.setIdentity();
//	return identityTransform;
//}
