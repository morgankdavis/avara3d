//
//  Cone.cc
//  avara3d
//
//  Created by Morgan Davis on 11/8/17.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#include "a3d/mesh/primitive/Cone.h"

#include <generator/generator.hpp>

#include "a3d/mesh/Mesh.h"
#include "a3d/mesh/MeshElement.h"
#include "a3d/mesh/PrimitiveTopology.h"
#include "a3d/mesh/VertexFormats.h"
#include "a3d/visual/material/Material.h"

using namespace a3d;
using namespace a3d::math;
using namespace std;

// [Pubic Static Members]

shared_ptr<Mesh> Cone::Mesh(float                      radius,
                            float                      height,
                            unsigned                   slices,
                            unsigned                   segments,
                            unsigned                   rings,
                            const shared_ptr<Material> material) {

    return make_shared<a3d::Mesh>("Cone", make_unique<Cone>(radius, height, slices, segments, rings), material);
}

// [Public Lifecycle Functions]

Cone::Cone(float radius, float height, unsigned slices, unsigned segments, unsigned rings):
    MeshElement {},
    _radius {radius},
    _height {height},
    _slices {slices},
    _segments {segments},
    _rings {rings} {

    using namespace generator;

    /// @param radius Radius of the negative z end on the xy-plane.
    /// @param size Half of the length of the cylinder along the z-axis.
    /// @param slices Number of subdivisions around the z-axis.
    /// @param segments Number subdivisions along the z-axis.
    /// @param start Counterclockwise angle around the z-axis relative to the x-axis.
    /// @param sweep Counterclockwise angle around the z-axis.

    auto cone = CappedConeMesh {radius, height / 2.0, (int) slices, (int) segments, (int) rings};

    beginBuild(VertexLayout::PNT, (uint16_t) sizeof(VertexPNT), PrimitiveTopology::Triangles, IndexFormat::U32);

    for (auto vs = cone.vertices(); !vs.done(); vs.next()) {
        const auto      v = vs.generate();
        const VertexPNT out {{(float) v.position[0], (float) v.position[1], (float) v.position[2]},
                             {(float) v.normal[0], (float) v.normal[1], (float) v.normal[2]},
                             {(float) v.texCoord[0], (float) v.texCoord[1]}};
        appendVertexBytes(&out);
    }

    for (auto ts = cone.triangles(); !ts.done(); ts.next()) {
        const auto t = ts.generate();
        appendTriangle((uint32_t) t.vertices[0], (uint32_t) t.vertices[1], (uint32_t) t.vertices[2]);
    }

    endBuild(false);

    auto xRotation = rotate(mat4(1.0f), (float) radians(-90.0f), vec3(1.0f, 0.0f, 0.0f));
    burnTransform(xRotation, true);
}

// [Public Member Functions]

float Cone::radius() const {
    return _radius;
}

float Cone::height() const {
    return _height;
}

unsigned Cone::slices() const {
    return _slices;
}

unsigned Cone::segments() const {
    return _segments;
}

unsigned Cone::rings() const {
    return _rings;
}
