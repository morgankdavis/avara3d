//
//  Box.cc
//	avara3d
//
//  Created by Morgan Davis on 10/31/17.
//  Copyright © 2017 Morgan K Davis. All rights reserved.
//

#include "a3d/mesh/primitive/Box.h"

#include <memory>
#include <vector>

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

Box::Box(float length, float width, float height,
		 unsigned lengthSegments, unsigned widthSegments, unsigned heightSegments):
		Mesh(vector<shared_ptr<MeshElement>>(), vector<shared_ptr<Material>>()),
		_length(length),
		_width(width),
		_height(height) {

	/// @param size Half of the side length in x (0), y (1) and z (2) direction.
	/// @param segments The number of segments in x (0), y (1) and z (2)

	BoxMesh box{{width/2.0, length/2.0, height/2.0},
				{lengthSegments, widthSegments, heightSegments}};

	auto verts = vector<Vertex>();
	for (const MeshVertex& v : box.vertices()) {
		Vertex vertex = { vec3(v.position[0], v.position[1], v.position[2]),
						  vec3(v.normal[0], v.normal[1], v.normal[2]),
						  vec2(v.texCoord[0], v.texCoord[1]) };
		verts.push_back(vertex);
	}

	auto faces = vector<Face>();
	for (const Triangle& t : box.triangles()) {
		Face face = { unsigned(t.vertices[0]),
					  unsigned(t.vertices[1]),
					  unsigned(t.vertices[2]) };
		faces.push_back(face);
	}
	std::reverse(faces.begin(), faces.end());

	auto element = make_shared<MeshElement>(verts, faces);
	_elements.push_back(element);
}

/*********************************************************************************************
 	Public
 *********************************************************************************************/

float Box::length() const {
	return _length;
}

float Box::width() const {
	return _width;
}

float Box::height() const {
	return _height;
}
