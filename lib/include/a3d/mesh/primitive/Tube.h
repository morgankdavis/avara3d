//
//  Tube.h
//  avara3d
//
//  Created by Morgan Davis on 11/8/17.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_MESH_PRIMITIVE_TUBE_H
#define AVARA3D_MESH_PRIMITIVE_TUBE_H

#include <memory>

#include "a3d/mesh/MeshElement.h"

namespace a3d {

    class Mesh;
    class Material;

    /**
     * @brief Capped hollow tube mesh element centered at the origin and aligned along the Z axis.
     *
     * Inner and outer radii lie in the XY plane and height is the full axial
     * length. Slices subdivide around the tube, segments subdivide along its axis,
     * and rings subdivide the annular end caps radially.
     */
    class Tube : public MeshElement {

    public:
        // [Public Static Member Functions]

        /** @brief Creates a Mesh containing a Tube and optional @p material. */
        static std::shared_ptr<Mesh> Mesh(float                     innerRadius,
                                          float                     outerRadius,
                                          float                     height,
                                          unsigned                  slices   = DEFAULT_SLICES,
                                          unsigned                  segments = DEFAULT_SEGMENTS,
                                          unsigned                  rings    = DEFAULT_RINGS,
                                          std::shared_ptr<Material> material = nullptr);

        // [Public Lifecycle Functions]

        /** @brief Generates capped tube geometry with the supplied dimensions and subdivisions. */
        Tube(float    innerRadius,
             float    outerRadius,
             float    height,
             unsigned slices   = DEFAULT_SLICES,
             unsigned segments = DEFAULT_SEGMENTS,
             unsigned rings    = DEFAULT_RINGS);

        // [Public Member Functions]

        /** @brief Returns the configured inner radius. */
        float    innerRadius() const;

        /** @brief Returns the configured outer radius. */
        float    outerRadius() const;

        /** @brief Returns the configured axial height. */
        float    height() const;

        /** @brief Returns the circumferential subdivision count. */
        unsigned slices() const;

        /** @brief Returns the axial subdivision count. */
        unsigned segments() const;

        /** @brief Returns the radial end-cap subdivision count. */
        unsigned rings() const;

    private:
        // [Private Constants]

        static constexpr unsigned DEFAULT_SLICES   = 32;
        static constexpr unsigned DEFAULT_SEGMENTS = 8;
        static constexpr unsigned DEFAULT_RINGS    = 1;

        // [Private Member Variables]

        float                     _innerRadius;
        float                     _outerRadius;
        float                     _height;
        unsigned                  _slices;
        unsigned                  _segments;
        unsigned                  _rings;
    };

}

#endif /* AVARA3D_MESH_PRIMITIVE_TUBE_H */
