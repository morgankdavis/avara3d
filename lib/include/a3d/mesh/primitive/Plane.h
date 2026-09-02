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

/**
 * @brief Rectangular grid mesh element centered at the origin in the XY plane.
 *
 * Width spans X, height spans Y, and generated normals point along +Z.
 */
class Plane : public MeshElement {

public:
    // [Public Static Member Functions]

    /** @brief Creates a Mesh containing a Plane and optional @p material. */
    static std::shared_ptr<Mesh> Mesh(float                     width, // x
                                      float                     height, // y
                                      unsigned                  widthSegements = DEFAULT_SEGMENTS,
                                      unsigned                  heightSegments = DEFAULT_SEGMENTS,
                                      std::shared_ptr<Material> material       = nullptr);

    // [Public Lifecycle Functions]

    /** @brief Generates plane geometry with the supplied dimensions and subdivision counts. */
    Plane(float    width,
          float    height,
          unsigned widthSegements = DEFAULT_SEGMENTS,
          unsigned heightSegments = DEFAULT_SEGMENTS);

    // [Public Member Functions]

    /** @brief Returns the configured plane width along X. */
    float    width() const;

    /** @brief Returns the configured plane height along Y. */
    float    height() const;

    /** @brief Returns the width subdivision count. */
    unsigned widthSegements() const;

    /** @brief Returns the height subdivision count. */
    unsigned heightSegments() const;

private:
    // [Private Constants]

    static constexpr unsigned DEFAULT_SEGMENTS = 8;

    // [Private Member Variables]

    float                     _width;
    float                     _height;
    unsigned                  _widthSegements;
    unsigned                  _heightSegments;
};

} // namespace a3d

#endif // AVARA3D_MESH_PRIMITIVE_PLANE_H
