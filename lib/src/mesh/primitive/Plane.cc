//
//  Plane.cc
//  avara3d
//
//  Created by Morgan Davis on 10/31/17.
//  Copyright © 2017-2026 Morgan K Davis. All rights reserved.
//

#include "a3d/mesh/primitive/Plane.h"

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

shared_ptr<Mesh> Plane::Mesh(float                      width,
                             float                      height,
                             unsigned                   widthSegements,
                             unsigned                   heightSegments,
                             const shared_ptr<Material> material) {

    return make_shared<a3d::Mesh>("Plane", make_unique<Plane>(width, height, widthSegements, heightSegments),
                                  material);
}

/// Public Lifecycle Functions ///

Plane::Plane(float width, float height, unsigned widthSegements, unsigned heightSegments):
    MeshElement {},
    _width {width},
    _height {height},
    _widthSegements {widthSegements},
    _heightSegments {heightSegments} {

    using namespace generator;

    /// @param size Half of the side length in x (0) and y (1) direction.
    /// @param segments Number of subdivisions in the x (0) and y (1) direction.

    auto plane = PlaneMesh {{width / 2.0, height / 2.0}, {widthSegements, heightSegments}};

    beginBuild(VertexLayout::PNT, (uint16_t) sizeof(VertexPNT), PrimitiveTopology::Triangles, IndexFormat::U32);

    for (auto vs = plane.vertices(); !vs.done(); vs.next()) {
        const auto      v = vs.generate();
        const VertexPNT out {{(float) v.position[0], (float) v.position[1], (float) v.position[2]},
                             {(float) v.normal[0], (float) v.normal[1], (float) v.normal[2]},
                             {(float) v.texCoord[0], (float) v.texCoord[1]}};
        appendVertexBytes(&out);
    }

    for (auto ts = plane.triangles(); !ts.done(); ts.next()) {
        const auto t = ts.generate();
        appendTriangle((uint32_t) t.vertices[0], (uint32_t) t.vertices[1], (uint32_t) t.vertices[2]);
    }

    endBuild(true);
}

/// Public Member Functions ///

float Plane::width() const {
    return _width;
}

float Plane::height() const {
    return _height;
}

unsigned Plane::widthSegements() const {
    return _widthSegements;
}

unsigned Plane::heightSegments() const {
    return _heightSegments;
}
