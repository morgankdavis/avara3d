//
//  Sphere.cc
//  avara3d
//
//  Created by Morgan Davis on 11/6/17.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#include "a3d/mesh/primitive/Sphere.h"

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

shared_ptr<Mesh> Sphere::Mesh(float radius, unsigned segments, const shared_ptr<Material> material) {

    return make_shared<a3d::Mesh>("Sphere", make_unique<Sphere>(radius, segments), material);
}

// [Public Lifecycle Functions]

Sphere::Sphere(float radius, unsigned segments):
    MeshElement {},
    _radius {radius},
    _segments {segments} {

    using namespace generator;

    /// @param radius The radius of the containing sphere.
    /// @param segments The number of segments per icosahedron edge. Must be >= 1.

    auto icoSphere = IcoSphereMesh {radius, (int) segments};

    beginBuild(VertexLayout::PNT, (uint16_t) sizeof(VertexPNT), PrimitiveTopology::Triangles, IndexFormat::U32);

    for (auto vs = icoSphere.vertices(); !vs.done(); vs.next()) {
        const auto      v = vs.generate();
        const VertexPNT out {{(float) v.position[0], (float) v.position[1], (float) v.position[2]},
                             {(float) v.normal[0], (float) v.normal[1], (float) v.normal[2]},
                             {(float) v.texCoord[0], (float) v.texCoord[1]}};
        appendVertexBytes(&out);
    }

    for (auto ts = icoSphere.triangles(); !ts.done(); ts.next()) {
        const auto t = ts.generate();
        appendTriangle((uint32_t) t.vertices[0], (uint32_t) t.vertices[1], (uint32_t) t.vertices[2]);
    }

    endBuild(true);
}

// [Public Member Functions]

float Sphere::radius() const {
    return _radius;
}

unsigned Sphere::segments() const {
    return _segments;
}
