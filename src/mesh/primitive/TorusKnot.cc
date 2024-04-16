//
//  TorusKnot.cc
//  avara3d
//
//  Created by Morgan Davis on 3/6/24.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#include "a3d/mesh/primitive/TorusKnot.h"

#include "generator/generator.hpp"
#include "glm/glm.hpp"

#include "a3d/mesh/Mesh.h"
#include "a3d/mesh/MeshElement.h"
#include "a3d/rendering/material/Material.h"


using namespace a3d;
using namespace generator;
using namespace glm;
using namespace std;


shared_ptr<Mesh> TorusKnot::Mesh(int p,
								 int q,
								 int slices,
								 int segments,
								 const shared_ptr<Material> material) {

	return make_shared<a3d::Mesh>("TorusKnot", make_unique<TorusKnot>(p,
																	  q,
																	  slices,
																	  segments),
								  material);
}

/*********************************************************************************************
	Lifecycle
*********************************************************************************************/

TorusKnot::TorusKnot(int p,
					 int q,
					 int slices,
					 int segments):
		MeshElement{},
		_p{p},
		_q{q},
		_slices{slices},
		_segments{segments} {

	// p & q are mysteries!

	/// @param slices Number subdivisions around the circle.
	/// @param segments Number of subdivisions around the path.

	auto torusKnot = TorusKnotMesh{p, q, slices, segments};

	for (const MeshVertex& v : torusKnot.vertices()) {
		_vertices.push_back({ vec3(v.position[0], v.position[1], v.position[2]),
							  vec3(v.normal[0], v.normal[1], v.normal[2]),
							  vec2(v.texCoord[0], v.texCoord[1]) });
	}

	for (const Triangle& t : torusKnot.triangles()) {
		_faces.push_back({ unsigned(t.vertices[0]),
						   unsigned(t.vertices[1]),
						   unsigned(t.vertices[2]) });
	}
}

/*********************************************************************************************
 	Public
 *********************************************************************************************/

int TorusKnot::p() const {
	return _p;
}

int TorusKnot::q() const {
	return _q;
}

int TorusKnot::slices() const {
	return _slices;
}

int TorusKnot::segments() const {
	return _segments;
}