//
//  AABB.h
//  avara3d
//
//  Created by Morgan Davis on 1/6/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_MESH_AABB_H
#define AVARA3D_MESH_AABB_H

#include "a3d/Math.h"

namespace a3d {

    struct AABB {

        static AABB       Zero();
        static AABB       Invalid();
        static AABB       Union(const AABB& a, const AABB& b);
        static void       Expand(AABB& a, const math::vec3& p);
        static math::vec3 Center(const AABB& a);

        bool              valid() const;

        AABB&             operator|=(const AABB& b); // union
        AABB&             operator|=(const math::vec3& p); // expand

        math::vec3        min;
        math::vec3        max;
    };

    AABB operator|(AABB a, const AABB& b);
    AABB operator|(AABB a, const math::vec3& p);

}

#endif // AVARA3D_MESH_AABB_H
