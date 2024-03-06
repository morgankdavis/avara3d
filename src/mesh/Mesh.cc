//
//  Mesh.cc
//	avara3d
//
//  Created by Morgan Davis on 10/21/16.
//  Copyright © 2016 Morgan K Davis. All rights reserved.
//

#include "a3d/mesh/Mesh.h"

#include "glm/gtx/transform.hpp"

#include "a3d/Color.h"
#include "a3d/Image.h"
#include "a3d/diagnostic/logging/Logger.h"
#include "a3d/diagnostic/exception/UnsupportedFormat.h"
#include "a3d/mesh/MeshElement.h"
#include "a3d/mesh/primitive/Box.h"
#include "a3d/mesh/primitive/Capsule.h"
#include "a3d/mesh/primitive/Cone.h"
#include "a3d/mesh/primitive/Cylinder.h"
#include "a3d/mesh/primitive/Plane.h"
#include "a3d/mesh/primitive/Sphere.h"
#include "a3d/mesh/primitive/Torus.h"
#include "a3d/mesh/primitive/Tube.h"
#include "a3d/rendering/Renderer.h"
#include "a3d/rendering/material/Material.h"
#include "a3d/scene/Node.h"
#include "a3d/scene/importer/GlTFImporter.h"


using namespace a3d;
using namespace glm;
using namespace std;


/*********************************************************************************************
	Public Static
 *********************************************************************************************/

shared_ptr<Mesh> Mesh::FromFile(const filesystem::path& path,
								MeshImportOptions options) {

	auto optsUnderlying = static_cast<underlying_type<MeshImportOptions>::type>(options);
	auto sceneOpts = SceneImportOptions(optsUnderlying) | SceneImportOptions::ImportMeshes;
	return GlTFImporter(path, sceneOpts).firstMesh();
}

//shared_ptr<Mesh> Mesh::Box(float length,
//						   float width,
//						   float height,
//						   int lengthSegments,
//						   int widthSegments,
//						   int heightSegments) {
//
//	return make_shared<Mesh>(make_shared<a3d::Box>(length,
//												   width,
//												   height,
//												   lengthSegments,
//												   widthSegments,
//												   heightSegments),
//							 Material::DefaultMaterial());
//}
//
//shared_ptr<Mesh> Mesh::Capsule(float radius,
//							   float height,
//							   int slices,
//							   int segments,
//							   int rings) {
//
//	return make_shared<Mesh>(make_shared<a3d::Capsule>(radius,
//													   height,
//													   slices,
//													   segments,
//													   rings),
//							 Material::DefaultMaterial());
//}
//
//shared_ptr<Mesh> Mesh::Cone(float radius,
//							float height,
//							int slices,
//							int segments) {
//
//	return make_shared<Mesh>(make_shared<a3d::Cone>(radius,
//													height,
//													slices,
//													segments),
//							 Material::DefaultMaterial());
//}
//
//shared_ptr<Mesh> Mesh::Cylinder(float radius,
//								float height,
//								int slices,
//								int segments) {
//
//	return make_shared<Mesh>(make_shared<a3d::Cylinder>(radius,
//														height,
//														slices,
//														segments),
//							 Material::DefaultMaterial());
//}
//
//// Disk
//shared_ptr<Mesh> Mesh::Plane(float width,
//							 float height,
//							 int widthSegements,
//							 int heightSegments) {
//
//	return make_shared<Mesh>(make_shared<a3d::Plane>(width,
//													 height,
//													 widthSegements,
//													 heightSegments),
//							 Material::DefaultMaterial());
//}
//
//// RoundedBox
//shared_ptr<Mesh> Mesh::Sphere(float radius,
//							  int segments) {
//
//	return make_shared<Mesh>(make_shared<a3d::Sphere>(radius,
//													  segments),
//							 Material::DefaultMaterial());
//}
//
//// Spring
//shared_ptr<Mesh> Mesh::Torus(float minorRadius,
//							 float majorRadius,
//							 int slices,
//							 int segments) {
//
//	return make_shared<Mesh>(make_shared<a3d::Torus>(minorRadius,
//													 majorRadius,
//													 slices,
//													 segments),
//							 Material::DefaultMaterial());
//}
//
//// TorusKnot
//shared_ptr<Mesh> Mesh::Tube(float innerRadius,
//							float outerRadius,
//							float height,
//							int slices,
//							int segments) {
//
//	return make_shared<Mesh>(make_shared<a3d::Tube>(innerRadius,
//													outerRadius,
//													height,
//													slices,
//													segments),
//							 Material::DefaultMaterial());
//}

/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

Mesh::Mesh():
		_name(nullopt),
		_elements(vector<shared_ptr<MeshElement>>()),
		_materials(vector<shared_ptr<Material>>()),
