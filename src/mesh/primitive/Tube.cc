//
//  Tube.cc
//	avara3d
//
//  Created by Morgan Davis on 11/8/17.
//  Copyright © 2017 Morgan K Davis. All rights reserved.
//

#include "a3d/mesh/primitive/Tube.h"

#include "generator/generator.hpp"
#include "glm/glm.hpp"

#include "a3d/mesh/Mesh.h"
#include "a3d/mesh/MeshElement.h"
#include "a3d/rendering/material/Material.h"


using namespace a3d;
using namespace generator;
using namespace glm;
using namespace std;


shared_ptr<Mesh> Tube::Mesh(float innerRadius,
							float outerRadius,
							float height,
							int slices,
							int segments,
							int rings,
							shared_ptr<Material> material) {

	return make_shared<a3d::Mesh>(make_shared<Tube>(innerRadius,
													outerRadius,
													height,
													slices,
													segments,
													rings),
								  material);
}

/*********************************************************************************************
	Lifecycle
*********************************************************************************************/

Tube::Tube(float innerRadius,
		   float outerRadius,
		   float height,
		   int slices,
		   int segments,
		   int rings):
		MeshElement(),
		_innerRadius{innerRadius},
		_outerRadius{outerRadius},
		_height{height},
		_slices{slices},
		_segments{segments},
		_rings{rings} {

	/// @param radius The outer radius of the cylinder on the xy-plane.
	/// @param innerRadius The inner radius of the cylinder on the xy-plane.
	/// @param size Half of the length of the cylinder along the z-axis.
	/// @param slices Number nubdivisions around the z-axis.
	/// @param segments Number of subdivisions along the z-axis.
	/// @param rings Number radial subdivisions in the cap.
	/// @param start Counterclockwise angle around the z-axis relative to the x-axis.
	/// @param sweep Counterclockwise angle around the z-axis.

	auto tube = CappedTubeMesh{outerRadius, innerRadius, height/2.0, slices, segments, rings};

	for (const MeshVertex& v : tube.vertices()) {
		_vertices.push_back({ vec3(v.position[0], v.position[1], v.position[2]),
							  vec3(v.normal[0], v.normal[1], v.normal[2]),
							  vec2(v.texCoord[0], v.texCoord[1]) });
	}

	for (const Triangle& t : tube.triangles()) {
		_faces.push_back({ unsigned(t.vertices[0]),
						   unsigned(t.vertices[1]),
						   unsigned(t.vertices[2]) });
	}
}

/*********************************************************************************************
 	Public
 *********************************************************************************************/

float Tube::innerRadius() const {
	return _innerRadius;
}

float Tube::outerRadius() const {
	return _outerRadius;
}

float Tube::height() const {
	return _height;
}

int Tube::slices() const {
	return _slices;
}

int Tube::segments() const {
	return _segments;
}

int Tube::rings() const {
	return _rings;
}
