//
//  Disk.cc
//  avara3d
//
//  Created by Morgan Davis on 3/6/24.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#include "a3d/mesh/primitive/Disk.h"

#include <generator/generator.hpp>

#include "a3d/mesh/Mesh.h"
#include "a3d/mesh/MeshElement.h"
#include "a3d/mesh/VertexFormats.h"
#include "a3d/visual/material/Material.h"

using namespace a3d;
using namespace a3d::math;
using namespace std;

/// Pubic Static Members ///

shared_ptr<Mesh> Disk::Mesh(float radius,
							float height,
							unsigned slices,
							unsigned rings,
							const shared_ptr<Material> material) {

	return make_shared<a3d::Mesh>("Disk",
								  make_unique<Disk>(radius,
													height,
													slices,
													rings),
								  material);
}

/// Public Lifecycle Functions ///

Disk::Disk(float radius,
		   float innerRadius,
		   unsigned slices,
		   unsigned rings):
		MeshElement{},
		_radius{radius},
		_innerRadius{innerRadius},
		_slices{slices},
		_rings{rings} {

	using namespace generator;

	/// @param radius Outer radius of the disk on the xy-plane.
	/// @param innerRadius radius of the inner circle on the xy-plane.
	/// @param slices Number of subdivisions around the z-axis.
	/// @param rings Number of subdivisions along the radius.
	/// @param start Counterclockwise angle relative to the x-axis
	/// @param sweep Counterclockwise angle.

	auto disk = DiskMesh{radius, innerRadius, (int)slices, (int)rings};

	beginBuild(VertexLayout::PNT, (uint16_t)sizeof(VertexPNT));

	for (auto vs = disk.vertices(); !vs.done(); vs.next()) {
		const auto v = vs.generate();
		const VertexPNT out{
				{ (float)v.position[0], (float)v.position[1], (float)v.position[2] },
				{ (float)v.normal[0],   (float)v.normal[1],   (float)v.normal[2]   },
				{ (float)v.texCoord[0], (float)v.texCoord[1] } };
		appendVertexBytes(&out);
	}

	for (auto ts = disk.triangles(); !ts.done(); ts.next()) {
		const auto t = ts.generate();
		appendFace(Face{
				(uint32_t)t.vertices[0],
				(uint32_t)t.vertices[1],
				(uint32_t)t.vertices[2] });
	}

	endBuild(false);

	// Bullet orientation
	auto xRotation = rotate(mat4(1.0f), (float)radians(-90.0f), vec3(1.0f, 0.0f, 0.0f));
	burnTransform(xRotation, true);
}

/// Public Member Functions ///

float Disk::radius() const {
	return _radius;
}

float Disk::innerRadius() const {
	return _innerRadius;
}

unsigned Disk::slices() const {
	return _slices;
}

unsigned Disk::rings() const {
	return _rings;
}
