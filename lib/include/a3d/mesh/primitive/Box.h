//
//  Box.h
//  avara3d
//
//  Created by Morgan Davis on 10/31/17.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_MESH_PRIMITIVE_BOX_H
#define AVARA3D_MESH_PRIMITIVE_BOX_H

#include <memory>

#include "a3d/mesh/MeshElement.h"

namespace a3d {

    class Mesh;
    class Material;

    /**
     * @brief Box mesh element centered at the origin.
     *
     * Width spans the X axis, height spans Y, and length spans Z.
     */
    class Box : public MeshElement {

    public:
        // [Public Static Member Functions]

        /**
         * @brief Creates a Mesh containing a Box and optional @p material.
         *
         * @param width full size along X.
         * @param height full size along Y.
         * @param length full size along Z.
         * @param widthSegments subdivisions along X.
         * @param heightSegments subdivisions along Y.
         * @param lengthSegments subdivisions along Z.
         */
        static std::shared_ptr<Mesh> Mesh(float                     width,
                                          float                     height,
                                          float                     length,
                                          unsigned                  widthSegments  = DEFAULT_SEGMENTS,
                                          unsigned                  heightSegments = DEFAULT_SEGMENTS,
                                          unsigned                  lengthSegments = DEFAULT_SEGMENTS,
                                          std::shared_ptr<Material> material       = nullptr);

        // [Public Lifecycle Functions]

        /** @brief Generates box geometry with the supplied dimensions and subdivision counts. */
        Box(float    width,
            float    height,
            float    length,
            unsigned widthSegments  = DEFAULT_SEGMENTS,
            unsigned heightSegments = DEFAULT_SEGMENTS,
            unsigned lengthSegments = DEFAULT_SEGMENTS);

        // [Public Member Functions]

        /** @brief Returns the configured box length along Z. */
        float    length() const;

        /** @brief Returns the configured box width along X. */
        float    width() const;

        /** @brief Returns the configured box height along Y. */
        float    height() const;

        /** @brief Returns the stored length subdivision count. */
        unsigned lengthSegments() const;

        /** @brief Returns the stored width subdivision count. */
        unsigned widthSegments() const;

        /** @brief Returns the stored height subdivision count. */
        unsigned heightSegments() const;

    private:
        // [Private Constants]

        static constexpr int DEFAULT_SEGMENTS = 1;

        // [Private Member Variables]

        float                _length;
        float                _width;
        float                _height;
        unsigned             _lengthSegments;
        unsigned             _widthSegments;
        unsigned             _heightSegments;
    };

}

#endif /* AVARA3D_MESH_PRIMITIVE_BOX_H */
