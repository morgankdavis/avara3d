//
//  BulletShapeProxy.h
//  avara3d
//
//  Created by Morgan Davis on 10/29/23.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_PHYSICS_BACKEND_BULLET_BULLETSHAPEPROXY_H
#define AVARA3D_PHYSICS_BACKEND_BULLET_BULLETSHAPEPROXY_H

#include <memory>
#include <vector>

#include "a3d/physics/proxy/PhysicsShapeProxy.h"

class btCollisionShape;
class btIndexedMesh;
class btRigidBody;
class btTriangleIndexVertexArray;

namespace a3d {
class BulletShapeProxy : public PhysicsShapeProxy {

public:
    // [Internal Lifecycle Functions]

    explicit BulletShapeProxy(PhysicsShape& shape);
    ~BulletShapeProxy() override;

    // [Internal Member Functions]

    float                                                 margin() const override;
    void                                                  margin(float margin) override;

    const std::vector<std::unique_ptr<btCollisionShape>>& btShapes();

private:
    // [Private Member Variables]

    std::vector<std::unique_ptr<btTriangleIndexVertexArray>> _btIndexVertexArrays;
    std::vector<std::unique_ptr<btCollisionShape>>           _btShapes;
};
} // namespace a3d

#endif // AVARA3D_PHYSICS_BACKEND_BULLET_BULLETSHAPEPROXY_H
