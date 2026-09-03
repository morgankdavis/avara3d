//
//  BulletUtilities.cc
//  avara3d
//
//  Created by Morgan Davis on 12/15/23.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#include "a3d/physics/backend/bullet/BulletUtilities.h"

#include <bullet/LinearMath/btQuaternion.h>
#include <bullet/LinearMath/btTransform.h>
#include <bullet/LinearMath/btVector3.h>

using namespace a3d::math;

namespace a3d {

vec3 A3DVec3FromBTVector3(const btVector3& from) {
    return {from.x(), from.y(), from.z()};
}

vec4 A3DVec4FromBTVector4(const btVector4& from) {
    return vec4 {from.x(), from.y(), from.z(), from.w()};
}

mat4 A3DMat4FromBTTransform(const btTransform& from) {
    mat4 glmMat;
    from.getOpenGLMatrix(value_ptr(glmMat));
    return glmMat;
}

btVector3 BTVector3FromA3DVec3(const math::vec3& from) {
    return {from.x, from.y, from.z};
}

btVector4 BTVector4FromA3DVec4(const math::vec4& from) {
    return {from.x, from.y, from.z, from.w};
}

btQuaternion BTQuaternionFromA3DQuat(const math::quat& from) {

    return {from.x, from.y, from.z, from.w};
}

btTransform BTTransformFromA3DMat4(const math::mat4& from) {

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

mat4 TransformByRemovingScale(const mat4& m, bool& scaled) {
    // TODO: optimize

    vec3 scale;
    quat orientation;
    vec3 translation;

    decompose(m, scale, orientation, translation);

    scaled = scale != vec3(1.0f); // a3d::math
    if (scaled) {
        return translate(mat4(1.0), translation) * mat4_cast(orientation) * mat4(1.0);
    }
    else {
        return m;
    }
}

} // namespace a3d
