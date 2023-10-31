//
//  GeometryElement.cc
//	avara-engine
//
//  Created by Morgan Davis on 12/23/16.
//  Copyright © 2016 Morgan K Davis. All rights reserved.
//

#include "GeometryElement.h"

#include <iostream>

#include "Logger.h"
#include "Renderer.h"
#include "Types.h"


using namespace ae;
using namespace std;
using namespace glm;


/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

GeometryElement::GeometryElement(std::vector<Vertex>& verticies,
								 std::vector<Face>& faces):
	_vertices(verticies),
	_faces(faces),
	_dirtyBits(GEOMETRY_ELEMENT_DIRTY_BITS::ALL) {
		
}

GeometryElement::~GeometryElement() {
	AE_LOG_D("Destroying GeometryElement {:p}", (void*)this);
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
						   RenderStats& stats) {
	
	renderer.render(shared_from_this(),
					material,
					modelMat, viewMat, projectionMat,
					debugOptions,
					stats);
	
	stats.meshes++;
}

void GeometryElement::burnTransform(const mat4& transform, bool normals) {
	for (int v=0; v<_vertices.size(); ++v) {
		Vertex* vertex = &_vertices[v];
		vertex->position = vec3(transform * vec4(vertex->position, 1.0f));

		if (normals) {
			vertex->normal = normalize(vec3(transform * vec4(vertex->normal, 0.0f)));
		}
	}
	
	GEOMETRY_ELEMENT_DIRTY_BITS_ADD(_dirtyBits, GEOMETRY_ELEMENT_DIRTY_BITS::VERTEX_DATA);
}

const vector<Vertex>& GeometryElement::vertices() const {
	return _vertices;
}

const vector<Face>& GeometryElement::faces() const {
	return _faces;
}

std::vector<Vertex>* GeometryElement::verticesPtr() {
	return &_vertices;
}

std::vector<Face>* GeometryElement::facesPtr() {
	return &_faces;
}

shared_ptr<std::vector<Vertex>> GeometryElement::verticesSPtr() {
	auto shared = make_shared<vector<Vertex>>();
	shared->reserve(_vertices.size());
	shared->insert(shared->end(), _vertices.begin(), _vertices.end());
	return shared;
}

shared_ptr<std::vector<Face>> GeometryElement::facesSPtr() {
	auto shared = make_shared<vector<Face>>();
	shared->reserve(_faces.size());
	shared->insert(shared->end(), _faces.begin(), _faces.end());
	return shared;
}

//const std::shared_ptr<std::vector<Vertex>>& GeometryElement::vertices() const {
//	return _vertices;
//}
//
//const std::shared_ptr<std::vector<Face>>& GeometryElement::faces() const {
//	return _faces;
//}

GEOMETRY_ELEMENT_DIRTY_BITS GeometryElement::dirtyBits() const {
	return _dirtyBits;
}

void GeometryElement::dirtyBits(GEOMETRY_ELEMENT_DIRTY_BITS bits) {
	_dirtyBits = bits;
}
