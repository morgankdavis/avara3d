//
//  Torus.h
//  avara3d
//
//  Created by Morgan Davis on 11/8/17.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_MESH_PRIMITIVE_TORUS_H
#define AVARA3D_MESH_PRIMITIVE_TORUS_H

#include <memory>

#include "a3d/mesh/MeshElement.h"

namespace a3d {

    class Mesh;
    class Material;

    /**
     * @brief Torus mesh element centered at the origin in the XY plane.
     *
     * Slices subdivide the minor ring and segments subdivide the major ring.
     */
    class Torus : public MeshElement {

    public:
        // [Public Static Member Functions]

        /** @brief Creates a Mesh containing a Torus and optional @p material. */
        static std::shared_ptr<Mesh> Mesh(float                     minorRadius,
                                          float                     majorRadius,
                                          unsigned                  slices   = DEFAULT_SLICES,
                                          unsigned                  segments = DEFAULT_SEGMENTS,
                                          std::shared_ptr<Material> material = nullptr);

        // [Public Lifecycle Functions]

        /** @brief Generates torus geometry with the supplied radii and subdivision counts. */
        Torus(float    minorRadius,
              float    majorRadius,
              unsigned slices   = DEFAULT_SLICES,
              unsigned segments = DEFAULT_SEGMENTS);

        // [Public Member Functions]

        /** @brief Returns the configured minor-radius parameter. */
        float    minorRadius() const;

        /** @brief Returns the configured major-radius parameter. */
        float    majorRadius() const;

        /** @brief Returns the minor-ring subdivision count. */
        unsigned slices() const;

        /** @brief Returns the major-ring subdivision count. */
        unsigned segments() const;

    private:
        // [Private Constants]

        static constexpr unsigned DEFAULT_SLICES   = 16;
        static constexpr unsigned DEFAULT_SEGMENTS = 32;

        // [Private Member Variables]

        float                     _minorRadius;
        float                     _majorRadius;
        unsigned                  _slices;
        unsigned                  _segments;
    };

}

#endif /* AVARA3D_MESH_PRIMITIVE_TORUS_H */
