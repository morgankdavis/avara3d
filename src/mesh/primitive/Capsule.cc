//
//  Capsule.cc
//  avara3d
//
//  Created by Morgan Davis on 11/8/17.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#include "a3d/mesh/primitive/Capsule.h"

#include <generator/generator.hpp>

#include "a3d/mesh/Mesh.h"
#include "a3d/mesh/MeshElement.h"
#include "a3d/mesh/VertexFormats.h"
#include "a3d/visual/material/Material.h"

using namespace a3d;
using namespace a3d::math;
using namespace std;

/// Pubic Static Members ///

shared_ptr<Mesh> Capsule::Mesh(float radius,
							   float height,
							   unsigned slices,
							   unsigned segments,
							   unsigned rings,
							   const shared_ptr<Material> material) {

	return make_shared<a3d::Mesh>("Capsule",
								  make_unique<Capsule>(radius,
													   height,
													   slices,
													   segments,
													   rings),
								  material);
}

/// Public Lifecycle Functions ///

Capsule::Capsule(float radius,
				 float height,
				 unsigned slices,
				 unsigned segments,
				 unsigned rings):
		MeshElement{},
		_radius{radius},
		_height{height},
		_slices{slices},
		_segments{segments},
		_rings{rings} {

	using namespace generator;

	/// @param radius Radius of the capsule on the xy-plane.
	/// @param size Half of the length between centers of the caps along the z-axis.
	/// @param slices Number of subdivisions around the z-axis.
	/// @param rings Number of radial subdivisions in the caps.
	/// @param start Counterclockwise angle relative to the x-axis.
	/// @param sweep Counterclockwise angle.

	auto capsule = CapsuleMesh{radius, height/2.0, (int)slices, (int)segments, (int)rings};

	beginBuild(VertexLayout::PNT, (uint16_t)sizeof(VertexPNT));

	for (auto vs = capsule.vertices(); !vs.done(); vs.next()) {
		const auto v = vs.generate();
		VertexPNT out {
				{ (float)v.position[0], (float)v.position[1], (float)v.position[2] },
				{ (float)v.normal[0],   (float)v.normal[1],   (float)v.normal[2]   },
				{ (float)v.texCoord[0], (float)v.texCoord[1] } };
		appendVertexBytes(&out);
	}

	for (auto ts = capsule.triangles(); !ts.done(); ts.next()) {
		const auto t = ts.generate();
		appendFace(Face{ (uint32_t)t.vertices[0],
						 (uint32_t)t.vertices[1],
						 (uint32_t)t.vertices[2] });
	}

	endBuild(false);

	// orientation bullet expects
	auto xRotation = rotate(mat4(1.0), (float)radians(-90.0), vec3(1.0, 0.0, 0.0));
	burnTransform(xRotation, true);
}

/// Public Member Functions ///

float Capsule::radius() const {
	return _radius;
}

float Capsule::height() const {
	return _height;
}

unsigned Capsule::slices() const {
	return _slices;
}

unsigned Capsule::segments() const {
	return _segments;
}

unsigned Capsule::rings() const {
	return _rings;
}
