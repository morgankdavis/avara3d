//
//  Plane.h
//  avara3d
//
//  Created by Morgan Davis on 10/31/17.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_MESH_PRIMITIVE_PLANE_H
#define AVARA3D_MESH_PRIMITIVE_PLANE_H

#include <memory>

#include "a3d/mesh/MeshElement.h"

namespace a3d {

    class Mesh;
    class Material;

    class Plane : public MeshElement {

    public:
        /// Public Static Member Functions ///

        static std::shared_ptr<Mesh> Mesh(float                     width, // x
                                          float                     height, // y
                                          unsigned                  widthSegements = DEFAULT_SEGMENTS,
                                          unsigned                  heightSegments = DEFAULT_SEGMENTS,
                                          std::shared_ptr<Material> material       = nullptr);

        /// Public Lifecycle Functions ///

        Plane(float    width,
              float    height,
              unsigned widthSegements = DEFAULT_SEGMENTS,
              unsigned heightSegments = DEFAULT_SEGMENTS);

        /// Public Member Functions ///

        float    width() const;
        float    height() const;
        unsigned widthSegements() const;
        unsigned heightSegments() const;

    private:
        /// Private Constants ///

        static constexpr unsigned DEFAULT_SEGMENTS = 8;

        /// Private Member Variables ///

        float                     _width;
        float                     _height;
        unsigned                  _widthSegements;
        unsigned                  _heightSegments;
    };

}

#endif /* AVARA3D_MESH_PRIMITIVE_PLANE_H */
