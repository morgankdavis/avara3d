//
//  Spring.h
//  avara3d
//
//  Created by Morgan Davis on 3/6/24.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_MESH_PRIMITIVE_SPRING_H
#define AVARA3D_MESH_PRIMITIVE_SPRING_H

#include <memory>

#include "a3d/mesh/MeshElement.h"

namespace a3d {

class Mesh;
class Material;

    /**
 * @brief Helical spring mesh element aligned along the Z axis.
 *
 * Minor radius is the radius of the spring tube itself; major radius is the
 * distance from the Z axis to the tube centerline. Length is the full axial
 * length. Slices subdivide the tube cross-section and segments subdivide the
 * helical path.
 */
class Spring : public MeshElement {

public:
    // [Public Static Member Functions]

        /** @brief Creates a Mesh containing a Spring and optional @p material. */
    static std::shared_ptr<Mesh> Mesh(float                     minorRadius,
                                      float                     majorRadius,
                                      float                     length,
                                      unsigned                  slices   = DEFAULT_SLICES,
                                      unsigned                  segments = DEFAULT_SEGMENTS,
                                      std::shared_ptr<Material> material = nullptr);

    // [Public Lifecycle Functions]

        /** @brief Generates spring geometry with the supplied dimensions and subdivisions. */
    Spring(float    minorRadius,
           float    majorRadius,
           float    length,
           unsigned slices   = DEFAULT_SLICES,
           unsigned segments = DEFAULT_SEGMENTS);

    // [Public Member Functions]

        /** @brief Returns the configured spring-tube radius. */
    float    minorRadius() const;

        /** @brief Returns the configured radius from the Z axis to the tube centerline. */
    float    majorRadius() const;

        /** @brief Returns the configured axial length. */
    float    length() const;

        /** @brief Returns the tube cross-section subdivision count. */
    unsigned slices() const;

        /** @brief Returns the helical-path subdivision count. */
    unsigned segments() const;

private:
    // [Private Constants]

    static constexpr unsigned DEFAULT_SLICES   = 8;
    static constexpr unsigned DEFAULT_SEGMENTS = 32;

    // [Private Member Variables]

    float                     _minorRadius;
    float                     _majorRadius;
    float                     _length;
    unsigned                  _slices;
    unsigned                  _segments;
};

}

#endif // AVARA3D_MESH_PRIMITIVE_SPRING_H
