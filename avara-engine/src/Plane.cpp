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

#include <glm/glm.hpp>

#include "GeometryElement.h"
#include "Types.h"


using namespace ae;
using namespace std;
using namespace glm;


/***************************************************************************************
	    	 MARK:   Lifecycle
**************************************************************************************/

Plane::Plane(float width, float height):
	Geometry(vector<shared_ptr<GeometryElement>>(), vector<shared_ptr<Material>>()) {

	Vertex verts[] = {
			{ vec3(-width/2.0f, -height/2.0f, 0.0f ), 	vec3(0.0f, 0.0f, 0.0f), 	vec2(0.0f, 0.0f) },
			{ vec3(-width/2.0f, height/2.0f, 0.0f ), 	vec3(0.0f, 0.0f, 0.0f), 	vec2(0.0f, 0.0f) },
			{ vec3(width/2.0f, height/2.0f, 0.0f ),		vec3(0.0f, 0.0f, 0.0f), 	vec2(0.0f, 0.0f) },
			{ vec3(width/2.0f, -height/2.0f, 0.0f ), 	vec3(0.0f, 0.0f, 0.0f), 	vec2(0.0f, 0.0f) }
	};

	Face faces[] = {
//			{ 0, 1, 2 },
//			{ 2, 3, 0 }
			{ 2, 1, 0 },
			{ 0, 3, 2 }
	};

	auto vertsVector = vector<Vertex>();
	vertsVector.assign(verts, verts+4);

	auto facesVector = vector<Face>();
	facesVector.assign(faces, faces+2);

	auto element = make_shared<GeometryElement>(vertsVector, facesVector);

	m_elements.push_back(element);

	generateFlatNormals();
}
