//
//  Dome.cc
//  avara3d
//
//  Created by Morgan Davis on 3/21/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#include "a3d/mesh/primitive/Dome.h"

#include <generator/generator.hpp>

#include "a3d/mesh/Mesh.h"
#include "a3d/mesh/MeshElement.h"
#include "a3d/mesh/PrimitiveTopology.h"
#include "a3d/mesh/VertexFormats.h"
#include "a3d/visual/material/Material.h"

using namespace a3d::math;
using namespace std;

namespace a3d {
// [Pubic Static Members]

shared_ptr<Mesh> Dome::Mesh(float                      radius,
                            float                      azimuthStart,
                            float                      azimuthSweep,
                            float                      elevationStart,
                            float                      elevationSweep,
                            unsigned                   slices,
                            unsigned                   segments,
                            const shared_ptr<Material> material) {

    return make_shared<a3d::Mesh>("Dome",
                                  make_unique<Dome>(radius, azimuthStart, azimuthSweep, elevationStart,
                                                    elevationSweep, slices, segments),
                                  material);
}

// [Public Lifecycle Functions]

Dome::Dome(float    radius,
           float    azimuthStart,
           float    azimuthSweep,
           float    elevationStart,
           float    elevationSweep,
           unsigned slices,
           unsigned segments):
    MeshElement {},
    _radius {radius},
    _azimuthStart {azimuthStart},
    _azimuthSweep {azimuthSweep},
    _elevationStart {elevationStart},
    _elevationSweep {elevationSweep},
    _slices {slices},
    _segments {segments} {

    using namespace generator;

    /// @param radius The radius of the sphere
    /// @param slices Subdivisions around the z-azis (longitudes).
    /// @param segments Subdivisions along the z-azis (latitudes).
    /// @param sliceStart Counterclockwise angle around the z-axis relative to x-axis.
    /// @param sliceSweep Counterclockwise angle.
    /// @param segmentStart Counterclockwise angle relative to the z-axis.
    /// @param segmentSweep Counterclockwise angle.

    auto dome = SphereMesh {radius,         (int) slices, (int) segments, elevationStart,
                            elevationSweep, azimuthStart, azimuthSweep};

    // Build an indexed triangle list.
    beginBuild(VertexLayout::PNT, (uint16_t) sizeof(VertexPNT), PrimitiveTopology::Triangles, IndexFormat::U16);

    // --- vertices ---
    for (auto vs = dome.vertices(); !vs.done(); vs.next()) {
        const auto      v = vs.generate();
        const VertexPNT out {{(float) v.position[0], (float) v.position[1], (float) v.position[2]},
                             {(float) v.normal[0], (float) v.normal[1], (float) v.normal[2]},
                             {(float) v.texCoord[0], (float) v.texCoord[1]}};
        appendVertexBytes(&out);
    }

    // --- triangles (collect, reverse order like your old _faces reverse) ---
    std::vector<std::array<uint32_t, 3>> tris;
    for (auto ts = dome.triangles(); !ts.done(); ts.next()) {
        const auto t = ts.generate();
        tris.push_back({(uint32_t) t.vertices[0], (uint32_t) t.vertices[1], (uint32_t) t.vertices[2]});
    }

//	std::reverse(tris.begin(), tris.end());

    for (const auto& tri : tris) {
        appendTriangle(tri[0], tri[1], tri[2]);
    }

    endBuild(true);
}

// [Public Member Functions]

float Dome::radius() const {
    return _radius;
}

float Dome::azimuthStart() const {
    return _azimuthStart;
}

float Dome::azimuthSweep() const {
    return _azimuthSweep;
}

float Dome::elevationStart() const {
    return _elevationStart;
}

float Dome::elevationSweep() const {
    return _elevationSweep;
}

unsigned Dome::slices() const {
    return _slices;
}

unsigned Dome::segments() const {
    return _segments;
}
} // namespace a3d
