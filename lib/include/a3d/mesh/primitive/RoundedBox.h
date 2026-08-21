//
//  RoundedBox.h
//  avara3d
//
//  Created by Morgan Davis on 3/6/24.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_MESH_PRIMITIVE_ROUNDEDBOX_H
#define AVARA3D_MESH_PRIMITIVE_ROUNDEDBOX_H

#include <memory>

#include "a3d/mesh/MeshElement.h"

namespace a3d {

    class Mesh;
    class Material;

    /**
     * @brief Rounded box mesh element centered at the origin.
     *
     * Width spans X, height spans Y, and length spans Z. Radius controls the
     * rounded edges, slices subdivide the rounded portions, and the segment counts
     * subdivide the corresponding flat-face dimensions.
     */
    class RoundedBox : public MeshElement {

    public:
        // [Public Static Member Functions]

        /** @brief Creates a Mesh containing a RoundedBox and optional @p material. */
        static std::shared_ptr<Mesh> Mesh(float                     radius,
                                          float                     length,
                                          float                     width,
                                          float                     height,
                                          unsigned                  slices         = DEFAULT_SLICES,
                                          unsigned                  lengthSegments = DEFAULT_SEGMENTS,
                                          unsigned                  widthSegments  = DEFAULT_SEGMENTS,
                                          unsigned                  heightSegments = DEFAULT_SEGMENTS,
                                          std::shared_ptr<Material> material       = nullptr);

        // [Public Lifecycle Functions]

        /** @brief Generates rounded-box geometry with the supplied dimensions and subdivisions. */
        RoundedBox(float    radius,
                   float    length,
                   float    width,
                   float    height,
                   unsigned slices         = DEFAULT_SLICES,
                   unsigned lengthSegments = DEFAULT_SEGMENTS,
                   unsigned widthSegments  = DEFAULT_SEGMENTS,
                   unsigned heightSegments = DEFAULT_SEGMENTS);

        // [Public Member Functions]

        /** @brief Returns the configured edge radius. */
        float    radius() const;

        /** @brief Returns the configured box length along Z. */
        float    length() const;

        /** @brief Returns the configured box width along X. */
        float    width() const;

        /** @brief Returns the configured box height along Y. */
        float    height() const;

        /** @brief Returns the rounded-edge subdivision count. */
        unsigned slices() const;

        /** @brief Returns the length subdivision count. */
        unsigned lengthSegments() const;

        /** @brief Returns the width subdivision count. */
        unsigned widthSegments() const;

        /** @brief Returns the height subdivision count. */
        unsigned heightSegments() const;

    private:
        // [Private Constants]

        static constexpr unsigned DEFAULT_SLICES   = 8;
        static constexpr unsigned DEFAULT_SEGMENTS = 8;

        // [Private Member Variables]

        float                     _radius;
        float                     _length;
        float                     _width;
        float                     _height;
        unsigned                  _slices;
        unsigned                  _lengthSegments;
        unsigned                  _widthSegments;
        unsigned                  _heightSegments;
    };

}

#endif // AVARA3D_MESH_PRIMITIVE_ROUNDEDBOX_H
