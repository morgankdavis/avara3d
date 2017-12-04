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
				   const std::vector<std::shared_ptr<Material>> materials):
	m_elements(elements),
	m_materials(materials) {
		
		cout << "Creating geometry with materials: " << endl;
		
		for (auto material : materials) {
			if (material->diffuse()->image()) {
				cout << "Diffuse image: ("
				<< material->diffuse()->image()->width()
				<< ", " << material->diffuse()->image()->height() << ")" << endl;
			}
			
			if (material->diffuse()->color()) {
				cout << "Diffuse color: ("
				<< material->diffuse()->color()->r << ", "
				<< material->diffuse()->color()->g << ", "
				<< material->diffuse()->color()->b << ")" << endl;
			}
		}
}

/***************************************************************************************
     MARK:   Public
 **************************************************************************************/

string Geometry::name() const {
	return m_name;
}

void Geometry::name(const string& name) {
	m_name = name;
}

vector<shared_ptr<GeometryElement>>& Geometry::elements() {
	return m_elements;
}

vector<shared_ptr<Material>>& Geometry::materials() {
	return m_materials;
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

unsigned int Geometry::draw(const mat4& modelMat,
							const mat4& viewMat,
							const mat4& projectionMat) {
	
	unsigned int numPolygons = 0;
	
	for (int e=0; e < m_elements.size(); ++e) {

		auto element = m_elements[e];

		shared_ptr<Material> material = nullptr;
		if (m_materials.size() > e) {
			//cout << "Using material " << e << endl;
			material = m_materials[e];
		}
		
		numPolygons += element->draw(modelMat, viewMat, projectionMat, &(*material));
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

	cout << "GEOMETRY '" << name() << "' " << "worldTransform: " << endl;
	cout << worldTransform << endl;
	cout << "Num elements: " << m_elements.size() << endl;

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

	cout << "GEOMETRY '" << name() << "' " << "boundingPoints: " << endl;
	for (auto const& i : (*boundingPoints)) {
		cout << i.first << ": " << i.second << endl;
	}

	return boundingPoints;
}
