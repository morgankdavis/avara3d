//
//  SkyboxGeometryElement.cpp
//	avara-engine
//
//  Created by Morgan Davis on 12/12/17.
//  Copyright © 2017 Morgan K Davis. All rights reserved.
//

#include "SkyboxGeometryElement.h"

#include <iostream>
#include <string>

#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>

#include "Logger.h"
#include "MaterialProperty.h"
#include "Program.h"
#include "SkyboxMaterial.h"
#include "Utilities.h"


using namespace ae;
using namespace glm;
using namespace std;


/***************************************************************************************
	Lifecycle
 ***************************************************************************************/

SkyboxGeometryElement::SkyboxGeometryElement(vector<Vertex>& verticies,
											 vector<Face>& faces):
	GeometryElement(verticies, faces) { 

}

SkyboxGeometryElement::~SkyboxGeometryElement() {
//	glDeleteBuffers(1, &m_glVBO);
//	glDeleteBuffers(1, &m_glIBO);
//	glDeleteVertexArrays(1, &m_glVAO);
}

/***************************************************************************************
     GeometryElement
 ***************************************************************************************/

void SkyboxGeometryElement::draw(const mat4& viewMat,
								 const mat4& projectionMat,
								 const SkyboxMaterial& material,
								 DrawStats& stats) {
	
	auto program = material.program();
	
	// gl config
	
	glDepthMask(GL_FALSE);

	// use shader program
	
	program->use();

	// uniforms
	
	program->setUniform("view", viewMat);
	program->setUniform("projection", projectionMat);

	// material
	
	material.cubeProperty()->bind(MATERIAL_PROPERTY_TYPE::AMBIENT, *program); // property type is arbitrary...
	material.prepareToRender(DEBUG_OPTIONS::NONE);

	// draw

	glBindVertexArray(m_glVAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_glIBO);
	unsigned int numFaces = m_faces.size();
	stats.polygons += numFaces;
	glDrawElements(GL_TRIANGLES, numFaces * sizeof(Face), GL_UNSIGNED_INT, (void*)0);
	
	program->unuse();
}

void SkyboxGeometryElement::loadVertexData(const Program& program) {
	
	// TODO: release any existing buffers
	
	//cout << "Loading vertex data... " << &program << endl;
	AE_LOG->info("Loading skybox vertex data...");
	
	auto verts = m_vertices;
	
	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(Vertex), &(verts[0]), GL_STATIC_DRAW);
	m_glIBO = vbo;
	
	glGenVertexArrays(1, &m_glVAO);
	glBindVertexArray(m_glVAO);
	
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	
	GLuint positionIndex = program.getAttributeLocation("vertex_position");
	glVertexAttribPointer(positionIndex, // attrib index
						  3, // num components per attrib (3 float in vec3)
						  GL_FLOAT, // component type
						  GL_FALSE, // normalize
						  sizeof(Vertex), // stride
						  0); // start offset
	glEnableVertexAttribArray(positionIndex);
	
	glGenBuffers(1, &m_glIBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_glIBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
				 m_faces.size() * sizeof(Face),
				 &(m_faces[0]),
				 GL_STATIC_DRAW);
	
	AE_LOG->info("Done.");
}
