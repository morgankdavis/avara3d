//
//  Torus.cc
//  avara3d
//
//  Created by Morgan Davis on 11/8/17.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#include "a3d/mesh/primitive/Torus.h"

#include <generator/generator.hpp>

#include "a3d/mesh/Mesh.h"
#include "a3d/mesh/MeshElement.h"
#include "a3d/visual/material/Material.h"

using namespace a3d;
using namespace a3d::math;
using namespace std;

/// Pubic Static Members ///

shared_ptr<Mesh> Torus::Mesh(float minorRadius,
							 float majorRadius,
							 unsigned slices,
							 unsigned segments,
							 const shared_ptr<Material> material) {

	return make_shared<a3d::Mesh>("Torus",
								  make_unique<Torus>(minorRadius,
													 majorRadius,
													 slices,
													 segments),
								  material);
}

/// Public Lifecycle Functions ///

Torus::Torus(float minorRadius,
			 float majorRadius,
			 unsigned slices,
			 unsigned segments):
		MeshElement{},
		_minorRadius{minorRadius},
		_majorRadius{majorRadius},
		_slices{slices},
		_segments{segments} {

	using namespace generator;

	/// @param minor Radius of the minor (inner) ring
	/// @param major Radius of the major (outer) ring
	/// @param slices Subdivisions around the minor ring
	/// @param segments Subdivisions around the major ring
	/// @param minorStart Counterclockwise angle relative to the xy-plane.
	/// @param minorSweep Counterclockwise angle around the circle.
	/// @param majorStart Counterclockwise angle around the z-axis relative to the x-axis.
	/// @param majorSweep Counterclockwise angle around the z-axis.

	auto torus = TorusMesh{majorRadius-minorRadius, majorRadius, (int)slices, (int)segments};

	for (const MeshVertex& v : torus.vertices()) {
		_vertices.push_back({ vec3(v.position[0], v.position[1], v.position[2]),
							  vec3(v.normal[0], v.normal[1], v.normal[2]),
							  vec2(v.texCoord[0], v.texCoord[1]) });
	}

	for (const Triangle& t : torus.triangles()) {
		_faces.push_back({ unsigned(t.vertices[0]),
						   unsigned(t.vertices[1]),
						   unsigned(t.vertices[2]) });
	}

	genLocalAABB();
}

/// Public Member Functions ///

float Torus::minorRadius() const {
	return _minorRadius;
}

float Torus::majorRadius() const {
	return _majorRadius;
}

unsigned Torus::slices() const {
	return _slices;
}

unsigned Torus::segments() const {
	return _segments;
}
