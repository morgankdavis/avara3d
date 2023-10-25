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
	_name(std::nullopt),
	_elements(vector<std::shared_ptr<ae::GeometryElement>>()),
	_materials(vector<std::shared_ptr<ae::Material>>()),
	_node({}),
	_dirtyBits(GEOMETRY_DIRTY_BITS::ALL) {

}

Geometry::Geometry(const std::shared_ptr<ae::GeometryElement> element,
				   const std::shared_ptr<ae::Material> material):
	Geometry() {
		
		_elements.emplace_back(element);
		_materials.emplace_back(material);
}

Geometry::Geometry(const vector<std::shared_ptr<ae::GeometryElement>> elements,
				   const vector<std::shared_ptr<ae::Material>> materials):
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

std::optional<string> Geometry::name() const {
	return _name;
}

void Geometry::name(const string& name) {
	_name = name;
}

const vector<std::shared_ptr<ae::GeometryElement>>& Geometry::elements() {
	return _elements;
}

const vector<std::shared_ptr<ae::Material>>& Geometry::materials() {
	return _materials;
}

std::shared_ptr<ae::Material> Geometry::firstMaterial() const {
	if (!_materials.empty()) {
		return _materials[0];
	}
	return nullptr;
}

std::shared_ptr<ae::Material> Geometry::materialNamed(const string& name) const {
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

void Geometry::addMaterial(const std::shared_ptr<ae::Material> material) {
	_materials.emplace_back(material);
}

void Geometry::insertMaterial(const std::shared_ptr<ae::Material> material, int index) {
	_materials.insert(_materials.begin()+index, material);
}

void Geometry::removeMaterial(int index) {
	if (_materials.size() >= index-1) {
		_materials.erase(_materials.begin()+index);
	}
}

void Geometry::replaceMaterial(int index, const std::shared_ptr<ae::Material> replacement) {
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
		
		std::shared_ptr<ae::GeometryElement> element = _elements[e];
		std::shared_ptr<ae::Material> material = nullptr;
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

//shared_ptr<map<string, vec3>> Geometry::aabb(bool worldSpace) const {
//
//	float maxFloat = numeric_limits<float>::max();
//	float minFloat = numeric_limits<float>::min();
//
//	auto aabb = make_shared<map<string, vec3>>();
//	(*aabb)["xMin"] = vec3(maxFloat, 0, 0);
//	(*aabb)["xMax"] = vec3(minFloat, 0, 0);
//	(*aabb)["yMin"] = vec3(0, maxFloat, 0);
//	(*aabb)["yMax"] = vec3(0, minFloat, 0);
//	(*aabb)["zMin"] = vec3(0, 0, maxFloat);
//	(*aabb)["zMax"] = vec3(0, 0, minFloat);
//
//	for (auto element : _elements) {
//		for (auto v : element->vertices()) {
//			vec3 p = v.position;
//			if (worldSpace) {
//				if (auto node = _node.lock()) {
//					p = vec3(node->worldTransform() * vec4(v.position, 1.0f));
//				}
//			}
//
//			if (p.x < (*boundingPoints)["xMin"].x) (*aabb)["xMin"] = p;
//			if (p.x > (*aabb)["xMax"].x) (*boundingPoints)["xMax"] = p;
//
//			if (p.y < (*boundingPoints)["yMin"].y) (*aabb)["yMin"] = p;
//			if (p.y > (*aabb)["yMax"].y) (*boundingPoints)["yMax"] = p;
//
//			if (p.z < (*boundingPoints)["zMin"].z) (*aabb)["zMin"] = p;
//			if (p.z > (*aabb)["zMax"].z) (*boundingPoints)["zMax"] = p;
//		}
//	}
//
//	return aabb;
//}

AABB Geometry::aabb(bool worldSpace) const {

	const float maxFloat = numeric_limits<float>::max();
	const float minFloat = numeric_limits<float>::min();

	AABB aabb;
	aabb.min.x = maxFloat;
	aabb.max.x = minFloat;
	aabb.min.y = maxFloat;
	aabb.max.y = minFloat;
	aabb.min.z = maxFloat;
	aabb.max.z = minFloat;

	const auto nodeWorldTransform = (worldSpace
									 ? _node.lock()->worldTransform()
									 : mat4(1.0));

	for (const auto& element : _elements) {
		for (auto v : element->vertices()) {
			vec3 p = (worldSpace
					  ? vec3(nodeWorldTransform * vec4(v.position, 1.0f))
					  : v.position);

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
//	auto bp = *aabb(worldSpace);
//	return vec3(bp["xMax"].x - bp["xMin"].x,
//				bp["yMax"].y - bp["yMin"].y,
//				bp["zMax"].z - bp["zMin"].z);
//}

vec3 Geometry::extent(bool worldSpace) const {
	auto aabb = Geometry::aabb(worldSpace);
	return {aabb.max.x - aabb.min.x,
			aabb.max.y - aabb.min.y,
			aabb.max.z - aabb.min.z};
}

void Geometry::attachedToNode(shared_ptr<Node> node) {
	_node = node;
}

weak_ptr<Node> Geometry::node() const {
	return _node;
}

GEOMETRY_DIRTY_BITS Geometry::dirtyBits() const {
	return _dirtyBits;
}

void Geometry::dirtyBits(GEOMETRY_DIRTY_BITS bits) {
	_dirtyBits = bits;
}
