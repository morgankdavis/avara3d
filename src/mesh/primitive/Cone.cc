//
//  Cone.cc
//  avara3d
//
//  Created by Morgan Davis on 11/8/17.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#include "a3d/mesh/primitive/Cone.h"

#include "generator/generator.hpp"
#
#include "a3d/mesh/Mesh.h"
#include "a3d/mesh/MeshElement.h"
#include "a3d/rendering/material/Material.h"

using namespace a3d;
using namespace a3d::math;
using namespace std;

/// Pubic Static Members ///

shared_ptr<Mesh> Cone::Mesh(float radius,
							float height,
							unsigned slices,
							unsigned segments,
							unsigned rings,
							const shared_ptr<Material> material) {

	return make_shared<a3d::Mesh>("Cone",
								  make_unique<Cone>(radius,
													height,
													slices,
													segments,
													rings),
								  material);
}

/// Public Lifecycle Functions ///

Cone::Cone(float radius,
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

	/// @param radius Radius of the negative z end on the xy-plane.
	/// @param size Half of the length of the cylinder along the z-axis.
	/// @param slices Number of subdivisions around the z-axis.
	/// @param segments Number subdivisions along the z-axis.
	/// @param start Counterclockwise angle around the z-axis relative to the x-axis.
	/// @param sweep Counterclockwise angle around the z-axis.

	auto cone = CappedConeMesh{radius, height/2.0, (int)slices, (int)segments, (int)rings};

	for (const MeshVertex& v : cone.vertices()) {
		_vertices.push_back({ vec3(v.position[0], v.position[1], v.position[2]),
							  vec3(v.normal[0], v.normal[1], v.normal[2]),
							  vec2(v.texCoord[0], v.texCoord[1]) });
	}

	for (const Triangle& t : cone.triangles()) {
		_faces.push_back({ unsigned(t.vertices[0]),
						   unsigned(t.vertices[1]),
						   unsigned(t.vertices[2]) });
	}

	// this orientation is what bullet expects
	auto xRotation = rotate(mat4(1.0), (float)radians(-90.0), vec3(1.0, 0.0, 0.0));
	burnTransform(xRotation, true);
}

/// Public Member Functions ///

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
