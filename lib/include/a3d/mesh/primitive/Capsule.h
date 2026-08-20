//
//  Capsule.h
//  avara3d
//
//  Created by Morgan Davis on 11/8/17.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_MESH_PRIMITIVE_CAPSULE_H
#define AVARA3D_MESH_PRIMITIVE_CAPSULE_H

#include <memory>

#include "a3d/mesh/MeshElement.h"

namespace a3d {

    class Mesh;
    class Material;

    /**
     * @brief Capsule mesh element centered at the origin and aligned along the Y axis.
     *
     * The height parameter represents the distance between the centers of the hemispherical
     * caps, so the overall tip-to-tip height is height + 2 * radius. Slices
     * subdivide around the capsule, segments subdivide its straight section, and
     * rings subdivide the caps.
     */
    class Capsule : public MeshElement {

    public:
        // [Public Static Member Functions]

        /** @brief Creates a Mesh containing a Capsule and optional @p material. */
        static std::shared_ptr<Mesh> Mesh(float                     radius,
                                          float                     height,
                                          unsigned                  slices   = DEFAULT_SLICES,
                                          unsigned                  segments = DEFAULT_SEGMENTS,
                                          unsigned                  rings    = DEFAULT_RINGS,
                                          std::shared_ptr<Material> material = nullptr);

        // [Public Lifecycle Functions]

        /** @brief Generates capsule geometry with the supplied dimensions and subdivisions. */
        Capsule(float    radius,
                float    height,
                unsigned slices   = DEFAULT_SLICES,
                unsigned segments = DEFAULT_SEGMENTS,
                unsigned rings    = DEFAULT_RINGS);

        // [Public Member Functions]

        /** @brief Returns the configured capsule radius. */
        float    radius() const;

        /** @brief Returns the configured distance between the centers of the capsule caps. */
        float    height() const;

        /** @brief Returns the circumferential subdivision count. */
        unsigned slices() const;

        /** @brief Returns the straight-section subdivision count. */
        unsigned segments() const;

        /** @brief Returns the cap subdivision count. */
        unsigned rings() const;

    private:
        // [Private Constants]

        static constexpr unsigned DEFAULT_SLICES   = 32;
        static constexpr unsigned DEFAULT_SEGMENTS = 4;
        static constexpr unsigned DEFAULT_RINGS    = 8;

        // [Private Member Variables]

        float                     _radius;
        float                     _height;
        unsigned                  _slices;
        unsigned                  _segments;
        unsigned                  _rings;
    };

}

#endif /* AVARA3D_MESH_PRIMITIVE_CAPSULE_H */
