//
//  BulletUtilities.h
//  avara3d
//
//  Created by Morgan Davis on 12/15/23.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_BULLETUTILITIES_H
#define AVARA3D_BULLETUTILITIES_H

#include "glm/glm.hpp"

class btQuaternion;
class btTransform;
class btVector3;
class btVector4;

namespace a3d {

	glm::vec3		GLMVec3FromBTVector3(const btVector3& from);
	glm::vec4		GLMVec4FromBTVector4(const btVector4& from);
	glm::mat4		GLMMat4FromBTTransform(const btTransform& from);
	btVector3		BTVector3FromGLMVec3(const glm::vec3& from);
	btVector4		BTVector4FromGLMVec4(const glm::vec4& from);
	btQuaternion	BTQuaternionFromGLMQuat(const glm::quat& from);
	btTransform		BTTransformFromGLMMat4(const glm::mat4& from);
	glm::mat4		TransformByRemovingScale(const glm::mat4& m, bool& scaled);
//	btTransform&	BTIdentityTransform();
}

#endif //AVARA3D_BULLETUTILITIES_H
