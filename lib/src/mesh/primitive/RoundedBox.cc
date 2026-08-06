//
//  RoundedBox.cc
//  avara3d
//
//  Created by Morgan Davis on 3/6/24.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#include "a3d/mesh/primitive/RoundedBox.h"

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

shared_ptr<Mesh> RoundedBox::Mesh(float                      radius,
                                  float                      length,
                                  float                      width,
                                  float                      height,
                                  unsigned                   slices,
                                  unsigned                   lengthSegments,
                                  unsigned                   widthSegments,
                                  unsigned                   heightSegments,
                                  const shared_ptr<Material> material) {

    return make_shared<a3d::Mesh>("RoundedBox",
                                  make_unique<RoundedBox>(radius, length, width, height, slices, lengthSegments,
                                                          widthSegments, heightSegments),
                                  material);
}

/// Public Lifecycle Functions ///

RoundedBox::RoundedBox(float    radius,
                       float    length,
                       float    width,
                       float    height,
                       unsigned slices,
                       unsigned lengthSegments,
                       unsigned widthSegments,
                       unsigned heightSegments):
    MeshElement {},
    _radius {radius},
    _length {length},
    _width {width},
    _height {height},
    _slices {slices},
    _lengthSegments {lengthSegments},
    _widthSegments {widthSegments},
    _heightSegments {heightSegments} {

    using namespace generator;

    /// @param radius Radius of the rounded edges.
    /// @param size Half of the side length in x (0), y (1) and z (2) direction.
    /// @param slices Number subdivions around in the rounded edges.
    /// @param segments Number of subdivisons in x (0), y (1) and z (2)
    /// direction for the flat faces.

    auto roundedBox = RoundedBoxMesh {radius,
                                      {width / 2.0, length / 2.0, height / 2.0},
                                      (int) slices,
                                      {widthSegments, lengthSegments, heightSegments}};

    beginBuild(VertexLayout::PNT, (uint16_t) sizeof(VertexPNT), PrimitiveTopology::Triangles, IndexFormat::U32);

    for (auto vs = roundedBox.vertices(); !vs.done(); vs.next()) {
        const auto      v = vs.generate();
        const VertexPNT out {{(float) v.position[0], (float) v.position[1], (float) v.position[2]},
                             {(float) v.normal[0], (float) v.normal[1], (float) v.normal[2]},
                             {(float) v.texCoord[0], (float) v.texCoord[1]}};
        appendVertexBytes(&out);
    }

    for (auto ts = roundedBox.triangles(); !ts.done(); ts.next()) {
        const auto t = ts.generate();
        appendTriangle((uint32_t) t.vertices[0], (uint32_t) t.vertices[1], (uint32_t) t.vertices[2]);
    }

    endBuild(false);

    auto xRotation = rotate(mat4(1.0f), (float) radians(-90.0f), vec3(1.0f, 0.0f, 0.0f));
    burnTransform(xRotation, true);
}

/// Public Member Functions ///

float RoundedBox::radius() const {
    return _radius;
}

float RoundedBox::length() const {
    return _length;
}

float RoundedBox::width() const {
    return _width;
}

float RoundedBox::height() const {
    return _height;
}

unsigned RoundedBox::slices() const {
    return _slices;
}

unsigned RoundedBox::lengthSegments() const {
    return _lengthSegments;
}

unsigned RoundedBox::widthSegments() const {
    return _widthSegments;
}

unsigned RoundedBox::heightSegments() const {
    return _heightSegments;
}
