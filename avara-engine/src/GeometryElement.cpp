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
#include "Types.h"
//#include "Utilities.h"


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

void GeometryElement::hardTransform(const mat4 t, bool norm) {

//	for (auto vertex : verts) {
//		vertex.position = vec3(t * vec4(vertex.position, 1.0f));
//
//		if (norm) {
//			vertex.normal = vec3(t * vec4(vertex.normal, 0.0f));
//		}
//	}

//	vector<Vertex>::iterator it = verts->begin(), end = verts->end();
//	while (it != end) {
//		vec3 pos_transformed = vec3(t * vec4((*it).position, 1.0f));
//		(*it).position = pos_transformed;
//
//		if (norm) {
//			vec3 norm_transformed = vec3(t * vec4((*it).normal, 0.0f));
//			(*it).normal = norm_transformed;
//		}
//
//		++it;
//	}
}

void GeometryElement::generateSmoothNormals() {
	// https://stackoverflow.com/questions/6656358/calculating-normals-in-a-triangle-mesh/6661242#6661242
}

void GeometryElement::generateFlatNormals() {
	// https://www.khronos.org/opengl/wiki/Calculating_a_Surface_Normal

	for (Face face : m_faces) {
		Vertex* v1 = &m_vertices[face.a];
		Vertex* v2 = &m_vertices[face.b];
		Vertex* v3 = &m_vertices[face.c];

		vec3 u = v2->position - v1->position;
		vec3 v = v3->position - v1->position;

		vec3 normal;
		normal.x = u.y * v.z - u.z * v.y;
		normal.y = u.z * v.x - u.x * v.z;
		normal.z = u.x * v.y - u.y * v.x;
		normal = normalize(normal);

		v1->normal = normal;
		v2->normal = normal;
		v3->normal = normal;
	}
}

void GeometryElement::loadVertexData() {
	cout << "Loading vertex data..." << endl;

	auto verts = m_vertices;

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
		loadVertexData();
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
