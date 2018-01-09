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
#include "Logger.h"
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

Geometry::Geometry():
	m_name(boost::none),
	m_elements(vector<shared_ptr<GeometryElement>>()),
	m_materials(vector<shared_ptr<Material>>()),
    m_glAABBVAO(-1) {

}

Geometry::Geometry(const vector<shared_ptr<GeometryElement>> elements,
				   const vector<shared_ptr<Material>> materials):
	m_name(boost::none),
	m_elements(elements),
	m_materials(materials),
    m_glAABBVAO(-1) {
		
		loadVertexData();
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

void Geometry::loadVertexData() {
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
}

void Geometry::hardTransform(mat4 t, bool norm) {
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
						int glEnvironmentUBO,
						DebugOption debugOptions) {
	
	unsigned numPolygons = 0;
	
	for (int e=0; e < m_elements.size(); ++e) {
		
		auto element = m_elements[e];
		shared_ptr<Material> material = nullptr;
		if (m_materials.size() > e) {
			material = m_materials[e];
		}
		else {
			material = Material::DefaultMaterial();
		}

//		shared_ptr<Material> material = nullptr;
//		if (m_materials.size() > e) {
//			material = m_materials[e];
//		}
//		else if (m_materials.size() > 0) {
//			material = m_materials[m_materials.size()-1 % e];
//		}
		
		numPolygons += element->draw(modelMat, viewMat, projectionMat,
									 *material, glEnvironmentUBO, debugOptions);
	}
    
    if (debugOptions & DebugOption_ShowBoundingBoxes) {
		drawAABB(modelMat, viewMat, projectionMat);
    }
	
	return numPolygons;
}

void Geometry::drawAABB(const glm::mat4& modelMat,
						const glm::mat4& viewMat,
						const glm::mat4& projectionMat) {
	
	Program aabbProgram = *Program::AABB();
	
	if (m_glAABBVAO < 0) {
		loadAABBVertexData(aabbProgram);
	}
	
	// gl config
	
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glDepthMask(GL_TRUE);

	// use shader program
	
	aabbProgram.use();
	
	// uniforms
	
	aabbProgram.setUniform("model", modelMat);
	aabbProgram.setUniform("view", inverse(viewMat));
	aabbProgram.setUniform("projection", projectionMat);
	
	// draw
	
	glBindVertexArray(m_glAABBVAO);
	glDrawArrays(GL_LINES, 0, 24 * sizeof(vec3));
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

	//auto worldTransform = m_node->worldTransform();

//	cout << "GEOMETRY '" << name() << "' " << "worldTransform: " << endl;
//	cout << worldTransform << endl;
//	cout << "Num elements: " << m_elements.size() << endl;

	for (auto element : m_elements) {
		for (auto v : element->vertices()) {
			vec3 p = v.position;
            if (worldSpace) p = vec3(m_node->worldTransform() * vec4(v.position, 1.0f));
//            else p = vec3(m_node->transform() * vec4(v.position, 1.0f));

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

void Geometry::loadAABBVertexData(const Program& program) {
    
    // TODO: release any existing buffers
    
    if (m_glAABBVAO < 0) {
        
        AE_LOG.debug("loadAABBVertexData()");
        
        map<string, vec3> bp = *boundingPoints(false);
 
        float xMin = bp["xMin"].x;
        float xMax = bp["xMax"].x;
        float yMin = bp["yMin"].y;
        float yMax = bp["yMax"].y;
        float zMin = bp["zMin"].z;
        float zMax = bp["zMax"].z;
        
        vec3 one =      vec3(xMin, yMax, zMin);
        vec3 two =      vec3(xMin, yMax, zMax);
        vec3 three =    vec3(xMax, yMax, zMax);
        vec3 four =     vec3(xMax, yMax, zMin);
        vec3 five =     vec3(xMin, yMin, zMin);
        vec3 six =      vec3(xMin, yMin, zMax);
        vec3 seven =    vec3(xMax, yMin, zMax);
        vec3 eight =    vec3(xMax, yMin, zMin);

        vec3 verts[] = {
            one, two,
            two, three,
            three, four,
            four, one,
            five, six,
            six, seven,
            seven, eight,
            eight, five,
            one, five,
            two, six,
            three, seven,
            four, eight};
        
        GLuint vbo;
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(vec3), &(verts[0]), GL_STATIC_DRAW);
        
        GLuint vao;
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);
        m_glAABBVAO = vao;
        
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        
        GLuint positionIndex = program.getAttributeLocation("vertex_position");
        glVertexAttribPointer(positionIndex, // attrib index
                              3, // num components per attrib (3 float in vec3)
                              GL_FLOAT, // component type
                              GL_FALSE, // normalize
                              sizeof(vec3), // stride
                              0); // start offset
        glEnableVertexAttribArray(positionIndex);
    }
}
