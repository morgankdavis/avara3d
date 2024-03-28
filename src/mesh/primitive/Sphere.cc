//
//  Sphere.cc
//	avara3d
//
//  Created by Morgan Davis on 11/6/17.
//  Copyright © 2017 Morgan K Davis. All rights reserved.
//

#include "a3d/mesh/primitive/Sphere.h"

#include "generator/generator.hpp"
#include "glm/glm.hpp"

#include "a3d/mesh/Mesh.h"
#include "a3d/mesh/MeshElement.h"
#include "a3d/rendering/material/Material.h"


using namespace a3d;
using namespace generator;
using namespace glm;
using namespace std;


shared_ptr<Mesh> Sphere::Mesh(float radius,
							  int segments,
							  shared_ptr<Material> material) {

	return make_shared<a3d::Mesh>(make_unique<Sphere>(radius,
													  segments),
								  material);
}

/*********************************************************************************************
	Lifecycle
*********************************************************************************************/

Sphere::Sphere(float radius,
			   int segments):
		MeshElement{},
		_radius{radius},
		_segments{segments} {

	/// @param radius The radius of the containing sphere.
	/// @param segments The number of segments per icosahedron edge. Must be >= 1.

	auto icoSphere = IcoSphereMesh{radius, segments};

	for (const MeshVertex& v : icoSphere.vertices()) {
		_vertices.push_back({ vec3(v.position[0], v.position[1], v.position[2]),
							  vec3(v.normal[0], v.normal[1], v.normal[2]),
							  vec2(v.texCoord[0], v.texCoord[1]) });
	}

	for (const Triangle& t : icoSphere.triangles()) {
		_faces.push_back({ unsigned(t.vertices[0]),
						   unsigned(t.vertices[1]),
						   unsigned(t.vertices[2]) });
	}
}

/*********************************************************************************************
 	Public
 *********************************************************************************************/

float Sphere::radius() const {
	return _radius;
}

int Sphere::segments() const {
	return _segments;
}
