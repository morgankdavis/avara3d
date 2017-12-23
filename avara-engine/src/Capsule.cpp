//
//  Capsule.cpp
//	avara-engine
//
//  Created by Morgan Davis on 11/8/17.
//  Copyright © 2017 Morgan K Davis. All rights reserved.
//

#include "Capsule.h"

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

Capsule::Capsule(float radius, float height, int slices, int segments, int rings):
	Geometry(vector<shared_ptr<GeometryElement>>(), vector<shared_ptr<Material>>()) {

//		double radius = 1.0,
//		double size = 0.5,
//		int slices = 32,
//		int segments = 4,
//		int rings = 8,
//		double start = 0.0,
//		double sweep = gml::radians(360.0)
		
		CapsuleMesh capsule{radius, height, slices, segments, rings};
		
		auto verts = vector<Vertex>();
		for (const MeshVertex& v : capsule.vertices()) {
			Vertex vertex = { vec3(v.position[0], v.position[1], v.position[2]),
				vec3(v.normal[0], v.normal[1], v.normal[2]),
				vec2(v.texCoord[0], v.texCoord[1]) };
			verts.push_back(vertex);
		}
			
		auto faces = vector<Face>();
		for (const Triangle& t : capsule.triangles()) {
			Face face = { (unsigned int)t.vertices[0], (unsigned int)t.vertices[1], (unsigned int)t.vertices[2] };
			faces.push_back(face);
		}
		
		auto element = make_shared<GeometryElement>(verts, faces);
		m_elements.push_back(element);
		
		loadVertexData();
}
