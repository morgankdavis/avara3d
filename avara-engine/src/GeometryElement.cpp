//
//  GeometryElement.cpp
//	avara-engine
//
//  Created by Morgan Davis on 12/23/16.
//  Copyright © 2016 Morgan K Davis. All rights reserved.
//

#include "GeometryElement.h"

#include <iostream>

#include "Color.h"
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

/***************************************************************************************
     MARK:   Internal
 **************************************************************************************/

unsigned GeometryElement::draw(const mat4& modelMat,
							   const mat4& viewMat,
							   const mat4& projectionMat,
							   const Material& material,
							   GLuint glEnvironmentUBO) {

	auto program = material.program();
	
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
	
	material.prepareToRender();

	program->bindUniformBlock("EnvironmentBlock", glEnvironmentUBO);
	
	// draw
	
	glBindVertexArray(m_glVAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_glIBO);
	unsigned int facesSize = m_faces.size();
	glDrawElements(GL_TRIANGLES, facesSize * sizeof(Face), GL_UNSIGNED_INT, (void*)0);
	
	// stats
	
	return facesSize;
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

	cout << "m_vertices: " << m_vertices.size() << endl;
	cout << "m_faces: " << m_faces.size() << endl;

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


	/*
	void GLSObject::ComputeVerticeNormal (int ixVertice)
	{
		// Allocate a temporary storage to store adjacent faces indexes
		if (!m_pStorage)
		{
			m_pStorage = new int[m_nbFaces];
			if (!m_pStorage)
				return;
		}
		// Store each face which has an intersection with the ixVertice'th vertex
		int nbAdjFaces = 0;
		GLFace * pFace = (GLFace *)&OBJ_FACES;
		for (int ix = 0; ix < m_nbFaces; ix++, pFace++)
			if (pFace->v1 == ixVertice)
				m_pStorage[nbAdjFaces++] = ix;
			else
			if (pFace->v2 == ixVertice)
				m_pStorage[nbAdjFaces++] = ix;
			else
			if (pFace->v3 == ixVertice)
				m_pStorage[nbAdjFaces++] = ix;
		// Average all adjacent faces normals to get the vertex normal
		GLpoint pn;
		pn.x = pn.y = pn.z = 0;
		for (int jx = 0; jx < nbAdjFaces; jx++)
		{
			int ixFace= m_pStorage[jx];
			pn.x += m_pFaceNormals[ixFace].x;
			pn.y += m_pFaceNormals[ixFace].y;
			pn.z += m_pFaceNormals[ixFace].z;
		}
		pn.x /= nbAdjFaces;
		pn.y /= nbAdjFaces;
		pn.z /= nbAdjFaces;

		// Normalize the vertex normal
		VectorNormalize(&pn, &m_pVertNormals[ixVertice]);
	}
	 */
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

//void GeometryElement::loadShaderProgram(const string& shaderName) {
//	
//	m_program = make_shared<Program>(shaderName);
//	
//	if (m_program->compile()) {
//		cout << "Shader program '" << shaderName << "' compiled." << endl;
//		
//		if (m_program->link()) {
//			cout << "Shader program '" << shaderName << "' linked." << endl;
//		}
//		else {
//			cout << "Couldn't link '" << shaderName << "' shader:\n" << *(m_program->logString()) << endl;
//		}
//	}
//	else {
//		cout << "Couldn't compile '" << shaderName << "' shader:\n" << *(m_program->logString()) << endl;
//	}
//}

void GeometryElement::loadVertexData(const Program& program) {
	
	// TODO: release any existing buffers
	
	cout << "Loading vertex data... " << &program << endl;
	
	auto verts = m_vertices;
	
	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(Vertex), &(verts[0]), GL_STATIC_DRAW);
	
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
	
	GLuint normalIndex = program.getAttributeLocation("vertex_normal");
	glVertexAttribPointer(normalIndex, // attrib index
						  3, // num components per attrib (3 float in vec3)
						  GL_FLOAT, // component type
						  GL_FALSE, // normalize
						  sizeof(Vertex), // stride
						  (void *)sizeof(vec3)); // start offset
	glEnableVertexAttribArray(normalIndex);
	
	GLuint texCoordIndex = program.getAttributeLocation("texture_coordinate");
	glVertexAttribPointer(texCoordIndex, // attrib index
						  2, // num components per attrib (2 float in vec2)
						  GL_FLOAT, // component type
						  GL_FALSE, // normalize
						  sizeof(Vertex), // stride
						  (void *)(sizeof(vec3) + sizeof(vec3))); // start offset
	glEnableVertexAttribArray(texCoordIndex);
	
	glGenBuffers(1, &m_glIBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_glIBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
				 m_faces.size() * sizeof(Face),
				 &(m_faces[0]),
				 GL_STATIC_DRAW);
}

vector<Vertex>& GeometryElement::vertices() {
	return m_vertices;
}

vector<Face>& GeometryElement::faces() {
	return m_faces;
}
