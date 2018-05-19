//
//  GeometryElement.cpp
//	avara-engine
//
//  Created by Morgan Davis on 12/23/16.
//  Copyright © 2016 Morgan K Davis. All rights reserved.
//

#include "GeometryElement.h"

#include <iostream>

#include "Renderer.h"
#include "Types.h"


using namespace ae;
using namespace std;
using namespace glm;


/***************************************************************************************
     Lifecycle
 ***************************************************************************************/

GeometryElement::GeometryElement(std::vector<Vertex>& verticies,
								 std::vector<Face>& faces):
	m_vertices(verticies),
	m_faces(faces),
	m_renderID(0),
	m_dirtyBits(GEOMETRY_ELEMENT_DIRTY_BITS::ALL) {

}

GeometryElement::~GeometryElement() {

}

/***************************************************************************************
     Internal
 ***************************************************************************************/

void GeometryElement::draw(Renderer& renderer,
						   Material& material,
						   const mat4& modelMat,
						   const mat4& viewMat,
						   const mat4& projectionMat,
						   const DEBUG_OPTIONS& debugOptions,
						   RenderStats& stats) {
	
	renderer.render(*this,
					material,
					modelMat, viewMat, projectionMat,
					debugOptions,
					stats);
	
	stats.meshes++;
}

void GeometryElement::hardTransform(const mat4 t, bool norm) {

#warning factor into statis function
	
	for (int v=0; v<m_vertices.size(); ++v) {
		Vertex* vertex = &m_vertices[v];
		vertex->position = vec3(t * vec4(vertex->position, 1.0f));

		if (norm) {
			vertex->normal = normalize(vec3(t * vec4(vertex->normal, 0.0f)));
		}
	}
	
	GEOMETRY_ELEMENT_DIRTY_BITS_ADD(m_dirtyBits, GEOMETRY_ELEMENT_DIRTY_BITS::VERTEX_DATA);
}

const vector<Vertex>& GeometryElement::vertices() const {
	return m_vertices;
}

const vector<Face>& GeometryElement::faces() const {
	return m_faces;
}

GEOMETRY_ELEMENT_ID GeometryElement::renderID() const {
	return m_renderID;
}

void GeometryElement::renderID(GEOMETRY_ELEMENT_ID id) {
	m_renderID = id;
}

GEOMETRY_ELEMENT_DIRTY_BITS GeometryElement::dirtyBits() const {
	return m_dirtyBits;
}

void GeometryElement::dirtyBits(GEOMETRY_ELEMENT_DIRTY_BITS bits) {
	m_dirtyBits = bits;
}
