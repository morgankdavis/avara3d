//
//  Tube.cc
//  avara3d
//
//  Created by Morgan Davis on 11/8/17.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#include "a3d/mesh/primitive/Tube.h"

#include <generator/generator.hpp>

#include "a3d/mesh/Mesh.h"
#include "a3d/mesh/MeshElement.h"
#include "a3d/mesh/PrimitiveTopology.h"
#include "a3d/mesh/VertexFormats.h"
#include "a3d/visual/material/Material.h"

using namespace a3d;
using namespace generator;
using namespace a3d::math;
using namespace std;

/// Pubic Static Members ///

shared_ptr<Mesh> Tube::Mesh(float                      innerRadius,
                            float                      outerRadius,
                            float                      height,
                            unsigned                   slices,
                            unsigned                   segments,
                            unsigned                   rings,
                            const shared_ptr<Material> material) {

    return make_shared<a3d::Mesh>("Tube",
                                  make_unique<Tube>(innerRadius, outerRadius, height, slices, segments, rings),
                                  material);
}

/// Public Lifecycle Functions ///

Tube::Tube(float    innerRadius,
           float    outerRadius,
           float    height,
           unsigned slices,
           unsigned segments,
           unsigned rings):
    MeshElement {},
    _innerRadius {innerRadius},
    _outerRadius {outerRadius},
    _height {height},
    _slices {slices},
    _segments {segments},
    _rings {rings} {

    using namespace generator;

    /// @param radius The outer radius of the cylinder on the xy-plane.
    /// @param innerRadius The inner radius of the cylinder on the xy-plane.
    /// @param size Half of the length of the cylinder along the z-axis.
    /// @param slices Number nubdivisions around the z-axis.
    /// @param segments Number of subdivisions along the z-axis.
    /// @param rings Number radial subdivisions in the cap.
    /// @param start Counterclockwise angle around the z-axis relative to the x-axis.
    /// @param sweep Counterclockwise angle around the z-axis.

    auto tube =
        CappedTubeMesh {outerRadius, innerRadius, height / 2.0, (int) slices, (int) segments, (int) rings};

    beginBuild(VertexLayout::PNT, (uint16_t) sizeof(VertexPNT), PrimitiveTopology::Triangles, IndexFormat::U32);

    for (auto vs = tube.vertices(); !vs.done(); vs.next()) {
        const auto      v = vs.generate();
        const VertexPNT out {{(float) v.position[0], (float) v.position[1], (float) v.position[2]},
                             {(float) v.normal[0], (float) v.normal[1], (float) v.normal[2]},
                             {(float) v.texCoord[0], (float) v.texCoord[1]}};
        appendVertexBytes(&out);
    }

    for (auto ts = tube.triangles(); !ts.done(); ts.next()) {
        const auto t = ts.generate();
        appendTriangle((uint32_t) t.vertices[0], (uint32_t) t.vertices[1], (uint32_t) t.vertices[2]);
    }

    endBuild(true);
}

/// Public Member Functions ///

float Tube::innerRadius() const {
    return _innerRadius;
}

float Tube::outerRadius() const {
    return _outerRadius;
}

float Tube::height() const {
    return _height;
}

unsigned Tube::slices() const {
    return _slices;
}

unsigned Tube::segments() const {
    return _segments;
}

unsigned Tube::rings() const {
    return _rings;
}
