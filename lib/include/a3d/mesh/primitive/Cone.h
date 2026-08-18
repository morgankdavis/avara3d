//
//  Cone.h
//  avara3d
//
//  Created by Morgan Davis on 11/8/17.
//  Copyright © 2017-2026 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_MESH_PRIMITIVE_CONE_H
#define AVARA3D_MESH_PRIMITIVE_CONE_H

#include <memory>

#include "a3d/mesh/MeshElement.h"

namespace a3d {

    class Mesh;
    class Material;

    class Cone : public MeshElement {

    public:
        /// Public Static Member Functions ///

        static std::shared_ptr<Mesh> Mesh(float                     radius,
                                          float                     height,
                                          unsigned                  slices   = DEFAULT_SLICES,
                                          unsigned                  segments = DEFAULT_SEGMENTS,
                                          unsigned                  rings    = DEFAULT_RINGS,
                                          std::shared_ptr<Material> material = nullptr);

        /// Public Lifecycle Functions ///

        Cone(float    radius,
             float    height,
             unsigned slices   = DEFAULT_SLICES,
             unsigned segments = DEFAULT_SEGMENTS,
             unsigned rings    = DEFAULT_RINGS);

        /// Public Member Functions ///

        float    radius() const;
        float    height() const;
        unsigned slices() const;
        unsigned segments() const;
        unsigned rings() const;

    private:
        /// Private Constants ///

        static constexpr unsigned DEFAULT_SLICES   = 32;
        static constexpr unsigned DEFAULT_SEGMENTS = 8;
        static constexpr unsigned DEFAULT_RINGS    = 4;

        /// Private Member Variables ///

        float                     _radius;
        float                     _height;
        unsigned                  _slices;
        unsigned                  _segments;
        unsigned                  _rings;
    };

}

#endif /* AVARA3D_MESH_PRIMITIVE_CONE_H */
