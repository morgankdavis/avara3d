//
//  Mesh.cc
//  avara3d
//
//  Created by Morgan Davis on 10/21/16.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#include "a3d/mesh/Mesh.h"

#include <utility>

#include "magic_enum/magic_enum.hpp"

#include "a3d/Color.h"
#include "a3d/Image.h"
#include "a3d/diagnostic/log/Log.h"
#include "a3d/diagnostic/exception/UnsupportedFormatException.h"
#include "a3d/mesh/Line.h"
#include "a3d/mesh/MeshElement.h"
#include "a3d/rendering/material/Material.h"
#include "a3d/rendering/renderer/Renderer.h"
#include "a3d/scene/Node.h"
#include "a3d/scene/importer/GlTFImporter.h"

using namespace a3d;
using namespace a3d::math;
using namespace std;

/// Public Static Member Functions ///

shared_ptr<Mesh> Mesh::FromFile(const filesystem::path& path,
								MeshImportOptions options) {

	auto optsUnderlying = static_cast<underlying_type<MeshImportOptions>::type>(options);
	auto sceneOpts = SceneImportOptions(optsUnderlying) | SceneImportOptions::ImportMeshes;
	return GlTFImporter(path, sceneOpts).firstMesh();
}

/// Public Lifecycle Functions ///

Mesh::Mesh(const string& name,
		   unique_ptr<MeshElement> element,
		   const shared_ptr<Material>& material):
		Mesh{std::move(element), material} {

	_name = name;
}

Mesh::Mesh(unique_ptr<MeshElement> element,
		   const shared_ptr<Material>& material):
		Mesh{} {

	if (element) _elements.push_back(std::move(element));
	if (material) _materials.push_back(material);

	genLocalAABB();
}

Mesh::Mesh(const string& name,
		   vector<unique_ptr<MeshElement>>& elements,
		   const vector<shared_ptr<Material>>& materials)
		: Mesh{elements, materials} {

	_name = name;
}

Mesh::Mesh(vector<unique_ptr<MeshElement>>& elements,
		   const vector<shared_ptr<Material>>& materials):
		Mesh{} {

	_elements = vector<unique_ptr<MeshElement>>();
	_elements.reserve(elements.size());
	_elements.insert(_elements.end(),
					 std::make_move_iterator(elements.begin()),
					 std::make_move_iterator(elements.end()));
	_materials = materials;

	genLocalAABB();
}

Mesh::~Mesh() {

	if (_name != nullopt) {
		A3D_LOG_D("Destroying Mesh '{}' ({:p})", *_name, static_cast<void*>(this));
	}
	else {
		A3D_LOG_D("Destroying Mesh {:p}", static_cast<void*>(this));
	}
}

/// Public Member Functions ///

optional<string> Mesh::name() const {
	return _name;
}

void Mesh::name(const string& name) {
	_name = name;
}

const vector<unique_ptr<MeshElement>>& Mesh::elements() {
	return _elements;
}

const vector<shared_ptr<Material>>& Mesh::materials() {
	return _materials;
}

shared_ptr<Material> Mesh::firstMaterial() const {
	if (!_materials.empty()) {
		return _materials[0];
	}
	return nullptr;
}

shared_ptr<Material> Mesh::materialNamed(const string& name) const {
	for (auto& material : _materials) {
		auto matName = material->name();
		if (matName) {
			if (!((*matName) == name)) {
				return material;
			}
		}
	}
	return nullptr;
}

void Mesh::addMaterial(const shared_ptr<Material>& material) {
	_materials.push_back(material);
}

void Mesh::insertMaterial(const shared_ptr<Material>& material, int index) {
	_materials.insert(_materials.begin()+index, material);
}

void Mesh::removeMaterial(int index) {
//	if (_materials.size() >= index-1) { // fails when index==0
		_materials.erase(_materials.begin()+index);
//	}
}

void Mesh::replaceMaterial(int index, const shared_ptr<Material>& replacement) {
	removeMaterial(index);
	insertMaterial(replacement, index);
}

/// Internal Member Functions ///

void Mesh::burnTransform(const mat4& transform, bool normals) {
	for (auto& element : elements()) {
		element->burnTransform(transform, normals);
	}

	genLocalAABB();
}

void Mesh::gather(vector<RenderItem>& items, mat4& model, FrameStats& stats) {

	// for (int e=0; e<_elements.size(); ++e) {
	// 	auto& element = _elements[e];
	//
	// 	Material* material = nullptr;
	// 	if (_materials.size() > e) {
	// 		material = _materials[e].get();
	// 	}
	// 	else {
	// 		material = Material::DefaultMaterial().get();
	// 	}
	//
	//	// cl.exe on windows thinks this doesn't match declaration in MeshElement.h?
	// 	element->gather(items, *material, model, stats);
	// }
	//
	// ++stats.numMeshes;
}

