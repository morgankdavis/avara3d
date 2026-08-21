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

    /**
     * @brief Generates a bounded angular patch of a sphere centered at the origin.
     *
     * Azimuth angles are measured counterclockwise around the Z axis from +X;
     * elevation angles are measured from the +Z axis. All angles are in radians.
     * The resulting patch remains positioned on the source sphere and is not
     * recentered around the patch's own bounds.
     */
    class Dome : public MeshElement {

    public:
        // [Public Static Member Functions]

        // NOTE: this is NOT centered.
        /** @brief Creates a Mesh containing a Dome and optional @p material. */
        static std::shared_ptr<Mesh> Mesh(float                     radius,
                                          float                     azimuthStart,
                                          float                     azimuthSweep,
                                          float                     elevationStart,
                                          float                     elevationSweep,
                                          unsigned                  slices   = DEFAULT_SLICES,
                                          unsigned                  segments = DEFAULT_SEGMENTS,
                                          std::shared_ptr<Material> material = nullptr);

        // [Public Lifecycle Functions]

        // z, x, y?
        /** @brief Generates a spherical patch with the supplied angular ranges and subdivisions. */
        Dome(float    radius,
             float    azimuthStart,
             float    azimuthSweep,
             float    elevationStart,
             float    elevationSweep,
             unsigned slices   = DEFAULT_SLICES,
             unsigned segments = DEFAULT_SEGMENTS);

        // [Public Member Functions]

        /** @brief Returns the source sphere radius. */
        float    radius() const;

        /** @brief Returns the azimuth start angle in radians. */
        float    azimuthStart() const;

        /** @brief Returns the azimuth sweep angle in radians. */
        float    azimuthSweep() const;

        /** @brief Returns the elevation start angle in radians. */
        float    elevationStart() const;

        /** @brief Returns the elevation sweep angle in radians. */
        float    elevationSweep() const;

        /** @brief Returns the azimuth subdivision count. */
        unsigned slices() const;

        /** @brief Returns the elevation subdivision count. */
        unsigned segments() const;

    private:
        // [Private Constants]

        static constexpr int DEFAULT_SLICES   = 32; // around circumference (azimuth / longitude)
        static constexpr int DEFAULT_SEGMENTS = 8; // bottom-to-top (elevation / latitude)

        // [Private Member Variables]

        float                _radius;
        float                _azimuthStart;
        float                _azimuthSweep;
        float                _elevationStart;
        float                _elevationSweep;
        unsigned             _slices;
        unsigned             _segments;
    };

}

#endif // AVARA3D_MESH_PRIMITIVE_DOME_H
