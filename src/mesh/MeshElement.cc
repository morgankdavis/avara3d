//
//  MeshElement.cc
//	avara3d
//
//  Created by Morgan Davis on 12/23/16.
//  Copyright © 2016 Morgan K Davis. All rights reserved.
//

#include "a3d/mesh/MeshElement.h"

#include <iostream>

#include "a3d/Types.h"
#include "a3d/diagnostic/logging/Logger.h"
#include "a3d/scene/Node.h"
#include "a3d/rendering/Renderer.h"


using namespace a3d;
using namespace std;
using namespace glm;


/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

MeshElement::MeshElement(const vector<Vertex>& verticies,
						 const vector<Face>& faces):
		MeshElement() {

	_vertices = verticies;
	_faces = faces;
}

MeshElement::~MeshElement() {
	A3D_LOG_D("Destroying MeshElement {:p}", static_cast<void*>(this));
}

/*********************************************************************************************
	Internal
 *********************************************************************************************/

void MeshElement::draw(Renderer& renderer,
					   Material& material,
					   const mat4& modelMat,
					   const mat4& viewMat,
					   const mat4& projectionMat,
					   const DebugOptions& debugOptions,
					   Stats& stats) {
	
	renderer.render(*this,
					material,
					modelMat,
					viewMat,
					projectionMat,
					debugOptions,
					stats);

	stats.elements++;
	stats.polygons += _faces.size();
}

void MeshElement::burnTransform(const mat4& transform, bool normals) {
	for (int v=0; v<_vertices.size(); ++v) {
		Vertex* vertex = &_vertices[v];
		vertex->position = vec3(transform * vec4(vertex->position, 1.0f));

		if (normals) {
			vertex->normal = normalize(vec3(transform * vec4(vertex->normal, 0.0f)));
		}
	}
	
	A3D_MASK_ADD(_dirtyMask, MeshElementDirtyMask::VertexData);
}

const vector<Vertex>& MeshElement::vertices() const {
	return _vertices;
}

const vector<Face>& MeshElement::faces() const {
	return _faces;
}

AABB MeshElement::aabb(const Node* convertTo) const {

	static const float maxFloat = numeric_limits<float>::max();
	static const float minFloat = numeric_limits<float>::min();

	AABB aabb = { {maxFloat, maxFloat, maxFloat},
				  {minFloat, minFloat, minFloat} };

	const auto nodeWorldTransform = (convertTo
									 ? convertTo->worldTransform()
									 : mat4(1.0));

	for (const auto& v : vertices()) {
		vec3 p = (convertTo
				  ? vec3(nodeWorldTransform * vec4(v.position, 1.0f))
				  : v.position);
		aabb.min.x = std::min(aabb.min.x, p.x);
		aabb.max.x = std::max(aabb.max.x, p.x);
		aabb.min.y = std::min(aabb.min.y, p.y);
		aabb.max.y = std::max(aabb.max.y, p.y);
		aabb.min.z = std::min(aabb.min.z, p.z);
		aabb.max.z = std::max(aabb.max.z, p.z);
	}

	return aabb;
}

glm::vec3 MeshElement::extent(const Node* convertTo) const {
	auto aabb = MeshElement::aabb(convertTo);
	return { aabb.max.x - aabb.min.x,
			 aabb.max.y - aabb.min.y,
			 aabb.max.z - aabb.min.z };
}

MeshElementDirtyMask MeshElement::dirtyMask() const {
	return _dirtyMask;
}

void MeshElement::dirtyMask(MeshElementDirtyMask mask) {
	_dirtyMask = mask;
}

/*********************************************************************************************
	Protected
 *********************************************************************************************/

MeshElement::MeshElement():
		_dirtyMask(MeshElementDirtyMask::All) { }
