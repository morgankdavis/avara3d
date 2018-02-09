//
//  GeometryElement.cpp
//	avara-engine
//
//  Created by Morgan Davis on 12/23/16.
//  Copyright © 2016 Morgan K Davis. All rights reserved.
//

#include "GeometryElement.h"

#include <iostream>

#include <GL/glew.h>

#include "Color.h"
#include "Logger.h"
#include "Material.h"
#include "MaterialProperty.h"
#include "Program.h"
#include "Types.h"
#include "Utilities.h"


using namespace ae;
using namespace ae::utils;
using namespace std;
using namespace glm;


/***************************************************************************************
     MARK:   Lifecycle
 **************************************************************************************/

GeometryElement::GeometryElement(std::vector<Vertex>& verticies,
								 std::vector<Face>& faces):
	m_vertices(verticies),
	m_faces(faces),
	//m_vertexDataLoaded(false),
	m_glVAO(0),
	m_glIBO(0) {//,
	//m_glUBO(0) {

}

GeometryElement::~GeometryElement() {
//	glDeleteBuffers(1, &m_glVBO);
//	glDeleteBuffers(1, &m_glIBO);
//	glDeleteVertexArrays(1, &m_glVAO);
}

/***************************************************************************************
     MARK:   Internal
 **************************************************************************************/

void GeometryElement::draw(const mat4& modelMat,
						   const mat4& viewMat,
						   const mat4& projectionMat,
						   Material& material,
						   unsigned glEnvironmentUBO,
						   DebugOption debugOptions,
						   DrawStats& stats) {

	//auto program = material.program();
	auto program = material.selectProgram(debugOptions);
	
	// gl config
	
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glDepthMask(GL_TRUE);
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	// use shader program
	
	program->use();
	
	// uniforms

	program->setUniform("model", modelMat);
	program->setUniform("view", inverse(viewMat));
	program->setUniform("projection", projectionMat);
	
	material.prepareToRender(debugOptions);
	
	if (!(debugOptions & DebugOption_ShowWireframes)) {
		program->bindUniformBlock("EnvironmentBlock", glEnvironmentUBO);
	}
	
	// draw
	
	glBindVertexArray(m_glVAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_glIBO);
	unsigned int numFaces = m_faces.size();
	stats.polygons += numFaces;
	glDrawElements(GL_TRIANGLES, numFaces * 3, GL_UNSIGNED_INT, (void*)0);
	
	program->unuse();
}

void GeometryElement::hardTransform(const mat4 t, bool norm) {

	for (int v=0; v<m_vertices.size(); ++v) {
		Vertex* vertex = &m_vertices[v];
		vertex->position = vec3(t * vec4(vertex->position, 1.0f));

		if (norm) {
			vertex->normal = normalize(vec3(t * vec4(vertex->normal, 0.0f)));
			//vertex->normal = vec3(t * vec4(vertex->normal, 0.0f));
		}
	}
}

void GeometryElement::generateSmoothNormals() {
	// https://www.codeguru.com/cpp/g-m/opengl/article.php/c2681/Computing-normals-to-achieve-flat-and-smooth-shading.htm
	// https://stackoverflow.com/questions/6656358/calculating-normals-in-a-triangle-mesh/6661242#6661242

	generateFlatNormals();

	//cout << "m_vertices: " << m_vertices.size() << endl;
	//cout << "m_faces: " << m_faces.size() << endl;

	auto newVerticies = vector<Vertex>();
	for (unsigned v=0 ; v<m_vertices.size() ; ++v) {
		auto sharedVerticies = vector<Vertex>();

		for (auto face : m_faces) {
			if (face.a == v) {
				sharedVerticies.push_back(m_vertices[face.a]);
			}
			else if (face.b == v) {
				sharedVerticies.push_back(m_vertices[face.b]);
			}
			else if (face.c == v) {
				sharedVerticies.push_back(m_vertices[face.c]);
			}
		}

		vec3 normal = vec3(0.0f, 0.0f, 0.0f);

		//cout << "sharedVerticies: " << sharedVerticies.size() << endl;
		for (auto sharedVertex : sharedVerticies) {
			normal.x += sharedVertex.normal.x;
			normal.y += sharedVertex.normal.y;
			normal.z += sharedVertex.normal.z;
		}
		normal.x /= sharedVerticies.size();
		normal.y /= sharedVerticies.size();
		normal.z /= sharedVerticies.size();
		normal = normalize(normal);

		Vertex newVertex;
		newVertex.position = m_vertices[v].position;
		newVertex.normal = normal;
		newVertex.textureCoordinate = m_vertices[v].textureCoordinate;
		//auto newVertex = (Vertex){ m_vertices[v].position, normal, m_vertices[v].textureCoordinate };
		newVerticies.push_back(newVertex);
	}
	m_vertices = newVerticies;
}

void GeometryElement::generateFlatNormals() {
	// https://www.khronos.org/opengl/wiki/Calculating_a_Surface_Normal

	for (Face face : m_faces) {
		Vertex* v1 = &m_vertices[face.a];
		Vertex* v2 = &m_vertices[face.b];
		Vertex* v3 = &m_vertices[face.c];

		vec3 u = v2->position - v1->position;
		vec3 v = v3->position - v1->position;

		vec3 normal = normalize(cross(u, v));

		v1->normal = normal;
		v2->normal = normal;
		v3->normal = normal;
	}
}

void GeometryElement::loadVertexData(const Program& program) {
	
	// TODO: release any existing buffers
	
	//cout << "Loading vertex data... " << &program << endl;
	AE_LOG->info("Loading vertex data...");
	
	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER,
				 m_vertices.size() * sizeof(Vertex),
				 &(m_vertices[0]),
				 GL_STATIC_DRAW);
	m_glVBO = vbo;
	
	glGenVertexArrays(1, &m_glVAO);
	glBindVertexArray(m_glVAO);
	
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	
	GLuint positionIndex = program.getAttributeLocation("vertex_position");
	glVertexAttribPointer(positionIndex, 			// attrib index
						  3, 						// num components per attrib (3 float in vec3)
						  GL_FLOAT, 				// component type
						  GL_FALSE, 				// normalize
						  sizeof(Vertex), 			// stride
						  0); 						// start offset
	glEnableVertexAttribArray(positionIndex);
	
	GLuint normalIndex = program.getAttributeLocation("vertex_normal");
	glVertexAttribPointer(normalIndex, 				// attrib index
						  3, 						// num components per attrib (3 float in vec3)
						  GL_FLOAT, 				// component type
						  GL_FALSE, 				// normalize
						  sizeof(Vertex), 			// stride
						  (void *)sizeof(vec3)); 	// start offset
	glEnableVertexAttribArray(normalIndex);
	
	GLuint texCoordIndex = program.getAttributeLocation("texture_coordinate");
	glVertexAttribPointer(texCoordIndex, 							// attrib index
						  2, 										// num components per attrib (2 float in vec2)
						  GL_FLOAT, 								// component type
						  GL_FALSE, 								// normalize
						  sizeof(Vertex), 							// stride
						  (void *)(sizeof(vec3) + sizeof(vec3))); 	// start offset
	glEnableVertexAttribArray(texCoordIndex);
	
	glGenBuffers(1, &m_glIBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_glIBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
				 m_faces.size() * sizeof(Face),
				 &(m_faces[0]),
				 GL_STATIC_DRAW);
	
	AE_LOG->info("Done.");
}

vector<Vertex>& GeometryElement::vertices() {
	return m_vertices;
}

vector<Face>& GeometryElement::faces() {
	return m_faces;
}
