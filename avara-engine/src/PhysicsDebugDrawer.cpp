//
//  PhysicsDebugDrawer.cpp
//	avara-engine
//
//  Created by Morgan Davis on 2/6/18.
//  Copyright © 2018 Morgan K Davis. All rights reserved.
//

#include "PhysicsDebugDrawer.h"

#include <GL/glew.h>

#include "Logger.h"
#include "Program.h"
#include "Utilities.h"


using namespace ae;
using namespace ae::utils;
using namespace glm;
using namespace std;


/***************************************************************************************
     MARK:   Lifecycle
 **************************************************************************************/

PhysicsDebugDrawer::PhysicsDebugDrawer():
	m_debugMode(0),
	//m_lines(vector<DebugLine>()),
	m_lines(vector<vec3>()),
	m_glLinesVBO(-1),
	m_glLinesVAO(-1) {

}

/***************************************************************************************
     MARK:   Internal
 **************************************************************************************/

void PhysicsDebugDrawer::clear() {
	m_lines.clear();
	
	GLuint vbo = m_glLinesVBO;
	glDeleteBuffers(1, &vbo);
	GLuint vao = m_glLinesVAO;
	glDeleteVertexArrays(1, &vao);

	m_glLinesVBO = -1;
	m_glLinesVAO = -1;
}

void PhysicsDebugDrawer::draw(const mat4& viewMat,
							  const mat4& projectionMat) {

	Program program = *Program::AABB();

	loadLinesVertexData(program);
	
	// gl config
	
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glDepthMask(GL_TRUE);
	
	// use shader program
	
	program.use();
	
	// uniforms
	
	program.setUniform("model", mat4(1.0)); // TEMPORARY
	program.setUniform("view", inverse(viewMat));
	program.setUniform("projection", projectionMat);
	
	// draw
	
	glBindVertexArray(m_glLinesVAO);
	glDrawArrays(GL_LINES, 0, m_lines.size());
	//glBindVertexArray(0);
}

/***************************************************************************************
     MARK:   btIDebugDraw
 **************************************************************************************/

void PhysicsDebugDrawer::drawLine(const btVector3& from,
								  const btVector3& to,
								  const btVector3& color) {

	m_lines.emplace_back(GLMVec3FromBTVector3(from));
	m_lines.emplace_back(GLMVec3FromBTVector3(to));
	//m_lines.emplace_back(color);
}

void PhysicsDebugDrawer::drawLine(const btVector3& from,
								  const btVector3& to,
								  const btVector3& fromColor,
								  const btVector3& toColor) {

}

void PhysicsDebugDrawer::drawSphere (const btVector3& p,
									 btScalar radius,
									 const btVector3& color) {

}

void PhysicsDebugDrawer::drawTriangle(const btVector3& a,
									  const btVector3& b,
									  const btVector3& c,
									  const btVector3& color,
									  btScalar alpha) {

}

void PhysicsDebugDrawer::drawContactPoint(const btVector3& pointOnB,
										  const btVector3& normalOnB,
										  btScalar distance,
										  int lifeTime,
										  const btVector3& color) {

}

void PhysicsDebugDrawer::reportErrorWarning(const char* warningString) {

}

void PhysicsDebugDrawer::draw3dText(const btVector3& location,
									const char* textString) {

}

void PhysicsDebugDrawer::setDebugMode(int debugMode) {
	m_debugMode = debugMode;
}

int PhysicsDebugDrawer::getDebugMode() const {
	return m_debugMode;
}

/***************************************************************************************
     MARK:   Private
 **************************************************************************************/

//std::vector<DebugLine> popDebugLines();

void PhysicsDebugDrawer::loadLinesVertexData(const Program& program) {
	
	//AE_LOG->debug("loadLinesVertexData()");

	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, m_lines.size() * sizeof(vec3), &(m_lines[0]), GL_STATIC_DRAW);
	m_glLinesVBO = vbo;
	
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	m_glLinesVAO = vao;
	
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
