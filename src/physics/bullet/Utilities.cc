//
// Created by mkd on 12/15/23.
//

#include "ae/physics/bullet/Utilities.h"


#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/matrix_decompose.hpp"

#include "ae/Utilities.h"


using namespace ae;
using namespace ae::utils;
using namespace glm;


vec3 ae::GLMVec3FromBTVector3(const btVector3& from) {
	return vec3(from.x(), from.y(), from.z());
}

vec4 ae::GLMVec4FromBTVector4(const btVector4& from) {
	return vec4(from.x(), from.y(), from.z(), from.w());
}

mat4 ae::GLMMat4FromBTTransform(const btTransform& from) {
	mat4 glmMat;
	from.getOpenGLMatrix(value_ptr(glmMat));
	return glmMat;
}

btVector3 ae::BTVector3FromGLMVec3(const vec3& from) {
	return btVector3(from.x, from.y, from.z);
}

btVector4 ae::BTVector4FromGLMVec4(const vec4& from) {
	return btVector4(from.x, from.y, from.z, from.w);
}

btQuaternion ae::BTQuaternionFromGLMQuat(const quat& from) {

	return btQuaternion(from.x, from.y, from.z, from.w);

}

btTransform ae::BTTransformFromGLMMat4(const mat4& from) {

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

mat4 ae::TransformByRemovingScale(const mat4& m, bool& scaled) {
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

//btTransform& ae::BTIdentityTransform() {
//	static auto identityTransform = btTransform();
//	identityTransform.setIdentity();
//	return identityTransform;
//}
