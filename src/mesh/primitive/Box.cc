//
//  Box.cc
//  avara3d
//
//  Created by Morgan Davis on 10/31/17.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#include "a3d/mesh/primitive/Box.h"

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

shared_ptr<Mesh> Box::Mesh(float length,
						   float width,
						   float height,
						   unsigned lengthSegments,
						   unsigned widthSegments,
						   unsigned heightSegments,
						   const shared_ptr<Material> material) {


	return make_shared<a3d::Mesh>("Box",
								  make_unique<Box>(length,
												   width,
												   height,
												   lengthSegments,
												   widthSegments,
												   heightSegments),
								  material);
}

/// Public Lifecycle Functions ///

Box::Box(float length,
		 float width,
		 float height,
		 unsigned lengthSegments,
		 unsigned widthSegments,
		 unsigned heightSegments):
		MeshElement{},
		_length{length},
		_width{width},
		_height{height},
		_lengthSegments{lengthSegments},
		_widthSegments{widthSegments},
		_heightSegments{heightSegments} {

	using namespace generator;

	/// @param size Half of the side length in x (0), y (1) and z (2) direction.
	/// @param segments The number of segments in x (0), y (1) and z (2)

	auto box = BoxMesh{ { width/2.0, length/2.0, height/2.0 },
						{ widthSegments, lengthSegments, heightSegments } };

	// Build an indexed triangle list.
	// U16 is safe for boxes (vertex count << 65535). If you ever generate >65535 verts, switch to U32.
	beginBuild(VertexLayout::PNT,
			   (uint16_t)sizeof(VertexPNT),
			   PrimitiveTopology::Triangles,
			   IndexFormat::U16);

	// --- vertices ---
	for (auto vs = box.vertices(); !vs.done(); vs.next()) {
		const auto v = vs.generate();
		const VertexPNT out{
				{ (float)v.position[0], (float)v.position[1], (float)v.position[2] },
				{ (float)v.normal[0],   (float)v.normal[1],   (float)v.normal[2]   },
				{ (float)v.texCoord[0], (float)v.texCoord[1] } };
		appendVertexBytes(&out);
	}

	// --- triangles (collect, reverse order like your old _faces reverse) ---
	std::vector<std::array<uint32_t, 3>> tris;
	for (auto ts = box.triangles(); !ts.done(); ts.next()) {
		const auto t = ts.generate();
		tris.push_back({ (uint32_t)t.vertices[0],
						 (uint32_t)t.vertices[1],
						 (uint32_t)t.vertices[2] });
	}

	std::reverse(tris.begin(), tris.end());

	for (const auto& tri : tris) {
		appendTriangle(tri[0], tri[1], tri[2]);
	}

	endBuild(true);
}

/// Public Member Functions ///

float Box::length() const {
	return _length;
}

float Box::width() const {
	return _width;
}

float Box::height() const {
	return _height;
}

unsigned Box::lengthSegments() const {
	return _lengthSegments;
}

unsigned Box::widthSegments() const {
	return _widthSegments;
}

unsigned Box::heightSegments() const {
	return _heightSegments;
}
