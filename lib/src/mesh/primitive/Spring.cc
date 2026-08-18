//
//  Spring.cc
//  avara3d
//
//  Created by Morgan Davis on 3/6/24.
//  Copyright © 2024-2026 Morgan K Davis. All rights reserved.
//

#include "a3d/mesh/primitive/Spring.h"

#include <generator/generator.hpp>

#include "a3d/mesh/Mesh.h"
#include "a3d/mesh/MeshElement.h"
#include "a3d/mesh/PrimitiveTopology.h"
#include "a3d/mesh/VertexFormats.h"
#include "a3d/visual/material/Material.h"

using namespace a3d;
using namespace a3d::math;
using namespace std;

/// Pubic Static Members ///

shared_ptr<Mesh> Spring::Mesh(float                      minorRadius,
                              float                      majorRadius,
                              float                      length,
                              unsigned                   slices,
                              unsigned                   segments,
                              const shared_ptr<Material> material) {

    return make_shared<a3d::Mesh>("Spring",
                                  make_unique<Spring>(minorRadius, majorRadius, length, slices, segments),
                                  material);
}

/// Public Lifecycle Functions ///

Spring::Spring(float minorRadius, float majorRadius, float length, unsigned slices, unsigned segments):
    MeshElement {},
    _minorRadius {minorRadius},
    _majorRadius {majorRadius},
    _length {length},
    _slices {slices},
    _segments {segments} {

    using namespace generator;

    /// @param minor Radius of the spring it self.
    /// @param major Radius from the z-axis
    /// @param size Half of the length along the z-axis.
    /// @param slices Subdivisions around the spring.
    /// @param segments Subdivisions along the path.
    /// @param majorStart Counterclockwise angle around the z-axis relative to the x-axis.
    /// @param majorSweep Counterclockwise angle arounf the z-axis.

    auto spring = SpringMesh {minorRadius, majorRadius, length / 2.0, (int) slices, (int) segments};

    beginBuild(VertexLayout::PNT, (uint16_t) sizeof(VertexPNT), PrimitiveTopology::Triangles, IndexFormat::U32);

    for (auto vs = spring.vertices(); !vs.done(); vs.next()) {
        const auto      v = vs.generate();
        const VertexPNT out {{(float) v.position[0], (float) v.position[1], (float) v.position[2]},
                             {(float) v.normal[0], (float) v.normal[1], (float) v.normal[2]},
                             {(float) v.texCoord[0], (float) v.texCoord[1]}};
        appendVertexBytes(&out);
    }

    for (auto ts = spring.triangles(); !ts.done(); ts.next()) {
        const auto t = ts.generate();
        appendTriangle((uint32_t) t.vertices[0], (uint32_t) t.vertices[1], (uint32_t) t.vertices[2]);
    }

    endBuild(true);
}

/// Public Member Functions ///

float Spring::minorRadius() const {
    return _minorRadius;
}

float Spring::majorRadius() const {
    return _majorRadius;
}

float Spring::length() const {
    return _length;
}

unsigned Spring::slices() const {
    return _slices;
}

unsigned Spring::segments() const {
    return _segments;
}
