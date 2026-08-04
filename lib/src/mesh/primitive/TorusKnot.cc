//
//  TorusKnot.cc
//  avara3d
//
//  Created by Morgan Davis on 3/6/24.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#include "a3d/mesh/primitive/TorusKnot.h"

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

shared_ptr<Mesh> TorusKnot::Mesh(unsigned                   p,
                                 unsigned                   q,
                                 unsigned                   slices,
                                 unsigned                   segments,
                                 const shared_ptr<Material> material) {

    return make_shared<a3d::Mesh>("TorusKnot", make_unique<TorusKnot>(p, q, slices, segments), material);
}

/// Public Lifecycle Functions ///

TorusKnot::TorusKnot(unsigned p, unsigned q, unsigned slices, unsigned segments):
    MeshElement {},
    _p {p},
    _q {q},
    _slices {slices},
    _segments {segments} {

    using namespace generator;

    /// @param slices Number subdivisions around the circle.
    /// @param segments Number of subdivisions around the path.

    auto torusKnot = TorusKnotMesh {(int) p, (int) q, (int) slices, (int) segments};

    beginBuild(VertexLayout::PNT, (uint16_t) sizeof(VertexPNT), PrimitiveTopology::Triangles, IndexFormat::U32);

    for (auto vs = torusKnot.vertices(); !vs.done(); vs.next()) {
        const auto      v = vs.generate();
        const VertexPNT out {{(float) v.position[0], (float) v.position[1], (float) v.position[2]},
                             {(float) v.normal[0], (float) v.normal[1], (float) v.normal[2]},
                             {(float) v.texCoord[0], (float) v.texCoord[1]}};
        appendVertexBytes(&out);
    }

    for (auto ts = torusKnot.triangles(); !ts.done(); ts.next()) {
        const auto t = ts.generate();
        appendTriangle((uint32_t) t.vertices[0], (uint32_t) t.vertices[1], (uint32_t) t.vertices[2]);
    }

    endBuild(true);
}

/// Public Member Functions ///

unsigned TorusKnot::p() const {
    return _p;
}

unsigned TorusKnot::q() const {
    return _q;
}

unsigned TorusKnot::slices() const {
    return _slices;
}

unsigned TorusKnot::segments() const {
    return _segments;
}