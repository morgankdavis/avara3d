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
			{ 0, 1, 2 },
			{ 2, 3, 0 }
	};

	auto vertsVector = vector<Vertex>();
	vertsVector.assign(verts, verts+4);

	auto facesVector = vector<Face>();
	facesVector.assign(faces, faces+4);

	auto element = make_shared<GeometryElement>(vertsVector, facesVector);

	m_elements.push_back(element);

	//auto elements = vector<shared_ptr<GeometryElement>>();
	//auto materials = vector<shared_ptr<Material>>();

	//elements->push_back(element);

	// c-array to vector: w_.assign(w, w + len);

	//Geometry(elements, materials);

//	Geometry(const std::vector<std::shared_ptr<GeometryElement>> elements,
//	const std::vector<std::shared_ptr<Material>> materials);
}
