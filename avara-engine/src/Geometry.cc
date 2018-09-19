//
//  Geometry.cc
//	avara-engine
//
//  Created by Morgan Davis on 10/21/16.
//  Copyright © 2016 Morgan K Davis. All rights reserved.
//

#include "Geometry.h"

#include <iostream>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>

#include "GeometryElement.h"
#include "Logger.h"
#include "Material.h"
#include "Node.h"
#include "Renderer.h"


using namespace ae;
using namespace std;
using namespace glm;


/***************************************************************************************
     Lifecycle
 ***************************************************************************************/

Geometry::Geometry():
	m_name(boost::none),
	m_elements(vector<shared_ptr<GeometryElement>>()),
	m_materials(vector<shared_ptr<Material>>()),
	m_node({}),
	m_dirtyBits(GEOMETRY_DIRTY_BITS::ALL) {

}

Geometry::Geometry(const shared_ptr<GeometryElement> element,
				   const shared_ptr<Material> material):
	Geometry() {
		
		m_elements.emplace_back(element);
		m_materials.emplace_back(material);
}

Geometry::Geometry(const vector<shared_ptr<GeometryElement>> elements,
				   const vector<shared_ptr<Material>> materials):
	Geometry() {

		m_elements.insert(m_elements.begin(), elements.begin(), elements.end());
		m_materials.insert(m_materials.begin(), materials.begin(), materials.end());
}

Geometry::~Geometry() {
	AE_LOG_D("Destroying Geometry {:p}", (void*)this);
}

/***************************************************************************************
     Public
 ***************************************************************************************/

boost::optional<string> Geometry::name() const {
	return m_name;
}

void Geometry::name(const string& name) {
	m_name = name;
}

const vector<shared_ptr<GeometryElement>>& Geometry::elements() {
	return m_elements;
}

const vector<shared_ptr<Material>>& Geometry::materials() {
	return m_materials;
}

shared_ptr<Material> Geometry::firstMaterial() const {
	if (m_materials.size() > 0) {
		return m_materials[0];
	}
	return nullptr;
}

shared_ptr<Material> Geometry::materialNamed(const string& name) const {
	for (auto material : m_materials) {
		auto matName = material->name();
		if (matName) {
			if (!(*matName).compare(name)) {
				return material;
			}
		}
	}
	return nullptr;
}

void Geometry::addMaterial(const shared_ptr<Material> material) {
	m_materials.emplace_back(material);
}

void Geometry::insertMaterial(const shared_ptr<Material> material, int index) {
	m_materials.insert(m_materials.begin()+index, material);
}

void Geometry::removeMaterial(int index) {
	if (m_materials.size() >= index-1) {
		m_materials.erase(m_materials.begin()+index);
	}
}

void Geometry::replaceMaterial(int index, const shared_ptr<Material> replacement) {
	removeMaterial(index);
	insertMaterial(replacement, index);
}

/***************************************************************************************
     Internal
 ***************************************************************************************/

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
	
#warning may be unneccesary
	if (!DEBUG_OPTIONS_CONTAINS(debugOptions, DEBUG_OPTIONS::SHOW_BOUNDING_BOXES)) {
		m_dirtyBits = GEOMETRY_DIRTY_BITS_ADD(m_dirtyBits, GEOMETRY_DIRTY_BITS::EXTENT);
	}
	
	renderer.render(shared_from_this(),
					modelMat, viewMat, projectionMat,
					debugOptions, stats);

	unsigned numElements = m_elements.size();
	stats.meshes += numElements;
	
	for (unsigned e=0; e<numElements; ++e) {
		
		shared_ptr<GeometryElement> element = m_elements[e];
		shared_ptr<Material> material = nullptr;
		if (m_materials.size() > e) {
			material = m_materials[e];
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

shared_ptr<map<string, vec3>> Geometry::boundingPoints(bool worldSpace) const {

	float maxFloat = numeric_limits<float>::max();
	float minFloat = numeric_limits<float>::min();

	auto boundingPoints = make_shared<map<string, vec3>>();
	(*boundingPoints)["xMin"] = vec3(maxFloat, 0, 0);
	(*boundingPoints)["xMax"] = vec3(minFloat, 0, 0);
	(*boundingPoints)["xMin"] = vec3(0, maxFloat, 0);
	(*boundingPoints)["yMax"] = vec3(0, minFloat, 0);
	(*boundingPoints)["zMin"] = vec3(0, 0, maxFloat);
	(*boundingPoints)["zMax"] = vec3(0, 0, minFloat);

	for (auto element : m_elements) {
		for (auto v : element->vertices()) {
			vec3 p = v.position;
			if (worldSpace) {
				if (auto node = m_node.lock()) {
					p = vec3(node->worldTransform() * vec4(v.position, 1.0f));
				}
			}

			if (p.x < (*boundingPoints)["xMin"].x) (*boundingPoints)["xMin"] = p;
			if (p.x > (*boundingPoints)["xMax"].x) (*boundingPoints)["xMax"] = p;

			if (p.y < (*boundingPoints)["yMin"].y) (*boundingPoints)["yMin"] = p;
			if (p.y > (*boundingPoints)["yMax"].y) (*boundingPoints)["yMax"] = p;

			if (p.z < (*boundingPoints)["zMin"].z) (*boundingPoints)["zMin"] = p;
			if (p.z > (*boundingPoints)["zMax"].z) (*boundingPoints)["zMax"] = p;
		}
	}

	return boundingPoints;
}

vec3 Geometry::extent(bool worldSpace) const {
	auto bp = *boundingPoints(worldSpace);
	return vec3(bp["xMax"].x - bp["xMin"].x,
				bp["yMax"].y - bp["yMin"].y,
				bp["zMax"].z - bp["zMin"].z);
}

void Geometry::attachedToNode(shared_ptr<Node> node) {
	m_node = node;
}

weak_ptr<Node> Geometry::node() const {
	return m_node;
}

GEOMETRY_DIRTY_BITS Geometry::dirtyBits() const {
	return m_dirtyBits;
}

void Geometry::dirtyBits(GEOMETRY_DIRTY_BITS bits) {
	m_dirtyBits = bits;
}
