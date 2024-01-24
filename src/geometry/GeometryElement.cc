//
//  GeometryElement.cc
//	avara-engine
//
//  Created by Morgan Davis on 12/23/16.
//  Copyright © 2016 Morgan K Davis. All rights reserved.
//

#include "geometry/GeometryElement.h"

#include <iostream>

#include "Types.h"
#include "diagnostic/logging/Logger.h"
#include "scene/Node.h"
#include "rendering/Renderer.h"


using namespace ae;
using namespace std;
using namespace glm;


/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

GeometryElement::GeometryElement(const vector<Vertex>& verticies,
								 const vector<Face>& faces):
		_vertices(verticies),
		_faces(faces),
		_dirtyMask(GEOMETRY_ELEMENT_DIRTY_MASK::ALL) {
		
}

GeometryElement::~GeometryElement() {
	AE_LOG_D("Destroying GeometryElement {:p}", static_cast<void*>(this));
}

/*********************************************************************************************
	Internal
 *********************************************************************************************/

void GeometryElement::draw(Renderer& renderer,
						   Material& material,
						   const mat4& modelMat,
						   const mat4& viewMat,
						   const mat4& projectionMat,
						   const DEBUG_OPTIONS& debugOptions,
						   Stats& stats) {
	
	renderer.render(shared_from_this(),
					material,
					modelMat,
					viewMat,
					projectionMat,
					debugOptions,
					stats);

	++stats.meshes;
	stats.polygons += _faces.size();
}

void GeometryElement::burnTransform(const mat4& transform, bool normals) {
	for (int v=0; v<_vertices.size(); ++v) {
		Vertex* vertex = &_vertices[v];
		vertex->position = vec3(transform * vec4(vertex->position, 1.0f));

		if (normals) {
			vertex->normal = normalize(vec3(transform * vec4(vertex->normal, 0.0f)));
		}
	}
	
	GEOMETRY_ELEMENT_DIRTY_MASK_ADD(_dirtyMask, GEOMETRY_ELEMENT_DIRTY_MASK::VERTEX_DATA);
}

const vector<Vertex>& GeometryElement::vertices() const {
	return _vertices;
}

const vector<Face>& GeometryElement::faces() const {
	return _faces;
}

AABB GeometryElement::aabb(const std::shared_ptr<Node> convertToNode) const {

	static const float maxFloat = numeric_limits<float>::max();
	static const float minFloat = numeric_limits<float>::min();

	AABB aabb = { {maxFloat, maxFloat, maxFloat},
				  {minFloat, minFloat, minFloat} };

	const auto nodeWorldTransform = (convertToNode
									 ? convertToNode->worldTransform()
									 : mat4(1.0));

		for (const auto& v : vertices()) {
			vec3 p = (convertToNode
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

glm::vec3 GeometryElement::extent(const std::shared_ptr<Node> convertToNode) const {
	auto aabb = GeometryElement::aabb(convertToNode);
	return {aabb.max.x - aabb.min.x,
			aabb.max.y - aabb.min.y,
			aabb.max.z - aabb.min.z};
}

GEOMETRY_ELEMENT_DIRTY_MASK GeometryElement::dirtyMask() const {
	return _dirtyMask;
}

void GeometryElement::dirtyMask(GEOMETRY_ELEMENT_DIRTY_MASK mask) {
	_dirtyMask = mask;
}
