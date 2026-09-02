//
//  Cylinder.cc
//  avara3d
//
//  Created by Morgan Davis on 11/8/17.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#include "a3d/mesh/primitive/Cylinder.h"

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

shared_ptr<Mesh> Cylinder::Mesh(float                      radius,
                                float                      height,
                                unsigned                   slices,
                                unsigned                   segments,
                                unsigned                   rings,
                                const shared_ptr<Material> material) {

    return make_shared<a3d::Mesh>("Cylinder", make_unique<Cylinder>(radius, height, slices, segments, rings),
                                  material);
}

// [Public Lifecycle Functions]

Cylinder::Cylinder(float radius, float height, unsigned slices, unsigned segments, unsigned rings):
    MeshElement {},
    _radius {radius},
    _height {height},
    _slices {slices},
    _segments {segments},
    _rings {rings} {

    using namespace generator;

    /// @param radius Radius of the cylinder along the xy-plane.
    /// @param size Half of the length of the cylinder along the z-axis.
    /// @param slices Subdivisions around the z-axis.
    /// @param segments Subdivisions along the z-axis.
    /// @param start Counterclockwise angle around the z-axis relative to the x-axis.
    /// @param sweep Counterclockwise angle around the z-axis.

    auto cylinder = CappedCylinderMesh {radius, height / 2.0, (int) slices, (int) segments, (int) rings};

    beginBuild(VertexLayout::PNT, (uint16_t) sizeof(VertexPNT), PrimitiveTopology::Triangles, IndexFormat::U32);

    for (auto vs = cylinder.vertices(); !vs.done(); vs.next()) {
        const auto      v = vs.generate();
        const VertexPNT out {{(float) v.position[0], (float) v.position[1], (float) v.position[2]},
                             {(float) v.normal[0], (float) v.normal[1], (float) v.normal[2]},
                             {(float) v.texCoord[0], (float) v.texCoord[1]}};
        appendVertexBytes(&out);
    }

    for (auto ts = cylinder.triangles(); !ts.done(); ts.next()) {
        const auto t = ts.generate();
        appendTriangle((uint32_t) t.vertices[0], (uint32_t) t.vertices[1], (uint32_t) t.vertices[2]);
    }

    endBuild(false);

    auto xRotation = rotate(mat4(1.0f), (float) radians(-90.0f), vec3(1.0f, 0.0f, 0.0f));
    burnTransform(xRotation, true);
}

// [Public Member Functions]

float Cylinder::radius() const {
    return _radius;
}

float Cylinder::height() const {
    return _height;
}

unsigned Cylinder::slices() const {
    return _slices;
}

unsigned Cylinder::segments() const {
    return _segments;
}

unsigned Cylinder::rings() const {
    return _rings;
}

} // namespace a3d
