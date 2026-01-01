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
#include "a3d/rendering/material/Material.h"

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

	for (const MeshVertex& v : disk.vertices()) {
		_vertices.push_back({ vec3(v.position[0], v.position[1], v.position[2]),
							  vec3(v.normal[0], v.normal[1], v.normal[2]),
							  vec2(v.texCoord[0], v.texCoord[1]) });
	}

	for (const Triangle& t : disk.triangles()) {
		_faces.push_back({ unsigned(t.vertices[0]),
						   unsigned(t.vertices[1]),
						   unsigned(t.vertices[2]) });
	}

	// this orientation is what bullet expects
	auto xRotation = rotate(mat4(1.0), (float)radians(-90.0), vec3(1.0, 0.0, 0.0));
	burnTransform(xRotation, true);

	//genLocalAABB(); // ^^ burnTransform() calls genLocalAABB()
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
