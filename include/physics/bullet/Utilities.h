//
// Created by mkd on 12/15/23.
//

#ifndef AVARA_ENGINE_BULLET_UTILITIES_H
#define AVARA_ENGINE_BULLET_UTILITIES_H


#include "glm/glm.hpp"
#include "LinearMath/btQuaternion.h"
#include "LinearMath/btTransform.h"
#include "LinearMath/btVector3.h"


namespace ae {

	glm::vec3		GLMVec3FromBTVector3(const btVector3& from);
	glm::vec4		GLMVec4FromBTVector4(const btVector4& from);
	glm::mat4		GLMMat4FromBTTransform(const btTransform& from);
	btVector3		BTVector3FromGLMVec3(const glm::vec3& from);
	btVector4		BTVector4FromGLMVec4(const glm::vec4& from);
	btQuaternion	BTQuaternionFromGLMQuat(const glm::quat& from);
	btTransform		BTTransformFromGLMMat4(const glm::mat4& from);
	glm::mat4		TransformByRemovingScale(const glm::mat4& m, bool& scaled);
	btTransform&	BTIdentityTransform();
}


#endif //AVARA_ENGINE_BULLET_UTILITIES_H
