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

    // [Public Types]

    /** @brief Axis-aligned bounding box defined by minimum and maximum corners. */
    struct AABB {

        // [Public Static Member Functions]

        /** @brief Returns a zero-size box at the origin. */
        static AABB       Zero();

        /** @brief Returns an invalid box suitable as an empty accumulation seed. */
        static AABB       Invalid();

        /** @brief Returns the union of @p a and @p b, treating invalid bounds as empty. */
        static AABB       Union(const AABB& a, const AABB& b);

        /** @brief Expands @p a to contain @p p, initializing invalid bounds to @p p. */
        static void       Expand(AABB& a, const math::vec3& p);

        /** @brief Returns the midpoint between @p a.min and @p a.max. */
        static math::vec3 Center(const AABB& a);

        // [Public Member Functions]

        /** @brief Returns whether min is no greater than max on every axis. */
        bool              valid() const;

        /** @brief Expands this box to include @p b. */
        AABB&             operator|=(const AABB& b);

        /** @brief Expands this box to include @p p. */
        AABB&             operator|=(const math::vec3& p);

        // [Public Member Variables]

        math::vec3        min; ///< Minimum corner.
        math::vec3        max; ///< Maximum corner.
    };

    // [Public Non-Member Functions]

    /** @brief Returns @p a expanded to include @p b. */
    AABB operator|(AABB a, const AABB& b);

    /** @brief Returns @p a expanded to include @p p. */
    AABB operator|(AABB a, const math::vec3& p);

}

#endif // AVARA3D_MESH_AABB_H
