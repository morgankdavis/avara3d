//
//  Geometry.cpp
//	avara-engine
//
//  Created by Morgan Davis on 10/21/16.
//  Copyright © 2016 Morgan K Davis. All rights reserved.
//

#include "Geometry.h"

#include <iostream>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>

#include "Color.h" // temporary
#include "GeometryElement.h"
#include "Image.h" // temporary
#include "Material.h"
#include "MaterialProperty.h"
#include "Node.h"
#include "Program.h"
#include "Utilities.h"


using namespace ae;
using namespace std;
using namespace glm;
using namespace utils;


/***************************************************************************************
     MARK:   Lifecycle
 **************************************************************************************/

Geometry::Geometry(const vector<shared_ptr<GeometryElement>> elements,
				   const vector<shared_ptr<Material>> materials):
	m_name(boost::none),
	m_elements(elements),
	m_materials(materials) {
		
		for (int e=0; e < m_elements.size(); ++e) {
			auto element = m_elements[e];
			
			if (m_materials.size() > e) {
				auto element = m_elements[e];
				auto material = m_materials[e];
				element->loadVertexData(*(material->program()));
			}
			else {
				element->loadVertexData(*(Material::DefaultMaterial()->program()));
			}
		}


//		for (int m=0; m < m_elements.size() - m_materials.size(); ++m) {
//			m_materials.push_back(Material::DefaultMaterial());
//		}
}

/***************************************************************************************
     MARK:   Public
 **************************************************************************************/

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
	m_materials.erase(m_materials.begin()+index);
}

void Geometry::replaceMaterial(int index, const shared_ptr<Material> replacement) {
	removeMaterial(index);
	insertMaterial(replacement, index);
}

/***************************************************************************************
     MARK:   Internal
 **************************************************************************************/

Node* Geometry::node() const {
	return m_node;
}

void Geometry::node(Node* node) {
	m_node = node;
}

void Geometry::hardTransform(const mat4 t, bool norm) {
	for (auto element : elements()) {
		element->hardTransform(t, norm);
	}
}

void Geometry::generateSmoothNormals() {
	for (auto element : elements()) {
		element->generateSmoothNormals();
	}
}

void Geometry::generateFlatNormals() {
	
	for (auto element : elements()) {
		element->generateFlatNormals();
	}
}

unsigned Geometry::draw(const mat4& modelMat,
						const mat4& viewMat,
						const mat4& projectionMat,
						int glLightsUBO) {
	
	unsigned numPolygons = 0;
	
	for (int e=0; e < m_elements.size(); ++e) {
		
		auto element = m_elements[e];
		auto material = m_materials[e];

//		shared_ptr<Material> material = nullptr;
//		if (m_materials.size() > e) {
//			material = m_materials[e];
//		}
//		else if (m_materials.size() > 0) {
//			material = m_materials[m_materials.size()-1 % e];
//		}
		
		numPolygons += element->draw(modelMat, viewMat, projectionMat, glLightsUBO, &(*material));
		//numPolygons += element->draw(modelMat, viewMat, projectionMat, *material);
	}
	
	return numPolygons;
}

shared_ptr<map<string, vec3>> Geometry::boundingPoints() const {

	float maxFloat = numeric_limits<float>::max();
	float minFloat = numeric_limits<float>::min();

	auto boundingPoints = make_shared<map<string, vec3>>();
	(*boundingPoints)["xMin"] = vec3(maxFloat, 0, 0);
	(*boundingPoints)["xMax"] = vec3(minFloat, 0, 0);
	(*boundingPoints)["xMin"] = vec3(0, maxFloat, 0);
	(*boundingPoints)["yMax"] = vec3(0, minFloat, 0);
	(*boundingPoints)["zMin"] = vec3(0, 0, maxFloat);
	(*boundingPoints)["zMax"] = vec3(0, 0, minFloat);

	auto worldTransform = m_node->worldTransform();

//	cout << "GEOMETRY '" << name() << "' " << "worldTransform: " << endl;
//	cout << worldTransform << endl;
//	cout << "Num elements: " << m_elements.size() << endl;

	for (auto element : m_elements) {
		for (auto v : element->vertices()) {
			vec3 p = vec3(worldTransform * vec4(v.position, 1.0f)); // transform to world space

			if (p.x < (*boundingPoints)["xMin"].x) (*boundingPoints)["xMin"] = p;
			if (p.x > (*boundingPoints)["xMax"].x) (*boundingPoints)["xMax"] = p;

			if (p.y < (*boundingPoints)["yMin"].y) (*boundingPoints)["yMin"] = p;
			if (p.y > (*boundingPoints)["yMax"].y) (*boundingPoints)["yMax"] = p;

			if (p.z < (*boundingPoints)["zMin"].z) (*boundingPoints)["zMin"] = p;
			if (p.z > (*boundingPoints)["zMax"].z) (*boundingPoints)["zMax"] = p;
		}
	}

//	cout << "GEOMETRY '" << name() << "' " << "boundingPoints: " << endl;
//	for (auto const& i : (*boundingPoints)) {
//		cout << i.first << ": " << i.second << endl;
//	}

	return boundingPoints;
}
