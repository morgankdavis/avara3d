//
//  Plane.cpp
//	avara-engine
//
//  Created by Morgan Davis on 10/31/17.
//  Copyright © 2017 Morgan K Davis. All rights reserved.
//

#include "Plane.h"

#include <memory>
#include <vector>

#include <generator/generator.hpp>

#include "GeometryElement.h"
#include "Types.h"


using namespace ae;
using namespace generator;
using namespace glm;
using namespace std;


/***************************************************************************************
	    	 MARK:   Lifecycle
**************************************************************************************/

Plane::Plane(float width, float height):
	Geometry(vector<shared_ptr<GeometryElement>>(), vector<shared_ptr<Material>>()) {

		/// @param size Half of the side length in x (0) and y (1) direction.
		/// @param segments Number of subdivisions in the x (0) and y (1) direction.

		PlaneMesh plane{{width, height}, {1, 1}};

		auto verts = vector<Vertex>();
		for (const MeshVertex& v : plane.vertices()) {
			Vertex vertex = { vec3(v.position[0], v.position[1], v.position[2]),
				vec3(v.normal[0], v.normal[1], v.normal[2]),
				vec2(v.texCoord[0], v.texCoord[1]) };
			verts.push_back(vertex);
		}

		auto faces = vector<Face>();
		for (const Triangle& t : plane.triangles()) {
			Face face = { (unsigned int)t.vertices[0], (unsigned int)t.vertices[1], (unsigned int)t.vertices[2] };
			faces.push_back(face);
		}

		auto element = make_shared<GeometryElement>(verts, faces);
		m_elements.push_back(element);
}

//Plane::Plane(float width, float height):
//	Geometry(vector<shared_ptr<GeometryElement>>(), vector<shared_ptr<Material>>()) {
//
//	Vertex verts[] = {
//			{ vec3(-width/2.0f, -height/2.0f, 0.0f ), 	vec3(0.0f, 0.0f, 1.0f), 	vec2(0.0f, 5.0f) },
//			{ vec3(-width/2.0f, height/2.0f, 0.0f ), 	vec3(0.0f, 0.0f, 1.0f), 	vec2(0.0f, 0.0f) },
//			{ vec3(width/2.0f, height/2.0f, 0.0f ),		vec3(0.0f, 0.0f, 1.0f), 	vec2(5.0f, 0.0f) },
//			{ vec3(width/2.0f, -height/2.0f, 0.0f ), 	vec3(0.0f, 0.0f, 1.0f), 	vec2(5.0f, 5.0f) }
//	};
//
//	Face faces[] = {
//			{ 2, 1, 0 },
//			{ 0, 3, 2 }
//	};
//
//	auto vertsVector = vector<Vertex>();
//	vertsVector.assign(verts, verts+4);
//
//	auto facesVector = vector<Face>();
//	facesVector.assign(faces, faces+2);
//
//	auto element = make_shared<GeometryElement>(vertsVector, facesVector);
//
//	m_elements.push_back(element);
//}

