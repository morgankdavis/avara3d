//
//  BulletUtilities.h
//  avara3d
//
//  Created by Morgan Davis on 12/15/23.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_PHYSICS_BACKEND_BULLET_BULLETUTILITIES_H
#define AVARA3D_PHYSICS_BACKEND_BULLET_BULLETUTILITIES_H

#include "a3d/Math.h"

class btQuaternion;
class btTransform;
class btVector3;
class btVector4;

namespace a3d {

math::vec3   A3DVec3FromBTVector3(const btVector3& from);
math::vec4   A3DVec4FromBTVector4(const btVector4& from);
math::mat4   A3DMat4FromBTTransform(const btTransform& from);
btVector3    BTVector3FromA3DVec3(const math::vec3& from);
btVector4    BTVector4FromA3DVec4(const math::vec4& from);
btQuaternion BTQuaternionFromA3DQuat(const math::quat& from);
btTransform  BTTransformFromA3DMat4(const math::mat4& from);
math::mat4   TransformByRemovingScale(const math::mat4& m, bool& scaled);

} // namespace a3d

#endif // AVARA3D_PHYSICS_BACKEND_BULLET_BULLETUTILITIES_H
