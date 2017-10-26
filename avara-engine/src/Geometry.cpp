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

#include "GeometryElement.h"
#include "Material.h"
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

unsigned int Geometry::draw(const glm::mat4& viewMat, const glm::mat4& projectionMat) {

	// MODEL
	mat4 modelMat = node()->worldTransform();
	
	//cout << "DRAW '" << node()->name() << "' worldTransform:\n" << modelMat << endl;
	
	unsigned int numPolygons = 0;

	for (auto element : elements()) {

		GLint program = element->program()->glProgramID();
		GLint vao = element->glVAO();
		GLint ibo = element->glIBO();
		auto faces = element->faces();

		// gl config
		
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // GL_FILL, GL_POINT, GL_LINE
		
		glUseProgram(program);
		
		// MVP
		
		GLint modelLoc = glGetUniformLocation(program, "model");
		GLint viewLoc = glGetUniformLocation(program, "view");
		GLint projectionLoc = glGetUniformLocation(program, "projection");
		
//		cout << "RENDER, modelMat:\n" << modelMat << endl;
//		cout << "RENDER, viewMat:\n" << viewMat << endl;
//		cout << "RENDER, projectionMat:\n" << projectionMat << endl;
		
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, value_ptr(modelMat));
		// TODO: WHY do we have to invert this?? see also Window::addDefaultCamera()
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, value_ptr(inverse(viewMat)));
		//glUniformMatrix4fv(viewLoc, 1, GL_FALSE, value_ptr(viewMat));
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, value_ptr(projectionMat));
		
		// lights
		
//		vec3 lightPosition = vec3(70.0f, 70.0f, 50.0f);
//
//		GLint lightPositionLoc = glGetUniformLocation(program, "light_position_world");
//		glUniform3fv (lightPositionLoc, 1, value_ptr(lightPosition));

		
		//vec3 lightColor = vec3(0.8f, 0.4f, 0.4f);
//		GLint lightColorLoc = glGetUniformLocation(program, "light_color");
//		glUniform3fv (lightColorLoc, 1, value_ptr(lightColor));
		
		// textures
		
//		GLint texDiffuseLoc = glGetUniformLocation(program, "texture_diffuse");
//
//		glActiveTexture(GL_TEXTURE0);
//		glBindTexture(GL_TEXTURE_2D, materials()[0]->diffuse()->glTex());
//		glUniform1i(texDiffuseLoc, 0);
//		glActiveTexture(GL_TEXTURE1);
//		glBindTexture(GL_TEXTURE_2D, texture1);
//		glUniform1i(_textureUniform, 1);
		
		
		// draw

		glBindVertexArray(vao);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
		unsigned int facesSize = faces.size();
		glDrawElements(GL_TRIANGLES, facesSize * sizeof(Face), GL_UNSIGNED_INT, (void*)0);
		
		// stats

		numPolygons += facesSize;
		
	} // geometry element

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
