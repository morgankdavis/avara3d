//
//  Box.cpp
//	avara-engine
//
//  Created by Morgan Davis on 10/31/17.
//  Copyright © 2017 Morgan K Davis. All rights reserved.
//

#include "Box.h"

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
	    	 Lifecycle
***************************************************************************************/

Box::Box(float width, float height, float length):
	Geometry(vector<shared_ptr<GeometryElement>>(), vector<shared_ptr<Material>>()) {
		
		m_width = width;
		m_height = height;
		m_length = length;
	
		/// @param size Half of the side length in x (0), y (1) and z (2) direction.
		/// @param segments The number of segments in x (0), y (1) and z (2)
		
		BoxMesh box{{width/2.0, height/2.0, length/2.0}, {1, 1, 1}};
		//BoxMesh box{{width/2.0, height/2.0, length/2.0}, {(int)lround(width), (int)lround(height), (int)lround(length)}};
		
		auto verts = vector<Vertex>();
		for (const MeshVertex& v : box.vertices()) {
			Vertex vertex = { vec3(v.position[0], v.position[1], v.position[2]),
				vec3(v.normal[0], v.normal[1], v.normal[2]),
				vec2(v.texCoord[0], v.texCoord[1]) };
			verts.push_back(vertex);
		}
		
		auto faces = vector<Face>();
		for (const Triangle& t : box.triangles()) {
			Face face = { (unsigned int)t.vertices[0], (unsigned int)t.vertices[1], (unsigned int)t.vertices[2] };
			faces.push_back(face);
		}
		
		auto element = make_shared<GeometryElement>(verts, faces);
		m_elements.push_back(element);
		
		//loadVertexData();
}

//Box::Box(float width, float height, float length):
//	Geometry(vector<shared_ptr<GeometryElement>>(), vector<shared_ptr<Material>>()) {
//
//	Vertex verts[] = {
//			{ vec3(-width/2.0f, -height/2.0f, length/2.0f ), 	vec3(0.0f, 0.0f, 0.0f), 	vec2(0.0f, 0.0f) },
//			{ vec3(-width/2.0f, height/2.0f, length/2.0f ), 	vec3(0.0f, 0.0f, 0.0f), 	vec2(0.0f, 0.0f) },
//			{ vec3(width/2.0f, height/2.0f, length/2.0f ), 		vec3(0.0f, 0.0f, 0.0f), 	vec2(0.0f, 0.0f) },
//			{ vec3(width/2.0f, -height/2.0f, length/2.0f ), 	vec3(0.0f, 0.0f, 0.0f), 	vec2(0.0f, 0.0f) },
//
//			{ vec3(-width/2.0f, -height/2.0f, -length/2.0f ), 	vec3(0.0f, 0.0f, 0.0f), 	vec2(0.0f, 0.0f) },
//			{ vec3(-width/2.0f, height/2.0f, -length/2.0f ), 	vec3(0.0f, 0.0f, 0.0f), 	vec2(0.0f, 0.0f) },
//			{ vec3(width/2.0f, height/2.0f, -length/2.0f ), 	vec3(0.0f, 0.0f, 0.0f), 	vec2(0.0f, 0.0f) },
//			{ vec3(width/2.0f, -height/2.0f, -length/2.0f ), 	vec3(0.0f, 0.0f, 0.0f), 	vec2(0.0f, 0.0f) }
//	};
//
//	Face faces[] = {
//			// front
//			{ 2, 1, 0 },
//			{ 0, 3, 2 },
//			// back
//			{ 5, 6, 7 },
//			{ 7, 4, 5 },
//			// left
//			{ 1, 5, 4 },
//			{ 4, 0, 1 },
//			// right
//			{ 6, 2, 3 },
//			{ 3, 7, 6 },
//			// top
//			{ 6, 5, 1 },
//			{ 1, 2, 6 },
//			// bottom
//			{ 3, 0, 4 },
//			{ 4, 7, 3 },
//	};
//
//	auto vertsVector = vector<Vertex>();
//	vertsVector.assign(verts, verts+8);
//
//	auto facesVector = vector<Face>();
//	facesVector.assign(faces, faces+12);
//
//	auto element = make_shared<GeometryElement>(vertsVector, facesVector);
//
//	m_elements.push_back(element);
//
//	generateFlatNormals();
//}

/***************************************************************************************
    	 Public
 ***************************************************************************************/

float Box::width() const {
	return m_width;
}

float Box::height() const {
	return m_height;
}

float Box::length() const {
	return m_length;
}
