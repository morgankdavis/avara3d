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

    class Wedge : public MeshElement {

    public:
        // [Pubic Static Members]

        // slanted side faces +Y.
        // pointy end faces -X, thick end faces +X.
        static std::shared_ptr<Mesh> Mesh(float                     length, // x
                                          float                     width, // z
                                          float                     height, // y
                                          unsigned                  runSegments   = DEFAULT_RUN_SEGMENTS,
                                          unsigned                  widthSegments = DEFAULT_WIDTH_SEGMENTS,
                                          unsigned                  riseSegments  = DEFAULT_RISE_SEGMENTS,
                                          std::shared_ptr<Material> material      = nullptr);

        // [Public Lifecycle Functions]

        // z, x, y?
        Wedge(float    length,
              float    width,
              float    height,
              unsigned lengthSegments = DEFAULT_RUN_SEGMENTS,
              unsigned widthSegments  = DEFAULT_WIDTH_SEGMENTS,
              unsigned heightSegments = DEFAULT_RISE_SEGMENTS);

        // [Public Member Functions]

        float    length() const;
        float    width() const;
        float    height() const;
        unsigned runSegments() const;
        unsigned widthSegments() const;
        unsigned riseSegments() const;

    private:
        // [ Private Constants]

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
