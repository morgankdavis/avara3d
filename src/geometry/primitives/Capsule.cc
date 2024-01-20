//
//  Capsule.cc
//	avara-engine
//
//  Created by Morgan Davis on 11/8/17.
//  Copyright © 2017 Morgan K Davis. All rights reserved.
//

#include "ae/geometry/primitives/Capsule.h"

#include <memory>
#include <vector>

#include "generator/generator.hpp"
//#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/transform.hpp"

#include "ae/Types.h"
#include "ae/geometry/GeometryElement.h"


using namespace ae;
using namespace generator;
using namespace glm;
using namespace std;


/*********************************************************************************************
	Lifecycle
*********************************************************************************************/

Capsule::Capsule(float radius, float height, int slices, int segments, int rings):
	Geometry(vector<shared_ptr<GeometryElement>>(), vector<shared_ptr<Material>>()) {

		_radius = radius;
		_height = height;
		
//		double radius = 1.0,
//		double size = 0.5,
//		int slices = 32,
//		int segments = 4,
//		int rings = 8,
//		double start = 0.0,
//		double sweep = gml::radians(360.0)
		
		/// @param radius Radius of the capsule on the xy-plane.
		/// @param size Half of the length between centers of the caps along the z-axis.
		/// @param slices Number of subdivisions around the z-axis.
		/// @param rings Number of radial subdivisions in the caps.
		/// @param start Counterclockwise angle relative to the x-axis.
		/// @param sweep Counterclockwise angle.
		
		CapsuleMesh capsule{radius, height/2.0, slices, segments, rings};
		
		auto verts = vector<Vertex>();
		for (const MeshVertex& v : capsule.vertices()) {
			Vertex vertex = { vec3(v.position[0], v.position[1], v.position[2]),
				vec3(v.normal[0], v.normal[1], v.normal[2]),
				vec2(v.texCoord[0], v.texCoord[1]) };
			verts.push_back(vertex);
		}
			
		auto faces = vector<Face>();
		for (const Triangle& t : capsule.triangles()) {
			Face face = { t.vertices[0], t.vertices[1], t.vertices[2] };
			faces.push_back(face);
		}
		
		auto element = make_shared<GeometryElement>(verts, faces);
		_elements.push_back(element);
		
		// this orientation is what bullet expects
		auto xRotation = rotate(mat4(1.0), (float)radians(-90.0), vec3(1.0, 0.0, 0.0));
		burnTransform(xRotation, true);
		
		//loadVertexData();
}

/*********************************************************************************************
 	Public
 *********************************************************************************************/

float Capsule::radius() const {
	return _radius;
}

float Capsule::height() const {
	return _height;
}
