//
//  Cylinder.cc
//	avara3d
//
//  Created by Morgan Davis on 11/8/17.
//  Copyright © 2017 Morgan K Davis. All rights reserved.
//

#include "a3d/mesh/primitive/Cylinder.h"

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

Cylinder::Cylinder(float radius, float height, int slices, int segments):
		Mesh(vector<shared_ptr<MeshElement>>(), vector<shared_ptr<Material>>()) {

		_radius = radius;
		_height = height;
		
//		double radius = 1.0,
//		double size = 1.0,
//		int slices = 32,
//		int segments = 8,
//		double start = 0.0,
//		double sweep = gml::radians(360.0)
		
		/// @param radius Radius of the cylinder along the xy-plane.
		/// @param size Half of the length of the cylinder along the z-axis.
		/// @param slices Subdivisions around the z-axis.
		/// @param segments Subdivisions along the z-axis.
		/// @param start Counterclockwise angle around the z-axis relative to the x-axis.
		/// @param sweep Counterclockwise angle around the z-axis.
		
		CappedCylinderMesh cylinder{radius, height/2.0, slices, segments};
		
		auto verts = vector<Vertex>();
		for (const MeshVertex& v : cylinder.vertices()) {
			Vertex vertex = { vec3(v.position[0], v.position[1], v.position[2]),
				vec3(v.normal[0], v.normal[1], v.normal[2]),
				vec2(v.texCoord[0], v.texCoord[1]) };
			verts.push_back(vertex);
		}
			
		auto faces = vector<Face>();
		for (const Triangle& t : cylinder.triangles()) {
			Face face = { unsigned(t.vertices[0]),
						  unsigned(t.vertices[1]),
						  unsigned(t.vertices[2]) };
			faces.push_back(face);
		}
		
		auto element = make_shared<MeshElement>(verts, faces);
		_elements.push_back(element);
		
		// this orientation is what bullet expects
		auto xRotation = rotate(mat4(1.0), (float)radians(-90.0), vec3(1.0, 0.0, 0.0));
		burnTransform(xRotation, true);
		
		//loadVertexData();
}

/*********************************************************************************************
	Public
 *********************************************************************************************/

float Cylinder::radius() const {
	return _radius;
}

float Cylinder::height() const {
	return _height;
}
