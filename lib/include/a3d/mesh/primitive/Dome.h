//
//  Dome.h
//  avara3d
//
//  Created by Morgan Davis on 3/21/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_MESH_PRIMITIVE_DOME_H
#define AVARA3D_MESH_PRIMITIVE_DOME_H

#include <memory>

#include "a3d/mesh/MeshElement.h"

namespace a3d {

    class Mesh;
    class Material;

    class Dome : public MeshElement {

    public:
        /// Pubic Static Members ///

        // NOTE: this is NOT centered.
        static std::shared_ptr<Mesh> Mesh(float                     radius,
                                          float                     azimuthStart,
                                          float                     azimuthSweep,
                                          float                     elevationStart,
                                          float                     elevationSweep,
                                          unsigned                  slices   = DEFAULT_SLICES,
                                          unsigned                  segments = DEFAULT_SEGMENTS,
                                          std::shared_ptr<Material> material = nullptr);

        /// Public Lifecycle Functions ///

        // z, x, y?
        Dome(float    radius,
             float    azimuthStart,
             float    azimuthSweep,
             float    elevationStart,
             float    elevationSweep,
             unsigned slices   = DEFAULT_SLICES,
             unsigned segments = DEFAULT_SEGMENTS);

        /// Public Member Functions ///

        float    radius() const;
        float    azimuthStart() const;
        float    azimuthSweep() const;
        float    elevationStart() const;
        float    elevationSweep() const;
        unsigned slices() const;
        unsigned segments() const;

    private:
        ///  Private Constants ///

        static constexpr int DEFAULT_SLICES   = 32; // around circumference (azimuth / longitude)
        static constexpr int DEFAULT_SEGMENTS = 8; // bottom-to-top (elevation / latitude)

        /// Private Member Variables ///

        float                _radius;
        float                _azimuthStart;
        float                _azimuthSweep;
        float                _elevationStart;
        float                _elevationSweep;
        unsigned             _slices;
        unsigned             _segments;
    };

}

#endif /* AVARA3D_MESH_PRIMITIVE_DOME_H */