void Mesh::draw(Renderer& renderer,
				const RenderContext& context,
				const mat4& modelMat,
				const mat4& viewMat,
				const mat4& projectionMat,
				const DebugOptions& debugOptions,
				FrameStats& stats) {

	renderer.render(*this,
					context,
					modelMat, viewMat, projectionMat,
					debugOptions, stats);
	
	for (int e=0; e<_elements.size(); ++e) {

		auto& element = _elements[e];
		shared_ptr<Material> material = nullptr;
		if (_materials.size() > e) {
			material = _materials[e];
		}
		else {
			material = Material::DefaultMaterial();
		}

		element->draw(renderer,
					  context,
					  *material,
					  modelMat,
					  viewMat,
					  projectionMat,
					  debugOptions,
					  stats);
	}

	++stats.numMeshes;
}

AABB Mesh::obb() const {

	static const float maxFloat = math::f32_max();
	static const float minFloat = math::f32_lowest();
	AABB aabb = { {maxFloat, maxFloat, maxFloat},
				  {minFloat, minFloat, minFloat} };

	for (const auto& element : _elements) {
		auto elementAABB = element->localAABB();
		aabb.min.x = math::min(aabb.min.x, elementAABB.min.x);
		aabb.max.x = math::max(aabb.max.x, elementAABB.max.x);
		aabb.min.y = math::min(aabb.min.y, elementAABB.min.y);
		aabb.max.y = math::max(aabb.max.y, elementAABB.max.y);
		aabb.min.z = math::min(aabb.min.z, elementAABB.min.z);
		aabb.max.z = math::max(aabb.max.z, elementAABB.max.z);
	}

	return aabb;
}

void Mesh::genLocalAABB() {

	static const float maxFloat = math::f32_max();
	static const float minFloat = math::f32_lowest();
	AABB aabb = { {maxFloat, maxFloat, maxFloat},
				  {minFloat, minFloat, minFloat} };

	for (const auto& element : _elements) {
		auto elementAABB = element->localAABB();
		aabb.min.x = math::min(aabb.min.x, elementAABB.min.x);
		aabb.max.x = math::max(aabb.max.x, elementAABB.max.x);
		aabb.min.y = math::min(aabb.min.y, elementAABB.min.y);
		aabb.max.y = math::max(aabb.max.y, elementAABB.max.y);
		aabb.min.z = math::min(aabb.min.z, elementAABB.min.z);
		aabb.max.z = math::max(aabb.max.z, elementAABB.max.z);
	}

	_localAABB = aabb;
}

AABB Mesh::localAABB() const {

	return _localAABB;
//	static const float maxFloat = math::f32_max();
//	static const float minFloat = math::f32_lowest();
//
//	AABB aabb = { {maxFloat, maxFloat, maxFloat},
//				  {minFloat, minFloat, minFloat} };
//
//	for (const auto& element : _elements) {
//		auto elementAABB = element->localAABB();
//		aabb.min.x = math::min(aabb.min.x, elementAABB.min.x);
//		aabb.max.x = math::max(aabb.max.x, elementAABB.max.x);
//		aabb.min.y = math::min(aabb.min.y, elementAABB.min.y);
//		aabb.max.y = math::max(aabb.max.y, elementAABB.max.y);
//		aabb.min.z = math::min(aabb.min.z, elementAABB.min.z);
//		aabb.max.z = math::max(aabb.max.z, elementAABB.max.z);
//	}
//
//	return aabb;
}

// FASTER
AABB Mesh::worldAABB(const AABB& local, const math::mat4& worldMat, bool vertfit) const {

	if (vertfit) {

		static const float maxFloat = math::f32_max();
		static const float minFloat = math::f32_lowest();
		AABB out = { {maxFloat, maxFloat, maxFloat},
					  {minFloat, minFloat, minFloat} };

		for (const auto& e : _elements) {
			for (const auto& v : e->vertices()) {
				vec3 p = vec3(worldMat * vec4(v.position, 1.0f));
				out.min.x = math::min(out.min.x, p.x); out.max.x = math::max(out.max.x, p.x);
				out.min.y = math::min(out.min.y, p.y); out.max.y = math::max(out.max.y, p.y);
				out.min.z = math::min(out.min.z, p.z); out.max.z = math::max(out.max.z, p.z);
			}
		}

		return out;
	}
	else {

		const vec3 c = (local.min + local.max) * 0.5f;   // local center
		const vec3 e = (local.max - local.min) * 0.5f;   // local half extents

		const vec3 C = vec3(worldMat * vec4(c, 1.0f));      // world center

		// linear part (rotation/scale/shear)
		const mat3 L = math::mat3(worldMat);

		const mat3 A = mat3( vec3(math::abs(L[0][0]), math::abs(L[0][1]), math::abs(L[0][2])),
							 vec3(math::abs(L[1][0]), math::abs(L[1][1]), math::abs(L[1][2])),
							 vec3(math::abs(L[2][0]), math::abs(L[2][1]), math::abs(L[2][2])) );

		const vec3 E = A * e; // world half extents

		return AABB{ C - E, C + E };
	}
}

