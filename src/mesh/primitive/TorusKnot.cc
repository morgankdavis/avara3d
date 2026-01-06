//
//  TorusKnot.cc
//  avara3d
//
//  Created by Morgan Davis on 3/6/24.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#include "a3d/mesh/primitive/TorusKnot.h"

#include <generator/generator.hpp>

#include "a3d/mesh/Mesh.h"
#include "a3d/mesh/MeshElement.h"
#include "a3d/visual/material/Material.h"

using namespace a3d;
using namespace a3d::math;
using namespace std;

/// Pubic Static Members ///

shared_ptr<Mesh> TorusKnot::Mesh(unsigned p,
								 unsigned q,
								 unsigned slices,
								 unsigned segments,
								 const shared_ptr<Material> material) {

	return make_shared<a3d::Mesh>("TorusKnot", make_unique<TorusKnot>(p,
																	  q,
																	  slices,
																	  segments),
								  material);
}

/// Public Lifecycle Functions ///

TorusKnot::TorusKnot(unsigned p,
					 unsigned q,
					 unsigned slices,
					 unsigned segments):
		MeshElement{},
		_p{p},
		_q{q},
		_slices{slices},
		_segments{segments} {

	using namespace generator;

	// p & q are mysteries!

	/// @param slices Number subdivisions around the circle.
	/// @param segments Number of subdivisions around the path.

	auto torusKnot = TorusKnotMesh{(int)p, (int)q, (int)slices, (int)segments};

//	for (const MeshVertex& v : torusKnot.vertices()) {
//		_vertices.push_back({ vec3(v.position[0], v.position[1], v.position[2]),
//							  vec3(v.normal[0], v.normal[1], v.normal[2]),
//							  vec2(v.texCoord[0], v.texCoord[1]) });
//	}
//
//	for (const Triangle& t : torusKnot.triangles()) {
//		_faces.push_back({ unsigned(t.vertices[0]),
//						   unsigned(t.vertices[1]),
//						   unsigned(t.vertices[2]) });
//	}
//
//	genLocalAABB();



	std::vector<Vertex> verts;
//	verts.reserve(box.vertices().size());

	for (const MeshVertex& v : torusKnot.vertices()) {
		verts.push_back({ vec3(v.position[0], v.position[1], v.position[2]),
						  vec3(v.normal[0],   v.normal[1],   v.normal[2]),
						  vec2(v.texCoord[0], v.texCoord[1]) });
	}

	std::vector<Face> faces;
//	faces.reserve(box.triangles().size());
	for (const Triangle& t : torusKnot.triangles()) {
		faces.push_back({ (uint32_t)t.vertices[0],
						  (uint32_t)t.vertices[1],
						  (uint32_t)t.vertices[2] });
	}

	setVertices(VertexLayout::PNT, verts);
	setFaces(faces);

	genLocalAABB();
}

/// Public Member Functions ///

unsigned TorusKnot::p() const {
	return _p;
}

unsigned TorusKnot::q() const {
	return _q;
}

unsigned TorusKnot::slices() const {
	return _slices;
}

unsigned TorusKnot::segments() const {
	return _segments;
}