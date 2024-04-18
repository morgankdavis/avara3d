//
//  MeshElement.cc
//  avara3d
//
//  Created by Morgan Davis on 12/23/16.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#include "a3d/mesh/MeshElement.h"

#include <iostream>

#include "a3d/Color.h"
#include "a3d/Types.h"
#include "a3d/diagnostic/logging/Logger.h"
#include "a3d/mesh/Line.h"
#include "a3d/scene/Node.h"
#include "a3d/rendering/renderer/Renderer.h"


using namespace a3d;
using namespace glm;
using namespace std;


/*********************************************************************************************
	Public Lifecycle
 *********************************************************************************************/

MeshElement::MeshElement(const vector<Vertex>& verticies,
						 const vector<Face>& faces):
		MeshElement{} {

	_vertices = verticies;
	_faces = faces;
}

MeshElement::~MeshElement() {
	A3D_LOG_D("Destroying MeshElement {:p}", static_cast<void*>(this));
}

/*********************************************************************************************
	Internal Members
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

	for (auto& vert : _vertices) {
		vert.position = {transform * vec4(vert.position, 1.0f)};

		if (normals) {
			vert.normal = normalize(vec3(transform * vec4(vert.normal, 0.0f)));
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

const vector<Line>& MeshElement::aabbLines() {

	if (A3D_MASK_CONTAINS(_dirtyMask, MeshElementDirtyMask::AABBLines)) {

		A3D_LOG_T("Creating AABB Lines for MeshElement {:p}...", static_cast<void*>(this));

		auto aabb = MeshElement::aabb();

		float xMin = aabb.min.x;
		float xMax = aabb.max.x;
		float yMin = aabb.min.y;
		float yMax = aabb.max.y;
		float zMin = aabb.min.z;
		float zMax = aabb.max.z;

		vec3 one = 		{xMin, yMax, zMin};
		vec3 two =      {xMin, yMax, zMax};
		vec3 three =    {xMax, yMax, zMax};
		vec3 four =     {xMax, yMax, zMin};
		vec3 five =     {xMin, yMin, zMin};
		vec3 six =      {xMin, yMin, zMax};
		vec3 seven =    {xMax, yMin, zMax};
		vec3 eight =    {xMax, yMin, zMin};

		static auto grey = Color{.5f};

		_aabbLines = vector<Line>{
				{one, two, grey},
				{two, three, grey},
				{three, four, grey},
				{four, one, grey},
				{five, six, grey},
				{six, seven, grey},
				{seven, eight, grey},
				{eight, five, grey},
				{one, five, grey},
				{two, six, grey},
				{three, seven, grey},
				{four, eight, grey}
		};

		_dirtyMask = A3D_MASK_REMOVE(_dirtyMask, MeshElementDirtyMask::AABBLines);
	}

	return _aabbLines;
}

MeshElementDirtyMask MeshElement::dirtyMask() const {
	return _dirtyMask;
}

void MeshElement::dirtyMask(MeshElementDirtyMask mask) {
	_dirtyMask = mask;
}

/*********************************************************************************************
	Protected Lifecycle
 *********************************************************************************************/

MeshElement::MeshElement():
		//_vertices{},
		//_faces{},
		_aabbLines{},
		_dirtyMask{MeshElementDirtyMask::All} { }
