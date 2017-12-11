//
//  Tube.cpp
//	avara-engine
//
//  Created by Morgan Davis on 11/8/17.
//  Copyright © 2017 Morgan K Davis. All rights reserved.
//

#include "Tube.h"

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

Tube::Tube(float innerRadius, float outerRadius, float height, int slices, int segments):
	Geometry(vector<shared_ptr<GeometryElement>>(), vector<shared_ptr<Material>>()) {

//		double radius = 1.0,
//		double innerRadius = 0.75,
//		double size = 1.0,
//		int slices = 32,
//		int segments = 8,
//		double start = 0.0,
//		double sweep = gml::radians(360.0)

	CappedTubeMesh tube{outerRadius, innerRadius, height, slices, segments};
	
	auto verts = vector<Vertex>();
	for (const MeshVertex& v : tube.vertices()) {
		Vertex vertex = { vec3(v.position[0], v.position[1], v.position[2]),
			vec3(v.normal[0], v.normal[1], v.normal[2]),
			vec2(v.texCoord[0], v.texCoord[1]) };
		verts.push_back(vertex);
	}
		
	auto faces = vector<Face>();
	for (const Triangle& t : tube.triangles()) {
		Face face = { (unsigned int)t.vertices[0], (unsigned int)t.vertices[1], (unsigned int)t.vertices[2] };
		faces.push_back(face);
	}
	
	auto element = make_shared<GeometryElement>(verts, faces);
	m_elements.push_back(element);
}
