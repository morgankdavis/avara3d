//
//  Plane.cc
//	avara3d
//
//  Created by Morgan Davis on 10/31/17.
//  Copyright © 2017 Morgan K Davis. All rights reserved.
//

#include "a3d/mesh/primitive/Plane.h"

#include "generator/generator.hpp"
#include "glm/glm.hpp"

#include "a3d/mesh/Mesh.h"
#include "a3d/mesh/MeshElement.h"
#include "a3d/rendering/material/Material.h"


using namespace a3d;
using namespace generator;
using namespace glm;
using namespace std;


shared_ptr<Mesh> Plane::Mesh(float width,
							 float height,
							 int widthSegements,
							 int heightSegments,
							 shared_ptr<Material> material) {

	return make_shared<a3d::Mesh>(make_unique<Plane>(width,
													 height,
													 widthSegements,
													 heightSegments),
								  material);
}

/*********************************************************************************************
	 	Lifecycle
*********************************************************************************************/

Plane::Plane(float width,
			 float height,
			 int widthSegements,
			 int heightSegments):
		MeshElement(),
		_width{width},
		_height{height},
		_widthSegements{widthSegements},
		_heightSegments{heightSegments} {

	/// @param size Half of the side length in x (0) and y (1) direction.
	/// @param segments Number of subdivisions in the x (0) and y (1) direction.

	// subdivisions are good for Bullet (so they say...)
	auto plane = PlaneMesh{ {width/2.0, height/2.0}, {widthSegements, heightSegments} };

	for (const MeshVertex& v : plane.vertices()) {
		_vertices.push_back({ vec3(v.position[0], v.position[1], v.position[2]),
							  vec3(v.normal[0], v.normal[1], v.normal[2]),
							  vec2(v.texCoord[0], v.texCoord[1]) });
	}

	for (const Triangle& t : plane.triangles()) {
		_faces.push_back({ unsigned(t.vertices[0]),
						   unsigned(t.vertices[1]),
						   unsigned(t.vertices[2]) });
	}
}

/*********************************************************************************************
 	Public
 *********************************************************************************************/

float Plane::width() const {
	return _width;
}

float Plane::height() const {
	return _height;
}

int Plane::widthSegements() const {
	return _widthSegements;
}

int Plane::heightSegments() const {
	return _heightSegments;
}
