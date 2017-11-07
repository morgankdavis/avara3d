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

#include <glm/glm.hpp>

#include "GeometryElement.h"
#include "Types.h"


using namespace ae;
using namespace std;
using namespace glm;


/***************************************************************************************
	    	 MARK:   Lifecycle
**************************************************************************************/

Box::Box(float width, float height, float length):
	Geometry(vector<shared_ptr<GeometryElement>>(), vector<shared_ptr<Material>>()) {

	Vertex verts[] = {
			{ vec3(-width/2.0f, -height/2.0f, length/2.0f ), 	vec3(0.0f, 0.0f, 0.0f), 	vec2(0.0f, 0.0f) },
			{ vec3(-width/2.0f, height/2.0f, length/2.0f ), 	vec3(0.0f, 0.0f, 0.0f), 	vec2(0.0f, 0.0f) },
			{ vec3(width/2.0f, height/2.0f, length/2.0f ), 		vec3(0.0f, 0.0f, 0.0f), 	vec2(0.0f, 0.0f) },
			{ vec3(width/2.0f, -height/2.0f, length/2.0f ), 	vec3(0.0f, 0.0f, 0.0f), 	vec2(0.0f, 0.0f) },

			{ vec3(-width/2.0f, -height/2.0f, -length/2.0f ), 	vec3(0.0f, 0.0f, 0.0f), 	vec2(0.0f, 0.0f) },
			{ vec3(-width/2.0f, height/2.0f, -length/2.0f ), 	vec3(0.0f, 0.0f, 0.0f), 	vec2(0.0f, 0.0f) },
			{ vec3(width/2.0f, height/2.0f, -length/2.0f ), 	vec3(0.0f, 0.0f, 0.0f), 	vec2(0.0f, 0.0f) },
			{ vec3(width/2.0f, -height/2.0f, -length/2.0f ), 	vec3(0.0f, 0.0f, 0.0f), 	vec2(0.0f, 0.0f) }
	};

	Face faces[] = {
//			// front
//			{ 0, 1, 2 },
//			{ 2, 3, 0 },
//			// back
//			{ 7, 6, 5 },
//			{ 5, 4, 7 },
//			// left
//			{ 4, 5, 1 },
//			{ 1, 0, 4 },
//			// right
//			{ 3, 2, 6 },
//			{ 6, 7, 3 },
//			// top
//			{ 1, 5, 6 },
//			{ 6, 2, 1 },
//			// bottom
//			{ 4, 0, 3 },
//			{ 3, 7, 4 },

			// front
			{ 2, 1, 0 },
			{ 0, 3, 2 },
			// back
			{ 5, 6, 7 },
			{ 7, 4, 5 },
			// left
			{ 1, 5, 4 },
			{ 4, 0, 1 },
			// right
			{ 6, 2, 3 },
			{ 3, 7, 6 },
			// top
			{ 6, 5, 1 },
			{ 1, 2, 6 },
			// bottom
			{ 3, 0, 4 },
			{ 4, 7, 3 },
	};

	auto vertsVector = vector<Vertex>();
	vertsVector.assign(verts, verts+8);

	auto facesVector = vector<Face>();
	facesVector.assign(faces, faces+12);

	auto element = make_shared<GeometryElement>(vertsVector, facesVector);

	m_elements.push_back(element);

	generateFlatNormals();
}
