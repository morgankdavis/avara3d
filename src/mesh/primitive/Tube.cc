//
//  Tube.cc
//	avara3d
//
//  Created by Morgan Davis on 11/8/17.
//  Copyright © 2017 Morgan K Davis. All rights reserved.
//

#include "a3d/mesh/primitive/Tube.h"

#include <memory>
#include <vector>

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

Tube::Tube(float innerRadius, float outerRadius, float height, int slices, int segments):
		Mesh(vector<shared_ptr<MeshElement>>(), vector<shared_ptr<Material>>()) {

		/// @param radius The outer radius of the cylinder on the xy-plane.
		/// @param innerRadius The inner radius of the cylinder on the xy-plane.
		/// @param size Half of the length of the cylinder along the z-axis.
		/// @param slices Number nubdivisions around the z-axis.
		/// @param segments Number of subdivisions along the z-axis.
		/// @param rings Number radial subdivisions in the cap.
		/// @param start Counterclockwise angle around the z-axis relative to the x-axis.
		/// @param sweep Counterclockwise angle around the z-axis.

		CappedTubeMesh tube{outerRadius, innerRadius, height/2.0, slices, segments};
		
		auto verts = vector<Vertex>();
		for (const MeshVertex& v : tube.vertices()) {
			Vertex vertex = { vec3(v.position[0], v.position[1], v.position[2]),
				vec3(v.normal[0], v.normal[1], v.normal[2]),
				vec2(v.texCoord[0], v.texCoord[1]) };
			verts.push_back(vertex);
		}
		
		auto faces = vector<Face>();
		for (const Triangle& t : tube.triangles()) {
			Face face = { unsigned(t.vertices[0]),
						  unsigned(t.vertices[1]),
						  unsigned(t.vertices[2]) };
			faces.push_back(face);
		}
		
		auto element = make_shared<MeshElement>(verts, faces);
		_elements.push_back(element);
}
