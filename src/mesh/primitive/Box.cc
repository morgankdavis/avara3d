//
//  Box.cc
//  avara3d
//
//  Created by Morgan Davis on 10/31/17.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#include "a3d/mesh/primitive/Box.h"

#include <generator/generator.hpp>

#include "a3d/mesh/Mesh.h"
#include "a3d/mesh/MeshElement.h"
#include "a3d/visual/material/Material.h"

using namespace a3d;
using namespace a3d::math;
using namespace std;

/// Pubic Static Members ///

shared_ptr<Mesh> Box::Mesh(float length,
						   float width,
						   float height,
						   unsigned lengthSegments,
						   unsigned widthSegments,
						   unsigned heightSegments,
						   const shared_ptr<Material> material) {


	return make_shared<a3d::Mesh>("Box",
								  make_unique<Box>(length,
												   width,
												   height,
												   lengthSegments,
												   widthSegments,
												   heightSegments),
								  material);
}

/// Public Lifecycle Functions ///

Box::Box(float length,
		 float width,
		 float height,
		 unsigned lengthSegments,
		 unsigned widthSegments,
		 unsigned heightSegments):
		MeshElement{},
		_length{length},
		_width{width},
		_height{height},
		_lengthSegments{lengthSegments},
		_widthSegments{widthSegments},
		_heightSegments{heightSegments} {

	using namespace generator;

	/// @param size Half of the side length in x (0), y (1) and z (2) direction.
	/// @param segments The number of segments in x (0), y (1) and z (2)

	auto box = BoxMesh{ { width/2.0, length/2.0, height/2.0 },
						{ widthSegments, lengthSegments, heightSegments } };

	for (const MeshVertex& v : box.vertices()) {
		_vertices.push_back({ vec3(v.position[0], v.position[1], v.position[2]),
							  vec3(v.normal[0], v.normal[1], v.normal[2]),
							  vec2(v.texCoord[0], v.texCoord[1]) });
	}

	for (const Triangle& t : box.triangles()) {
		_faces.push_back({ unsigned(t.vertices[0]),
						   unsigned(t.vertices[1]),
						   unsigned(t.vertices[2]) });
	}
	std::reverse(_faces.begin(), _faces.end());

	genLocalAABB();
}

/// Public Member Functions ///

float Box::length() const {
	return _length;
}

float Box::width() const {
	return _width;
}

float Box::height() const {
	return _height;
}

unsigned Box::lengthSegments() const {
	return _lengthSegments;
}

unsigned Box::widthSegments() const {
	return _widthSegments;
}

unsigned Box::heightSegments() const {
	return _heightSegments;
}
