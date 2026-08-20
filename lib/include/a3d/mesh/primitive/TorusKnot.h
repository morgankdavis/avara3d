//
//  TorusKnot.h
//  avara3d
//
//  Created by Morgan Davis on 3/6/24.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_MESH_PRIMITIVE_TORUSKNOT_H
#define AVARA3D_MESH_PRIMITIVE_TORUSKNOT_H

#include <memory>

#include "a3d/mesh/MeshElement.h"

namespace a3d {

    class Mesh;
    class Material;

    /**
     * @brief Tubular torus-knot mesh element controlled by the knot parameters p and q.
     *
     * Slices subdivide the tube cross-section and segments subdivide the knot path.
     */
    class TorusKnot : public MeshElement {

    public:
        // [Public Static Member Functions]

        /** @brief Creates a Mesh containing a TorusKnot and optional @p material. */
        static std::shared_ptr<Mesh> Mesh(unsigned                  p,
                                          unsigned                  q,
                                          unsigned                  slices   = DEFAULT_SLICES,
                                          unsigned                  segments = DEFAULT_SEGMENTS,
                                          std::shared_ptr<Material> material = nullptr);

        // [Public Lifecycle Functions]

        /** @brief Generates torus-knot geometry with the supplied knot parameters and subdivisions. */
        TorusKnot(unsigned p,
                  unsigned q,
                  unsigned slices   = DEFAULT_SLICES,
                  unsigned segments = DEFAULT_SEGMENTS);

        // [Public Member Functions]

        /** @brief Returns the configured p knot parameter. */
        unsigned p() const;

        /** @brief Returns the configured q knot parameter. */
        unsigned q() const;

        /** @brief Returns the tube cross-section subdivision count. */
        unsigned slices() const;

        /** @brief Returns the knot-path subdivision count. */
        unsigned segments() const;

    private:
        // [Private Constants]

        static constexpr unsigned DEFAULT_SLICES   = 8;
        static constexpr unsigned DEFAULT_SEGMENTS = 96;

        // [Private Member Variables]

        unsigned                  _p;
        unsigned                  _q;
        unsigned                  _slices;
        unsigned                  _segments;
    };

}

#endif //AVARA3D_MESH_PRIMITIVE_TORUSKNOT_H