//	_node({}),
		_dirtyMask(MeshDirtyMask::All) {

}

Mesh::Mesh(const shared_ptr<MeshElement> element,
		   const shared_ptr<Material> material):
		Mesh() {

	_elements.emplace_back(element);
	_materials.emplace_back(material);
}

Mesh::Mesh(const vector<shared_ptr<MeshElement>> elements,
		   const vector<shared_ptr<Material>> materials):
		Mesh() {

	_elements.insert(_elements.begin(), elements.begin(), elements.end());
	_materials.insert(_materials.begin(), materials.begin(), materials.end());
}

Mesh::~Mesh() {
	A3D_LOG_D("Destroying Mesh {:p}", static_cast<void*>(this));
}

/*********************************************************************************************
	Public
 *********************************************************************************************/

optional<string> Mesh::name() const {
	return _name;
}

void Mesh::name(const string& name) {
	_name = name;
}

const vector<shared_ptr<MeshElement>>& Mesh::elements() {
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
	for (auto material : _materials) {
		auto matName = material->name();
		if (matName) {
			if (!((*matName) == name)) {
				return material;
			}
		}
	}
	return nullptr;
}

void Mesh::addMaterial(const shared_ptr<Material> material) {
	_materials.emplace_back(material);
}

void Mesh::insertMaterial(const shared_ptr<Material> material, int index) {
	_materials.insert(_materials.begin()+index, material);
}

void Mesh::removeMaterial(int index) {
	if (_materials.size() >= index-1) {
		_materials.erase(_materials.begin()+index);
	}
}

void Mesh::replaceMaterial(int index, const shared_ptr<Material> replacement) {
	removeMaterial(index);
	insertMaterial(replacement, index);
}

/*********************************************************************************************
	Internal
 *********************************************************************************************/

void Mesh::burnTransform(const mat4& transform, bool normals) {
	for (auto element : elements()) {
		element->burnTransform(transform, normals);
	}
	//node()->lock()->transform(mat4(1.0));
}

void Mesh::draw(Renderer& renderer,
				const mat4& modelMat,
				const mat4& viewMat,
				const mat4& projectionMat,
				const DebugOptions& debugOptions,
				Stats& stats) {
	
	// forces Renderer to re-create AABB linesets next time they're turned on.
	// this seems like hacky way to do it.
	if (A3D_MASK_CONTAINS(debugOptions, DebugOptions::ShowBoundingBoxes)) {
		_dirtyMask = A3D_MASK_ADD(_dirtyMask, MeshDirtyMask::Extent);
	}
	
	renderer.render(shared_from_this(),
					modelMat, viewMat, projectionMat,
					debugOptions, stats);
	
	for (unsigned e=0; e<_elements.size(); ++e) {
		
		shared_ptr<MeshElement> element = _elements[e];
		shared_ptr<Material> material = nullptr;
		if (_materials.size() > e) {
			material = _materials[e];
		}
		else {
			material = Material::DefaultMaterial();
		}
		
		element->draw(renderer,
					  *material,
					  modelMat,
					  viewMat,
					  projectionMat,
					  debugOptions,
					  stats);
	}

	stats.meshes++;
}

AABB Mesh::aabb(const shared_ptr<Node> convertToNode) const {

	static const float maxFloat = numeric_limits<float>::max();
	static const float minFloat = numeric_limits<float>::min();

	AABB aabb = { {maxFloat, maxFloat, maxFloat},
				  {minFloat, minFloat, minFloat} };

	for (const auto& element : _elements) {
		auto elementAABB = element->aabb(convertToNode);
			aabb.min.x = std::min(aabb.min.x, elementAABB.min.x);
			aabb.max.x = std::max(aabb.max.x, elementAABB.max.x);
			aabb.min.y = std::min(aabb.min.y, elementAABB.min.y);
			aabb.max.y = std::max(aabb.max.y, elementAABB.max.y);
			aabb.min.z = std::min(aabb.min.z, elementAABB.min.z);
			aabb.max.z = std::max(aabb.max.z, elementAABB.max.z);
	}

	return aabb;
}

vec3 Mesh::extent(const shared_ptr<Node> convertToNode) const {
	auto aabb = Mesh::aabb(convertToNode);
	return {aabb.max.x - aabb.min.x,
			aabb.max.y - aabb.min.y,
			aabb.max.z - aabb.min.z};
}

//void Mesh::attachedToNode(shared_ptr<Node> node) {
//	_node = node;
//}
//
//weak_ptr<Node> Mesh::node() const {
//	return _node;
//}

MeshDirtyMask Mesh::dirtyMask() const {
	return _dirtyMask;
}

void Mesh::dirtyMask(MeshDirtyMask mask) {
	_dirtyMask = mask;
}
