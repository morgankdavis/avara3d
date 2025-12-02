//
//  Mesh.cc
//  avara3d
//
//  Created by Morgan Davis on 10/21/16.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#include "a3d/mesh/Mesh.h"

#include <utility>

#include "glm/gtx/transform.hpp"
#include "magic_enum.hpp"

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
using namespace glm;
using namespace std;


/*********************************************************************************************
	Public Static Member Functions
 *********************************************************************************************/

shared_ptr<Mesh> Mesh::FromFile(const filesystem::path& path,
								MeshImportOptions options) {

	auto optsUnderlying = static_cast<underlying_type<MeshImportOptions>::type>(options);
	auto sceneOpts = SceneImportOptions(optsUnderlying) | SceneImportOptions::ImportMeshes;
	return GlTFImporter(path, sceneOpts).firstMesh();
}

/*********************************************************************************************
	Public Lifecycle Functions
 *********************************************************************************************/

Mesh::Mesh(const std::string& name,
		   std::unique_ptr<MeshElement> element,
		   const std::shared_ptr<Material>& material):
		Mesh{std::move(element), material} {

	_name = name;
}

Mesh::Mesh(unique_ptr<MeshElement> element,
		   const shared_ptr<Material>& material):
		Mesh{} {

	if (element) _elements.push_back(std::move(element));
	if (material) _materials.push_back(material);
}

Mesh::Mesh(const std::string& name,
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
}

Mesh::~Mesh() {

	if (_name != nullopt) {
		A3D_LOG_D("Destroying Mesh '{}' ({:p})", *_name, static_cast<void*>(this));
	}
	else {
		A3D_LOG_D("Destroying Mesh {:p}", static_cast<void*>(this));
	}
}

/*********************************************************************************************
	Public Member Functions
 *********************************************************************************************/

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

/*********************************************************************************************
	Internal Member Functions
 *********************************************************************************************/

void Mesh::burnTransform(const mat4& transform, bool normals) {
	for (auto& element : elements()) {
		element->burnTransform(transform, normals);
	}
	//node()->lock()->transform(mat4(1.0));
}

void Mesh::draw(Renderer& renderer,
				const RenderContext& context,
				const mat4& modelMat,
				const mat4& viewMat,
				const mat4& projectionMat,
				const DebugOptions& debugOptions,
				Stats& stats) {

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

	++stats.meshes;
}

AABB Mesh::aabb(const Node* convertTo) const {

	static const float maxFloat = numeric_limits<float>::max();
	static const float minFloat = numeric_limits<float>::min();

	AABB aabb = { {maxFloat, maxFloat, maxFloat},
				  {minFloat, minFloat, minFloat} };

	for (const auto& element : _elements) {
		auto elementAABB = element->aabb(convertTo);
		aabb.min.x = std::min(aabb.min.x, elementAABB.min.x);
		aabb.max.x = std::max(aabb.max.x, elementAABB.max.x);
		aabb.min.y = std::min(aabb.min.y, elementAABB.min.y);
		aabb.max.y = std::max(aabb.max.y, elementAABB.max.y);
		aabb.min.z = std::min(aabb.min.z, elementAABB.min.z);
		aabb.max.z = std::max(aabb.max.z, elementAABB.max.z);
	}

	return aabb;
}

vec3 Mesh::extent(const Node* convertTo) const {
	auto aabb = Mesh::aabb(convertTo);
	return { aabb.max.x - aabb.min.x,
			 aabb.max.y - aabb.min.y,
			 aabb.max.z - aabb.min.z };
}

//const vector<Line>& Mesh::aabbLines(bool& dirty) {
//
//	if (A3D_MASK_CONTAINS(_dirtyMask, MeshDirtyMask::AABBLines)) {
//
//		A3D_LOG_T("Creating AABB Lines for Mesh {:p}...", static_cast<void*>(this));
//
//		auto aabb = Mesh::aabb();
//
//		float xMin = aabb.min.x;
//		float xMax = aabb.max.x;
//		float yMin = aabb.min.y;
//		float yMax = aabb.max.y;
//		float zMin = aabb.min.z;
//		float zMax = aabb.max.z;
//
//		vec3 one = 		{xMin, yMax, zMin};
//		vec3 two =      {xMin, yMax, zMax};
//		vec3 three =    {xMax, yMax, zMax};
//		vec3 four =     {xMax, yMax, zMin};
//		vec3 five =     {xMin, yMin, zMin};
//		vec3 six =      {xMin, yMin, zMax};
//		vec3 seven =    {xMax, yMin, zMax};
//		vec3 eight =    {xMax, yMin, zMin};
//
//		static auto red = Color::Red();
//
//		_aabbLines = vector<Line>{
//			{one, two, red},
//			{two, three, red},
//			{three, four, red},
//			{four, one, red},
//			{five, six, red},
//			{six, seven, red},
//			{seven, eight, red},
//			{eight, five, red},
//			{one, five, red},
//			{two, six, red},
//			{three, seven, red},
//			{four, eight, red}
//		};
//
//		dirty = true;
//
//		_dirtyMask = A3D_MASK_REMOVE(_dirtyMask, MeshDirtyMask::AABBLines);
//	}
//	else {
//		dirty = false;
//	}
//
//	return _aabbLines;
//}

const vector<Line>& Mesh::aabbLines() {

	if (A3D_MASK_CONTAINS(_dirtyMask, MeshDirtyMask::AABBLines)) {

		A3D_LOG_T("Creating AABB Lines for Mesh {:p}...", static_cast<void*>(this));

		auto aabb = Mesh::aabb();

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

		_aabbLines = vector<Line>{
				{one, two, red},
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
				{four, eight, red}
		};

		_dirtyMask = A3D_MASK_REMOVE(_dirtyMask, MeshDirtyMask::AABBLines);
	}

	return _aabbLines;
}

MeshDirtyMask Mesh::dirtyMask() const {
	return _dirtyMask;
}

void Mesh::dirtyMask(MeshDirtyMask mask) {
	_dirtyMask = mask;
}

/*********************************************************************************************
	Private Lifecycle Functions
 *********************************************************************************************/

Mesh::Mesh():
		_name{},
		_elements{},
		_materials{},
		_aabbLines{},
		_dirtyMask{MeshDirtyMask::All} { }
