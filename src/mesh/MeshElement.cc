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
#include "a3d/diagnostic/log/Log.h"
#include "a3d/Math.h"
#include "a3d/mesh/Line.h"
#include "a3d/scene/Node.h"
#include "a3d/rendering/RenderItem.h"
#include "a3d/rendering/renderer/Renderer.h"

using namespace a3d;
using namespace a3d::math;
using namespace std;

/// Public Lifecycle Functions ///

MeshElement::MeshElement(const vector<Vertex>& verticies,
						 const vector<Face>& faces):
		MeshElement{} {

	_vertices = verticies;
	_faces = faces;

	genLocalAABB();
}

MeshElement::~MeshElement() {
	A3D_LOG_D("Destroying MeshElement {:p}", static_cast<void*>(this));
}

/// Internal Member Functions ///

void MeshElement::gather(vector<RenderItem>& items,
						 Material& material,
						 mat4& model,
						 FrameStats& stats) {
	++stats.numElements;
	stats.numPolygons += _faces.size();

	items.push_back({this, &material, model});
}

void MeshElement::draw(Renderer& renderer,
					   const RenderContext& context,
					   Material& material,
					   const mat4& modelMat,
					   const mat4& viewMat,
					   const mat4& projectionMat,
					   const DebugOptions& debugOptions,
					   FrameStats& stats) {
	
	renderer.render(*this,
					context,
					material,
					modelMat,
					viewMat,
					projectionMat,
					debugOptions,
					stats);

	++stats.numElements;
	stats.numPolygons += _faces.size();
}

void MeshElement::burnTransform(const mat4& transform, bool normals) {

	for (auto& vert : _vertices) {
		vert.position = vec3(transform * vec4(vert.position, 1.0f));
		if (normals) {
			vert.normal = normalize(vec3(transform * vec4(vert.normal, 0.0f)));
		}
	}

	genLocalAABB();
	
	A3D_MASK_ADD(_dirtyMask, MeshElementDirtyMask::VertexData);
}

const vector<Vertex>& MeshElement::vertices() const {
	return _vertices;
}

const vector<Face>& MeshElement::faces() const {
	return _faces;
}

AABB MeshElement::localAABB() const {
	return _localAABB;
}

vec3 MeshElement::localExtent() const {
	auto aabb = MeshElement::localAABB();
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

/// Protected Member Functions ///

void MeshElement::genLocalAABB() {

	static const float maxFloat = math::f32_max();
	static const float minFloat = math::f32_lowest();

	AABB aabb = { {maxFloat, maxFloat, maxFloat},
				  {minFloat, minFloat, minFloat} };

	for (const auto& v : vertices()) {
		vec3 p = (v.position);
		aabb.min.x = math::min(aabb.min.x, p.x);
		aabb.max.x = math::max(aabb.max.x, p.x);
		aabb.min.y = math::min(aabb.min.y, p.y);
		aabb.max.y = math::max(aabb.max.y, p.y);
		aabb.min.z = math::min(aabb.min.z, p.z);
		aabb.max.z = math::max(aabb.max.z, p.z);
	}

	_localAABB = aabb;
}

/// Protected Lifecycle ///

MeshElement::MeshElement():
		//_vertices{},
		//_faces{},
		//_aabbLines{},
		_dirtyMask{MeshElementDirtyMask::All} { }
