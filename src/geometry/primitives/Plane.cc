//
//  Plane.cc
//	avara-engine
//
//  Created by Morgan Davis on 10/31/17.
//  Copyright © 2017 Morgan K Davis. All rights reserved.
//

#include "ae/geometry/primitives/Plane.h"

#include <memory>
#include <vector>

#include "generator/generator.hpp"

#include "ae/Types.h"
#include "ae/geometry/GeometryElement.h"


using namespace ae;
using namespace generator;
using namespace glm;
using namespace std;


/*********************************************************************************************
	 	Lifecycle
*********************************************************************************************/

Plane::Plane(float width, float height,
			 unsigned widthSegements, unsigned heightSegments):
	Geometry(vector<shared_ptr<GeometryElement>>(), vector<shared_ptr<Material>>()) {

		_width = width;
		_height = height;
		
		/// @param size Half of the side length in x (0) and y (1) direction.
		/// @param segments Number of subdivisions in the x (0) and y (1) direction.

		// subdivisions are good for Bullet (so they say...)
		//PlaneMesh plane{{width/2.0, height/2.0}, {(int)lround(width), (int)lround(height)}};
		PlaneMesh plane{{width/2.0, height/2.0}, {widthSegements, heightSegments}};

		auto verts = vector<Vertex>();
		for (const MeshVertex& v : plane.vertices()) {
			Vertex vertex = { vec3(v.position[0], v.position[1], v.position[2]),
				vec3(v.normal[0], v.normal[1], v.normal[2]),
				vec2(v.texCoord[0], v.texCoord[1]) };
			verts.push_back(vertex);
		}

		auto faces = vector<Face>();
		for (const Triangle& t : plane.triangles()) {
			Face face = { unsigned(t.vertices[0]),
						  unsigned(t.vertices[1]),
						  unsigned(t.vertices[2]) };
			faces.push_back(face);
		}

		auto element = make_shared<GeometryElement>(verts, faces);
		_elements.push_back(element);
		
		//loadVertexData();
}

//Plane::Plane(float width, float height):
//	Geometry(vector<shared_ptr<GeometryElement>>(), vector<shared_ptr<Material>>()) {
//
//		Vertex verts[] = {
//				{ vec3(-width/2.0f, -height/2.0f, 0.0f ), 	vec3(0.0f, 0.0f, 1.0f), 	vec2(0.0f, 1.0f) },
//				{ vec3(-width/2.0f, height/2.0f, 0.0f ), 	vec3(0.0f, 0.0f, 1.0f), 	vec2(0.0f, 1.0f) },
//				{ vec3(width/2.0f, height/2.0f, 0.0f ),		vec3(0.0f, 0.0f, 1.0f), 	vec2(1.0f, 0.0f) },
//				{ vec3(width/2.0f, -height/2.0f, 0.0f ), 	vec3(0.0f, 0.0f, 1.0f), 	vec2(1.0f, 5.0f) }
//		};
//
//		Face faces[] = {
//				{ 2, 1, 0 },
//				{ 0, 3, 2 }
//		};
//
//		auto vertsVector = vector<Vertex>();
//		vertsVector.assign(verts, verts+4);
//
//		auto facesVector = vector<Face>();
//		facesVector.assign(faces, faces+2);
//
//		auto element = make_shared<GeometryElement>(vertsVector, facesVector);
//
//		_elements.push_back(element);
//			
//		loadVertexData();
//}

/*********************************************************************************************
 	Public
 *********************************************************************************************/

float Plane::width() const {
	return _width;
}

float Plane::height() const {
	return _height;
}
