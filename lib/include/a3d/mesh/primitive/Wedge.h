//
//  Wedge.h
//  avara3d
//
//  Created by Morgan Davis on 3/21/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_MESH_PRIMITIVE_WEDGE_H
#define AVARA3D_MESH_PRIMITIVE_WEDGE_H

#include <memory>

#include "a3d/mesh/MeshElement.h"

namespace a3d {

    class Mesh;
    class Material;

    /**
     * @brief Wedge mesh element extending along X, Y, and Z.
     *
     * Length spans X from the point at -X to the tall face at +X, width spans Z,
     * and height rises along +Y. The base lies at Y = 0, so the geometry is not
     * vertically centered about the origin. Subdivision counts are clamped to at
     * least one.
     */
    class Wedge : public MeshElement {

    public:
        // [Public Static Member Functions]

        // slanted side faces +Y.
        // pointy end faces -X, thick end faces +X.
        /** @brief Creates a Mesh containing a Wedge and optional @p material. */
        static std::shared_ptr<Mesh> Mesh(float                     length, // x
                                          float                     width, // z
                                          float                     height, // y
                                          unsigned                  runSegments   = DEFAULT_RUN_SEGMENTS,
                                          unsigned                  widthSegments = DEFAULT_WIDTH_SEGMENTS,
                                          unsigned                  riseSegments  = DEFAULT_RISE_SEGMENTS,
                                          std::shared_ptr<Material> material      = nullptr);

        // [Public Lifecycle Functions]

        // z, x, y?
        /** @brief Generates wedge geometry with the supplied dimensions and subdivision counts. */
        Wedge(float    length,
              float    width,
              float    height,
              unsigned runSegments   = DEFAULT_RUN_SEGMENTS,
              unsigned widthSegments = DEFAULT_WIDTH_SEGMENTS,
              unsigned riseSegments  = DEFAULT_RISE_SEGMENTS);

        // [Public Member Functions]

        /** @brief Returns the configured wedge length along X. */
        float    length() const;

        /** @brief Returns the configured wedge width along Z. */
        float    width() const;

        /** @brief Returns the configured wedge height along Y. */
        float    height() const;

        /** @brief Returns the subdivision count along the wedge run in X. */
        unsigned runSegments() const;

        /** @brief Returns the subdivision count across the wedge width in Z. */
        unsigned widthSegments() const;

        /** @brief Returns the subdivision count along the vertical rise in Y. */
        unsigned riseSegments() const;

    private:
        // [Private Constants]

        // this is kind of jacked up.
        static constexpr int DEFAULT_RUN_SEGMENTS   = 1; // thin end (-X) toward the thick/tall end (+X)
        static constexpr int DEFAULT_WIDTH_SEGMENTS = 1; // across the wedge from one side to the other along Z
        static constexpr int DEFAULT_RISE_SEGMENTS  = 1; // upward along the tall vertical face in +Y

        // [Private Member Variables]

        float                _length;
        float                _width;
        float                _height;
        unsigned             _runSegments;
        unsigned             _widthSegments;
        unsigned             _riseSegments;
    };

}

#endif /* AVARA3D_MESH_PRIMITIVE_WEDGE_H */
