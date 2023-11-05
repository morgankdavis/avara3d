//
//  Geometry.cc
//	avara-engine
//
//  Created by Morgan Davis on 10/21/16.
//  Copyright © 2016 Morgan K Davis. All rights reserved.
//

#include "Geometry.h"

#include <iostream>

// apparently we're not using anything experimental here since at least GLM .9.9.8
//#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>

#include "GeometryElement.h"
#include "Logger.h"
#include "Material.h"
#include "Node.h"
#include "Renderer.h"


using namespace ae;
using namespace std;
using namespace glm;


/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

Geometry::Geometry():
		_name(nullopt),
		_elements(vector<shared_ptr<GeometryElement>>()),
		_materials(vector<shared_ptr<Material>>()),
//	_node({}),
		_dirtyBits(GEOMETRY_DIRTY_BITS::ALL) {

}

Geometry::Geometry(const shared_ptr<GeometryElement> element,
				   const shared_ptr<Material> material):
		Geometry() {

	_elements.emplace_back(element);
	_materials.emplace_back(material);
}

Geometry::Geometry(const vector<shared_ptr<GeometryElement>> elements,
				   const vector<shared_ptr<Material>> materials):
		Geometry() {

	_elements.insert(_elements.begin(), elements.begin(), elements.end());
	_materials.insert(_materials.begin(), materials.begin(), materials.end());
}

Geometry::~Geometry() {
	AE_LOG_D("Destroying Geometry {:p}", (void*)this);
}

/*********************************************************************************************
	Public
 *********************************************************************************************/

optional<string> Geometry::name() const {
	return _name;
}

void Geometry::name(const string& name) {
	_name = name;
}

const vector<shared_ptr<GeometryElement>>& Geometry::elements() {
	return _elements;
}

const vector<shared_ptr<Material>>& Geometry::materials() {
	return _materials;
}

shared_ptr<Material> Geometry::firstMaterial() const {
	if (!_materials.empty()) {
		return _materials[0];
	}
	return nullptr;
}

shared_ptr<Material> Geometry::materialNamed(const string& name) const {
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

void Geometry::addMaterial(const shared_ptr<Material> material) {
	_materials.emplace_back(material);
}

void Geometry::insertMaterial(const shared_ptr<Material> material, int index) {
	_materials.insert(_materials.begin()+index, material);
}

void Geometry::removeMaterial(int index) {
	if (_materials.size() >= index-1) {
		_materials.erase(_materials.begin()+index);
	}
}

void Geometry::replaceMaterial(int index, const shared_ptr<Material> replacement) {
	removeMaterial(index);
	insertMaterial(replacement, index);
}

/*********************************************************************************************
	Internal
 *********************************************************************************************/

void Geometry::burnTransform(const mat4& transform, bool normals) {
	for (auto element : elements()) {
		element->burnTransform(transform, normals);
	}
	//node()->lock()->transform(mat4(1.0));
}

void Geometry::draw(Renderer& renderer,
					const mat4& modelMat,
					const mat4& viewMat,
					const mat4& projectionMat,
					const DEBUG_OPTIONS& debugOptions,
					RenderStats& stats) {
	
	// forces Renderer to re-create AABB linesets next time they're turned on.
	// this seems like hacky way to do it.
	if (!DEBUG_OPTIONS_CONTAINS(debugOptions, DEBUG_OPTIONS::SHOW_BOUNDING_BOXES)) {
		_dirtyBits = GEOMETRY_DIRTY_BITS_ADD(_dirtyBits, GEOMETRY_DIRTY_BITS::EXTENT);
	}
	
	renderer.render(shared_from_this(),
					modelMat, viewMat, projectionMat,
					debugOptions, stats);

	unsigned numElements = _elements.size();
	stats.meshes += numElements;
	
	for (unsigned e=0; e<numElements; ++e) {
		
		shared_ptr<GeometryElement> element = _elements[e];
		shared_ptr<Material> material = nullptr;
		if (_materials.size() > e) {
			material = _materials[e];
		}
		else {
			material = Material::DefaultMaterial();
		}
		
		element->draw(renderer,
					  *material,
					  modelMat, viewMat, projectionMat,
					  debugOptions, stats);
	}	
}

//AABB Geometry::aabb(bool worldSpace) const {
AABB Geometry::aabb(const shared_ptr<Node> convertToNode) const {

	const float maxFloat = numeric_limits<float>::max();
	const float minFloat = numeric_limits<float>::min();

	AABB aabb = { {maxFloat, maxFloat, maxFloat},
				  {minFloat, minFloat, minFloat} };

	const auto nodeWorldTransform = (convertToNode
									 ? convertToNode->worldTransform()
									 : mat4(1.0));

	for (const auto& element : _elements) {
		for (auto v : element->vertices()) {
			vec3 p = (convertToNode
					  ? vec3(nodeWorldTransform * vec4(v.position, 1.0f))
					  : v.position);
//			vec3 p = v.position;

			aabb.min.x = std::min(aabb.min.x, p.x);
			aabb.max.x = std::max(aabb.max.x, p.x);
			aabb.min.y = std::min(aabb.min.y, p.y);
			aabb.max.y = std::max(aabb.max.y, p.y);
			aabb.min.z = std::min(aabb.min.z, p.z);
			aabb.max.z = std::max(aabb.max.z, p.z);
		}
	}

	return aabb;
}

//vec3 Geometry::extent(bool worldSpace) const {
vec3 Geometry::extent(const shared_ptr<Node> convertToNode) const {
//	auto aabb = Geometry::aabb(worldSpace);
	auto aabb = Geometry::aabb(convertToNode);
	return {aabb.max.x - aabb.min.x,
			aabb.max.y - aabb.min.y,
			aabb.max.z - aabb.min.z};
}

//void Geometry::attachedToNode(shared_ptr<Node> node) {
//	_node = node;
//}
//
//weak_ptr<Node> Geometry::node() const {
//	return _node;
//}

GEOMETRY_DIRTY_BITS Geometry::dirtyBits() const {
	return _dirtyBits;
}

void Geometry::dirtyBits(GEOMETRY_DIRTY_BITS bits) {
	_dirtyBits = bits;
}
