//
//  Sphere.cc
//	avara3d
//
//  Created by Morgan Davis on 11/6/17.
//  Copyright © 2017 Morgan K Davis. All rights reserved.
//

#include "a3d/mesh/primitive/Sphere.h"

#include <memory>

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

Sphere::Sphere(float radius, int segments):
		Mesh(vector<shared_ptr<MeshElement>>(), vector<shared_ptr<Material>>()) {

		_radius = radius;
		
		/// @param radius The radius of the containing sphere.
		/// @param segments The number of segments per icosahedron edge. Must be >= 1.
		
		IcoSphereMesh icoSphere{radius, segments};

		auto verts = vector<Vertex>();
		for (const MeshVertex& v : icoSphere.vertices()) {
			Vertex vertex = { vec3(v.position[0], v.position[1], v.position[2]),
							  vec3(v.normal[0], v.normal[1], v.normal[2]),
							  vec2(v.texCoord[0], v.texCoord[1]) };
			verts.push_back(vertex);
		}
		
		auto faces = vector<Face>();
		for (const Triangle& t : icoSphere.triangles()) {
			Face face = { unsigned(t.vertices[0]),
						  unsigned(t.vertices[1]),
						  unsigned(t.vertices[2]) };
			faces.push_back(face);
		}

		auto element = make_shared<MeshElement>(verts, faces);
		_elements.push_back(element);
		
		//loadVertexData();
}

/*********************************************************************************************
 	Public
 *********************************************************************************************/

float Sphere::radius() const {
	return _radius;
}
