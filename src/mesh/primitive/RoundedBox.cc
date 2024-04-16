//
//  RoundedBox.cc
//	avara3d
//
//  Created by Morgan Davis on 3/6/24.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#include "a3d/mesh/primitive/RoundedBox.h"

#include "generator/generator.hpp"
#include "glm/glm.hpp"

#include "a3d/mesh/Mesh.h"
#include "a3d/mesh/MeshElement.h"
#include "a3d/rendering/material/Material.h"


using namespace a3d;
using namespace generator;
using namespace glm;
using namespace std;


shared_ptr<Mesh> RoundedBox::Mesh(float radius,
								  float length,
								  float width,
								  float height,
								  int slices,
								  int lengthSegments,
								  int widthSegments,
								  int heightSegments,
								  const shared_ptr<Material> material) {

	return make_shared<a3d::Mesh>("RoundedBox",
								  make_unique<RoundedBox>(radius,
														  length,
														  width,
														  height,
														  slices,
														  lengthSegments,
														  widthSegments,
														  heightSegments),
								  material);
}

/*********************************************************************************************
	Lifecycle
*********************************************************************************************/

RoundedBox::RoundedBox(float radius,
					   float length,
					   float width,
					   float height,
					   int slices,
					   int lengthSegments,
					   int widthSegments,
					   int heightSegments):
		MeshElement{},
		_radius{radius},
		_length{length},
		_width{width},
		_height{height},
		_slices{slices},
		_lengthSegments{lengthSegments},
		_widthSegments{widthSegments},
		_heightSegments{heightSegments} {

	/// @param radius Radius of the rounded edges.
	/// @param size Half of the side length in x (0), y (1) and z (2) direction.
	/// @param slices Number subdivions around in the rounded edges.
	/// @param segments Number of subdivisons in x (0), y (1) and z (2)
	/// direction for the flat faces.

	auto roundedBox = RoundedBoxMesh{radius, { width/2.0, length/2.0, height/2.0 },
									 slices, { widthSegments, lengthSegments, heightSegments }};

	for (const MeshVertex& v : roundedBox.vertices()) {
		_vertices.push_back({ vec3(v.position[0], v.position[1], v.position[2]),
							  vec3(v.normal[0], v.normal[1], v.normal[2]),
							  vec2(v.texCoord[0], v.texCoord[1]) });
	}

	for (const Triangle& t : roundedBox.triangles()) {
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

float RoundedBox::radius() const {
	return _radius;
}

float RoundedBox::length() const {
	return _length;
}

float RoundedBox::width() const {
	return _width;
}

float RoundedBox::height() const {
	return _height;
}

int RoundedBox::slices() const {
	return _slices;
}

int RoundedBox::lengthSegments() const {
	return _lengthSegments;
}

int RoundedBox::widthSegments() const {
	return _widthSegments;
}

int RoundedBox::heightSegments() const {
	return _heightSegments;
}
