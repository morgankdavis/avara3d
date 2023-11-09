//
//  Torus.cc
//	avara-engine
//
//  Created by Morgan Davis on 11/8/17.
//  Copyright © 2017 Morgan K Davis. All rights reserved.
//

#include "geometry/primitives/Torus.h"

#include <memory>
#include <vector>

#include "generator/generator.hpp"

#include "Types.h"
#include "geometry/GeometryElement.h"


using namespace ae;
using namespace generator;
using namespace glm;
using namespace std;


/*********************************************************************************************
	Lifecycle
*********************************************************************************************/

Torus::Torus(float minorRadius, float majorRadius, int slices, int segments):
	Geometry(vector<shared_ptr<GeometryElement>>(), vector<shared_ptr<Material>>()) {
		
//		double minor = 0.25,
//		double major = 1.0,
//		int slices = 16,
//		int segments = 32,
//		double minorStart = 0.0,
//		double minorSweep = gml::radians(360.0),
//		double majorStart = 0.0,
//		double majorSweep = gml::radians(360.0)
		
		/// @param minor Radius of the minor (inner) ring
		/// @param major Radius of the major (outer) ring
		/// @param slices Subdivisions around the minor ring
		/// @param segments Subdivisions around the major ring
		/// @param minorStart Counterclockwise angle relative to the xy-plane.
		/// @param minorSweep Counterclockwise angle around the circle.
		/// @param majorStart Counterclockwise angle around the z-axis relative to the x-axis.
		/// @param majorSweep Counterclockwise angle around the z-axis.

		TorusMesh torus{minorRadius, majorRadius, slices, segments};
		
		auto verts = vector<Vertex>();
		for (const MeshVertex& v : torus.vertices()) {
			Vertex vertex = { vec3(v.position[0], v.position[1], v.position[2]),
				vec3(v.normal[0], v.normal[1], v.normal[2]),
				vec2(v.texCoord[0], v.texCoord[1]) };
			verts.push_back(vertex);
		}
		
		auto faces = vector<Face>();
		for (const Triangle& t : torus.triangles()) {
			Face face = { t.vertices[0], t.vertices[1], t.vertices[2] };
			faces.push_back(face);
		}
		
		auto element = make_shared<GeometryElement>(verts, faces);
		_elements.push_back(element);
		
		//loadVertexData();
}
