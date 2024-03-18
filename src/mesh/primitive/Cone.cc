//
//  Cone.cc
//	avara3d
//
//  Created by Morgan Davis on 11/8/17.
//  Copyright © 2017 Morgan K Davis. All rights reserved.
//

#include "a3d/mesh/primitive/Cone.h"

#include "generator/generator.hpp"
#include "glm/glm.hpp"

#include "a3d/mesh/Mesh.h"
#include "a3d/mesh/MeshElement.h"
#include "a3d/rendering/material/Material.h"


using namespace a3d;
using namespace generator;
using namespace glm;
using namespace std;


unique_ptr<Mesh> Cone::Mesh(float radius,
							float height,
							int slices,
							int segments,
							int rings,
							shared_ptr<Material> material) {

	return make_unique<a3d::Mesh>(make_unique<Cone>(radius,
													height,
													slices,
													segments,
													rings),
								  material);
}

/*********************************************************************************************
	Lifecycle
*********************************************************************************************/

Cone::Cone(float radius,
		   float height,
		   int slices,
		   int segments,
		   int rings):
		MeshElement(),
		_radius{radius},
		_height{height},
		_slices{slices},
		_segments{segments},
		_rings{rings} {

	/// @param radius Radius of the negative z end on the xy-plane.
	/// @param size Half of the length of the cylinder along the z-axis.
	/// @param slices Number of subdivisions around the z-axis.
	/// @param segments Number subdivisions along the z-axis.
	/// @param start Counterclockwise angle around the z-axis relative to the x-axis.
	/// @param sweep Counterclockwise angle around the z-axis.

	auto cone = CappedConeMesh{radius, height/2.0, slices, segments, rings};

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

/*********************************************************************************************
	Public
 *********************************************************************************************/

float Cone::radius() const {
	return _radius;
}

float Cone::height() const {
	return _height;
}

int Cone::slices() const {
	return _slices;
}

int Cone::segments() const {
	return _segments;
}

int Cone::rings() const {
	return _rings;
}
