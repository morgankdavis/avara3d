//
//  Torus.cc
//	avara3d
//
//  Created by Morgan Davis on 11/8/17.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#include "a3d/mesh/primitive/Torus.h"

#include "generator/generator.hpp"
#include "glm/glm.hpp"

#include "a3d/mesh/Mesh.h"
#include "a3d/mesh/MeshElement.h"
#include "a3d/rendering/material/Material.h"


using namespace a3d;
using namespace generator;
using namespace glm;
using namespace std;


shared_ptr<Mesh> Torus::Mesh(float minorRadius,
							 float majorRadius,
							 int slices,
							 int segments,
							 const shared_ptr<Material> material) {

	return make_shared<a3d::Mesh>("Torus",
								  make_unique<Torus>(minorRadius,
													 majorRadius,
													 slices,
													 segments),
								  material);
}

/*********************************************************************************************
	Lifecycle
*********************************************************************************************/

Torus::Torus(float minorRadius,
			 float majorRadius,
			 int slices,
			 int segments):
		MeshElement{},
		_minorRadius{minorRadius},
		_majorRadius{majorRadius},
		_slices{slices},
		_segments{segments} {

	/// @param minor Radius of the minor (inner) ring
	/// @param major Radius of the major (outer) ring
	/// @param slices Subdivisions around the minor ring
	/// @param segments Subdivisions around the major ring
	/// @param minorStart Counterclockwise angle relative to the xy-plane.
	/// @param minorSweep Counterclockwise angle around the circle.
	/// @param majorStart Counterclockwise angle around the z-axis relative to the x-axis.
	/// @param majorSweep Counterclockwise angle around the z-axis.

	auto torus = TorusMesh{majorRadius-minorRadius, majorRadius, slices, segments};

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
}

/*********************************************************************************************
 	Public
 *********************************************************************************************/

float Torus::minorRadius() const {
	return _minorRadius;
}

float Torus::majorRadius() const {
	return _majorRadius;
}

int Torus::slices() const {
	return _slices;
}

int Torus::segments() const {
	return _segments;
}
