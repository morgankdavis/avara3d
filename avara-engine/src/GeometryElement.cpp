//
//  GeometryElement.cpp
//	avara-engine
//
//  Created by Morgan Davis on 12/23/16.
//  Copyright © 2016 Morgan K Davis. All rights reserved.
//

#include "GeometryElement.h"

#include <iostream>

#include "Program.h"


using namespace ae;
using namespace std;
using namespace glm;


/***************************************************************************************
     MARK:   Lifecycle
 **************************************************************************************/

GeometryElement::GeometryElement(vector<Vertex> verticies, vector<Face> faces):
	m_vertices(verticies),
	m_faces(faces),
	m_glVAO(-1),
	m_glIBO(-1) {
	// TODO: **** TEMPORARY ***
	m_program = make_shared<Program>(ProgramType_Default);
	if (!m_program->compile()) {
		cerr << "*** ERROR COMPILING SHADER ***" << endl;
	}
}

/***************************************************************************************
     MARK:   Public
 **************************************************************************************/

shared_ptr<Program> GeometryElement::program() const {
	return m_program;
}

void GeometryElement::program(const shared_ptr<Program> program) {
	m_program = program;
}

/***************************************************************************************
     MARK:   Internal
 **************************************************************************************/

void GeometryElement::load() {
	cout << "Loading vertex data..." << endl;
	
	auto verts = vertices();

	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(Vertex), &(verts[0]), GL_STATIC_DRAW);
	
	GLuint vao;
	glGenVertexArrays(1, &vao);
	m_glVAO = vao;  // m_glVAO is signed (change this!)
	glBindVertexArray(vao);
	
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
//	GLuint positionIndex = glGetAttribLocation(glProgramID, "vertex_position");
	GLuint positionIndex = 0;
	glVertexAttribPointer(positionIndex, // attrib index
						  3, // num components per attrib (3 float in vec3)
						  GL_FLOAT, // component type
						  GL_FALSE, // normalize
						  sizeof(Vertex), // stride
						  0); // start offset
	// TODO: MIGHT HAVE TO HAPPEN EVERY DRAW
	glEnableVertexAttribArray(positionIndex);
	
//	GLuint normalIndex = glGetAttribLocation(glProgramID, "vertex_normal");
	GLuint normalIndex = 1;
	glVertexAttribPointer(normalIndex, // attrib index
						  3, // num components per attrib (3 float in vec3)
						  GL_FLOAT, // component type
						  GL_FALSE, // normalize
						  sizeof(Vertex), // stride
						  (void *)sizeof(vec3)); // start offset
	// TODO: MIGHT HAVE TO HAPPEN EVERY DRAW
	glEnableVertexAttribArray(normalIndex);
	
//	GLuint texCoordIndex = glGetAttribLocation(glProgramID, "texture_coordinate");
	GLuint texCoordIndex = 2;
	glVertexAttribPointer(texCoordIndex, // attrib index
						  2, // num components per attrib (2 float in vec2)
						  GL_FLOAT, // component type
						  GL_FALSE, // normalize
						  sizeof(Vertex), // stride
						  (void *)(sizeof(vec3) + sizeof(vec3))); // start offset
	// TODO: MIGHT HAVE TO HAPPEN EVERY DRAW
	glEnableVertexAttribArray(texCoordIndex);

	

	GLuint ibo;
	glGenBuffers(1, &ibo);
	m_glIBO = ibo; // m_glIBO is signed (change this!)
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
				 m_faces.size() * sizeof(Face),
				 &(m_faces[0]),
				 GL_STATIC_DRAW);
}

GLint GeometryElement::glVAO() {
	if (m_glVAO == -1) {
		load();
	}
	return m_glVAO;
}

GLint GeometryElement::glIBO() {
	return m_glIBO;
}

vector<Vertex>& GeometryElement::vertices() {
	return m_vertices;
}

vector<Face>& GeometryElement::faces() {
	return m_faces;
}
