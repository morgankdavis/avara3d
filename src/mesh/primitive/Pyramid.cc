//
//  Pyramid.cc
//	avara3d
//
//  Created by Morgan Davis on 11/8/17.
//  Copyright © 2017 Morgan K Davis. All rights reserved.
//

#include "a3d/mesh/primitive/Pyramid.h"

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

Pyramid::Pyramid(float width, float height):
		Mesh(vector<shared_ptr<MeshElement>>(), vector<shared_ptr<Material>>()) {

//	PyramidMesh plane{{width, height}, {1, 1}};
//
//	auto verts = vector<Vertex>();
//	for (const MeshVertex& v : plane.vertices()) {
//		Vertex vertex = { vec3(v.position[0], v.position[1], v.position[2]),
//			vec3(v.normal[0], v.normal[1], v.normal[2]),
//			vec2(v.texCoord[0], v.texCoord[1])};
//		verts.push_back(vertex);
//	}
//
//	auto faces = vector<Face>();
//	for (const Triangle& t : plane.triangles()) {
//		Face face = { (unsigned int)t.vertices[0], (unsigned int)t.vertices[1], (unsigned int)t.vertices[2]};
//		faces.push_back(face);
//	}
//
//	auto element = make_shared<GeometryElement>(verts, faces);
//	_elements.push_back(element);
}
