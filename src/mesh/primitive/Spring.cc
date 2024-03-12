//
// Created by mkd on 3/6/24.
//

#include "a3d/mesh/primitive/Spring.h"

#include "generator/generator.hpp"
#include "glm/glm.hpp"

#include "a3d/mesh/Mesh.h"
#include "a3d/mesh/MeshElement.h"
#include "a3d/rendering/material/Material.h"


using namespace a3d;
using namespace generator;
using namespace glm;
using namespace std;


unique_ptr<Mesh> Spring::Mesh(float minorRadius,
							  float majorRadius,
							  float length,
							  int slices,
							  int segments,
							  shared_ptr<Material> material) {

	return make_unique<a3d::Mesh>(make_shared<Spring>(minorRadius,
													  majorRadius,
													  length,
													  slices,
													  segments),
								  material);
}

/*********************************************************************************************
	Lifecycle
*********************************************************************************************/

Spring::Spring(float minorRadius,
			   float majorRadius,
			   float length,
			   int slices,
			   int segments):
		MeshElement(),
		_minorRadius{minorRadius},
		_majorRadius{majorRadius},
		_length{length},
		_slices{slices},
		_segments{segments} {

	/// @param minor Radius of the spring it self.
	/// @param major Radius from the z-axis
	/// @param size Half of the length along the z-axis.
	/// @param slices Subdivisions around the spring.
	/// @param segments Subdivisions along the path.
	/// @param majorStart Counterclockwise angle around the z-axis relative to the x-axis.
	/// @param majorSweep Counterclockwise angle arounf the z-axis.

	auto spring = SpringMesh{minorRadius, majorRadius, length/2.0, slices, segments};

	for (const MeshVertex& v : spring.vertices()) {
		_vertices.push_back({ vec3(v.position[0], v.position[1], v.position[2]),
							  vec3(v.normal[0], v.normal[1], v.normal[2]),
							  vec2(v.texCoord[0], v.texCoord[1]) });
	}

	for (const Triangle& t : spring.triangles()) {
		_faces.push_back({ unsigned(t.vertices[0]),
						   unsigned(t.vertices[1]),
						   unsigned(t.vertices[2]) });
	}
}

/*********************************************************************************************
	Public
 *********************************************************************************************/

float Spring::minorRadius() const {
	return _minorRadius;
}

float Spring::majorRadius() const {
	return _majorRadius;
}

float Spring::length() const {
	return _length;
}

int Spring::slices() const {
	return _slices;
}

int Spring::segments() const {
	return _segments;
}