//AABB Mesh::worldAABB_fromVertices(const mat4& worldMat) const {
//	AABB out{{f32_max(),f32_max(),f32_max()},
//			 {f32_lowest(),f32_lowest(),f32_lowest()}};
//
//	for (const auto& e : _elements) {
//		for (const auto& v : e->vertices()) {
//			vec3 p = vec3(worldMat * vec4(v.position, 1.0f));
//			out.min.x = min(out.min.x, p.x); out.max.x = max(out.max.x, p.x);
//			out.min.y = min(out.min.y, p.y); out.max.y = max(out.max.y, p.y);
//			out.min.z = min(out.min.z, p.z); out.max.z = max(out.max.z, p.z);
//		}
//	}
//	return out;
//}


// SIMPLER
//inline void GetAABBCorners(const a3d::AABB& b, a3d::math::vec3 out[8])
//{
//	const auto& mn = b.min;
//	const auto& mx = b.max;
//
//	// Top (y = max)
//	out[0] = { mn.x, mx.y, mn.z };
//	out[1] = { mn.x, mx.y, mx.z };
//	out[2] = { mx.x, mx.y, mx.z };
//	out[3] = { mx.x, mx.y, mn.z };
//
//	// Bottom (y = min)
//	out[4] = { mn.x, mn.y, mn.z };
//	out[5] = { mn.x, mn.y, mx.z };
//	out[6] = { mx.x, mn.y, mx.z };
//	out[7] = { mx.x, mn.y, mn.z };
//}
//
//AABB Mesh::worldAABB(const AABB& local, const mat4& worldMat) const {
//	vec3 corners[8];
//	GetAABBCorners(local, corners);
//
//	AABB out{ {f32_max(), f32_max(), f32_max()},
//			  {f32_lowest(), f32_lowest(), f32_lowest()} };
//
////	for (auto& c : corners) {
////		vec3 p = vec3(worldMat * vec4(c, 1.0f));
////		out.min = math::min(out.min, p);
////		out.max = math::max(out.max, p);
////	}
//	for (const auto& c : corners) {
//		const vec3 p = vec3(worldMat * vec4(c, 1.0f));
//
//		out.min.x = math::min(out.min.x, p.x);
//		out.min.y = math::min(out.min.y, p.y);
//		out.min.z = math::min(out.min.z, p.z);
//
//		out.max.x = math::max(out.max.x, p.x);
//		out.max.y = math::max(out.max.y, p.y);
//		out.max.z = math::max(out.max.z, p.z);
//	}
//
//	return out;
//}



//vec3 Mesh::extent(const Node* convertTo) const {
//	auto aabb = Mesh::aabb(convertTo);
//	return { aabb.max.x - aabb.min.x,
//			 aabb.max.y - aabb.min.y,
//			 aabb.max.z - aabb.min.z };
//}

vector<Line> Mesh::obbLines() {

//	if (A3D_MASK_CONTAINS(_dirtyMask, MeshDirtyMask::AABBLines)) {

	A3D_LOG_T("Creating AABB Lines for Mesh {:p}...", static_cast<void*>(this));

	auto aabb = Mesh::obb();

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

	static auto gray = Color{vec3{0.5f, 0.5f, 0.5f}};

	return vector<Line>{ {one, two, gray},
						 {two, three, gray},
						 {three, four, gray},
						 {four, one, gray},
						 {five, six, gray},
						 {six, seven, gray},
						 {seven, eight, gray},
						 {eight, five, gray},
						 {one, five, gray},
						 {two, six, gray},
						 {three, seven, gray},
						 {four, eight, gray} };

//		_dirtyMask = A3D_MASK_REMOVE(_dirtyMask, MeshDirtyMask::AABBLines);
//	}
//
//	return _aabbLines;
}

vector<Line> Mesh::worldAabbLines(const math::mat4& worldMat, bool vertfit) {

	A3D_LOG_T("Creating world AABB Lines for Mesh {:p}...", static_cast<void*>(this));

	auto aabb = worldAABB(localAABB(), worldMat, vertfit);

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

	static auto red = Color{vec3{1.0f, 0.f, 0.f}};

	return vector<Line>{ {one, two, red},
						 {two, three, red},
						 {three, four, red},
						 {four, one, red},
						 {five, six, red},
						 {six, seven, red},
						 {seven, eight, red},
						 {eight, five, red},
						 {one, five, red},
						 {two, six, red},
						 {three, seven, red},
						 {four, eight, red} };
}

MeshDirtyMask Mesh::dirtyMask() const {
	return _dirtyMask;
}

void Mesh::dirtyMask(MeshDirtyMask mask) {
	_dirtyMask = mask;
}

/// Private Lifecycle Functions ///

Mesh::Mesh():
		_name{},
		_elements{},
		_materials{},
//		_aabbLines{},
		_dirtyMask{MeshDirtyMask::All} { }
