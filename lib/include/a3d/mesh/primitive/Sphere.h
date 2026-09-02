//
//  Sphere.h
//  avara3d
//
//  Created by Morgan Davis on 11/6/17.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_MESH_PRIMITIVE_SPHERE_H
#define AVARA3D_MESH_PRIMITIVE_SPHERE_H

#include <memory>

#include "a3d/mesh/MeshElement.h"

namespace a3d {

class Mesh;
class Material;

/**
 * @brief Icosphere mesh element centered at the origin.
 *
 * Segments controls the number of subdivisions applied to each icosahedron edge.
 */
class Sphere : public MeshElement {

public:
    // [Public Static Member Functions]

    /** @brief Creates a Mesh containing a Sphere and optional @p material. */
    static std::shared_ptr<Mesh> Mesh(float                     radius,
                                      unsigned                  segments = DEFAULT_SEGMENTS,
                                      std::shared_ptr<Material> material = nullptr);

    // [Public Lifecycle Functions]

    /** @brief Generates icosphere geometry with @p radius and @p segments subdivisions per edge. */
    explicit Sphere(float radius, unsigned segments = DEFAULT_SEGMENTS);

    // [Public Member Functions]

    /** @brief Returns the configured sphere radius. */
    float    radius() const;

    /** @brief Returns the number of subdivisions per icosahedron edge. */
    unsigned segments() const;

private:
    // [Private Constants]

    static constexpr unsigned DEFAULT_SEGMENTS = 4;

    // [Private Member Variables]

    float                     _radius;
    unsigned                  _segments;
};

}

#endif // AVARA3D_MESH_PRIMITIVE_SPHERE_H
