//
//  Capsule.cc
//	avara3d
//
//  Created by Morgan Davis on 11/8/17.
//  Copyright © 2017 Morgan K Davis. All rights reserved.
//

#include "a3d/mesh/primitive/Capsule.h"

#include "generator/generator.hpp"

#include "a3d/Types.h"
#include "a3d/mesh/MeshElement.h"


using namespace a3d;
using namespace generator;
using namespace glm;
using namespace std;


/*********************************************************************************************
	Lifecycle
*********************************************************************************************/

Capsule::Capsule(float radius,
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

	/// @param radius Radius of the capsule on the xy-plane.
	/// @param size Half of the length between centers of the caps along the z-axis.
	/// @param slices Number of subdivisions around the z-axis.
	/// @param rings Number of radial subdivisions in the caps.
	/// @param start Counterclockwise angle relative to the x-axis.
	/// @param sweep Counterclockwise angle.

	auto capsule = CapsuleMesh{ radius, height/2.0, slices, segments, rings };

	for (const MeshVertex& v : capsule.vertices()) {
		_vertices.push_back({ vec3(v.position[0], v.position[1], v.position[2]),
							  vec3(v.normal[0], v.normal[1], v.normal[2]),
							  vec2(v.texCoord[0], v.texCoord[1]) });
	}

	for (const Triangle& t : capsule.triangles()) {
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

float Capsule::radius() const {
	return _radius;
}

float Capsule::height() const {
	return _height;
}

int Capsule::slices() const {
	return _slices;
}

int Capsule::segments() const {
	return _segments;
}

int Capsule::rings() const {
	return _rings;
}
