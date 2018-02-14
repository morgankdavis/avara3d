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

PhysicsDebugDrawer::~PhysicsDebugDrawer() {
	if (m_glLinesVBO >=0) {
		GLuint vbo = m_glLinesVBO;
		glDeleteBuffers(1, &vbo);
	}
	if (m_glLinesVAO >=0) {
		GLuint vao = m_glLinesVAO;
		glDeleteVertexArrays(1, &vao);
	}
}

/***************************************************************************************
     MARK:   Internal
 **************************************************************************************/

void PhysicsDebugDrawer::clear() {
	m_lines.clear();
}

void PhysicsDebugDrawer::draw(const mat4& viewMat,
							  const mat4& projectionMat) {
	
	if (getDebugMode() != btIDebugDraw::DBG_NoDebug) {
		AE_LOG->trace("PhysicsDebugDrawer::draw()");
		
		Program program = *Program::Lines();
		
		loadLinesVertexData(program);
		
		// gl config
		
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
		glDepthMask(GL_TRUE);
		glEnable(GL_LINE_SMOOTH);
		
		// https://www.opengl.org/archives/resources/faq/technical/polygonoffset.htm
		//glDepthRange(0.0, 0.9);
		glDisable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(0.0, 0.0);

		// use shader program
		
		program.use();
		
		// uniforms
		
		program.setUniform("model", mat4(1.0));
		program.setUniform("view", inverse(viewMat));
		program.setUniform("projection", projectionMat);
		
		// draw
		
		glBindVertexArray(m_glLinesVAO);
		glDrawArrays(GL_LINES, 0, m_lines.size());
		//glBindVertexArray(0);
	}
}

/***************************************************************************************
     MARK:   btIDebugDraw
 **************************************************************************************/

void PhysicsDebugDrawer::drawLine(const btVector3& from,
								  const btVector3& to,
								  const btVector3& color) {
	//AE_LOG->debug("drawLine() - 1 color");
	
	drawLine(from, to, color, color);
}

void PhysicsDebugDrawer::drawLine(const btVector3& from,
								  const btVector3& to,
								  const btVector3& fromColor,
								  const btVector3& toColor) {
	//AE_LOG->debug("drawLine() - 2 colors");
	
	m_lines.emplace_back(GLMVec3FromBTVector3(from));
	m_lines.emplace_back(GLMVec3FromBTVector3(fromColor));
	m_lines.emplace_back(GLMVec3FromBTVector3(to));
	m_lines.emplace_back(GLMVec3FromBTVector3(toColor));
}

/*
void PhysicsDebugDrawer::drawSphere(btScalar radius,
									const btTransform& transform,
									const btVector3& color) {
	//AE_LOG->debug(AE_FUNC);
}

void PhysicsDebugDrawer::drawSphere (const btVector3& p,
									 btScalar radius,
									 const btVector3& color) {
	//AE_LOG->debug(AE_FUNC);
}

void PhysicsDebugDrawer::drawTriangle(const btVector3& v0,
									  const btVector3& v1,
									  const btVector3& v2,
									  const btVector3& color,
									  btScalar alpha) {
	//AE_LOG->debug(AE_FUNC);
}
*/

void PhysicsDebugDrawer::drawContactPoint(const btVector3& pointOnB,
										  const btVector3& normalOnB,
										  btScalar distance,
										  int lifeTime,
										  const btVector3& color) {
	//AE_LOG->debug(AE_FUNC);
	
	const float DISTANCE_EXTENSION = 0.0;
	btVector3 to = pointOnB + normalOnB * (distance + DISTANCE_EXTENSION);
	drawLine(pointOnB, to, color, color);
}

void PhysicsDebugDrawer::reportErrorWarning(const char* warningString) {
	AE_LOG->warn("Bullet warning: {}", warningString);
}

void PhysicsDebugDrawer::draw3dText(const btVector3& location,
									const char* textString) {
	AE_LOG->debug(AE_FUNC);
}

/*
void PhysicsDebugDrawer::drawAabb(const btVector3 &from,
								  const btVector3 &to,
								  const btVector3 &color) {
	//AE_LOG->debug(AE_FUNC);
}

void PhysicsDebugDrawer::drawTransform(const btTransform &transform,
									   btScalar orthoLen) {
	//AE_LOG->debug(AE_FUNC);
}

void PhysicsDebugDrawer::drawArc(const btVector3& center,
								 const btVector3& normal,
								 const btVector3& axis,
								 btScalar radiusA,
								 btScalar radiusB,
								 btScalar minAngle,
								 btScalar maxAngle,
								 const btVector3& color,
								 bool drawSect,
								 btScalar stepDegrees) {
	//AE_LOG->debug(AE_FUNC);
}

void PhysicsDebugDrawer::drawSpherePatch(const btVector3& center,
										 const btVector3& up,
										 const btVector3& axis,
										 btScalar radius,
										 btScalar minTh,
										 btScalar maxTh,
										 btScalar minPs,
										 btScalar maxPs,
										 const btVector3& color,
										 btScalar stepDegrees,
										 bool drawCenter) {
	//AE_LOG->debug(AE_FUNC);
}

void PhysicsDebugDrawer::drawBox(const btVector3& bbMin,
								 const btVector3& bbMax,
								 const btVector3& color) {
	//AE_LOG->debug(AE_FUNC);
}

void PhysicsDebugDrawer::drawBox(const btVector3& bbMin,
								 const btVector3& bbMax,
								 const btTransform& trans,
								 const btVector3& color) {
	//AE_LOG->debug(AE_FUNC);
}

void PhysicsDebugDrawer::drawCapsule(btScalar radius,
									 btScalar halfHeight,
									 int upAxis,
									 const btTransform& transform,
									 const btVector3& color) {
	//AE_LOG->debug(AE_FUNC);
}

void PhysicsDebugDrawer::drawCylinder(btScalar radius,
									  btScalar halfHeight,
									  int upAxis,
									  const btTransform& transform,
									  const btVector3& color) {
	//AE_LOG->debug(AE_FUNC);
}

void PhysicsDebugDrawer::drawCone(btScalar radius,
								  btScalar height,
								  int upAxis,
								  const btTransform& transform,
								  const btVector3& color) {
	//AE_LOG->debug(AE_FUNC);
}

void PhysicsDebugDrawer::drawPlane(const btVector3& planeNormal,
								   btScalar planeConst,
								   const btTransform& transform,
								   const btVector3& color) {
	//AE_LOG->debug(AE_FUNC);
}
*/

//void PhysicsDebugDrawer::flushLines() {
//}

void PhysicsDebugDrawer::setDebugMode(int debugMode) {
	m_debugMode = debugMode;
}

int PhysicsDebugDrawer::getDebugMode() const {
	return m_debugMode;
}

/***************************************************************************************
     MARK:   Private
 **************************************************************************************/

//void PhysicsDebugDrawer::loadLinesVertexData(const Program& program) {
//	
//	////AE_LOG->debug("loadLinesVertexData()");
//	
//	if (m_glLinesVBO == -1) {
//		GLuint vbo;
//		glGenBuffers(1, &vbo);
//		m_glLinesVBO = vbo;
//	}
//	glBindBuffer(GL_ARRAY_BUFFER, (GLuint)m_glLinesVBO);
//	glBufferData(GL_ARRAY_BUFFER, m_lines.size() * sizeof(DebugLine), &(m_lines[0]), GL_DYNAMIC_DRAW);
//	
//	if (m_glLinesVAO == -1) {
//		GLuint vao;
//		glGenVertexArrays(1, &vao);
//		m_glLinesVAO = vao;
//	}
//	glBindVertexArray((GLuint)m_glLinesVAO);
//	
//	GLuint positionIndex = program.getAttributeLocation("vertex_position");
//	glVertexAttribPointer(positionIndex, 		// attrib index
//						  3, 					// num components per attrib (3 float in vec3)
//						  GL_FLOAT, 			// component type
//						  GL_FALSE, 			// normalize
//						  sizeof(DebugLine),	// stride
//						  0); 					// start offset
//	glEnableVertexAttribArray(positionIndex);
//	
//	GLuint colorIndex = program.getAttributeLocation("vertex_color");
//	glVertexAttribPointer(colorIndex, 		// attrib index
//						  3, 					// num components per attrib (3 float in vec3)
//						  GL_FLOAT, 			// component type
//						  GL_FALSE, 			// normalize
//						  sizeof(DebugLine),	// stride
//						  (void*)&(m_lines[2]));	// start offset <-----------
//	glEnableVertexAttribArray(colorIndex);
//}

//void PhysicsDebugDrawer::loadLinesVertexData(const Program& program) {
//	
//	////AE_LOG->debug("loadLinesVertexData()");
//	
//	if (m_glLinesVBO == -1) {
//		GLuint vbo;
//		glGenBuffers(1, &vbo);
//		m_glLinesVBO = vbo;
//	}
//	glBindBuffer(GL_ARRAY_BUFFER, (GLuint)m_glLinesVBO);
//	glBufferData(GL_ARRAY_BUFFER, m_lines.size() * sizeof(DebugLine), &(m_lines[0]), GL_DYNAMIC_DRAW);
//	
//	if (m_glLinesVAO == -1) {
//		GLuint vao;
//		glGenVertexArrays(1, &vao);
//		m_glLinesVAO = vao;
//	}
//	glBindVertexArray((GLuint)m_glLinesVAO);
//	
//	GLuint positionIndex = program.getAttributeLocation("vertex_position");
//	glVertexAttribPointer(positionIndex, 		// attrib index
//						  3, 					// num components per attrib (3 float in vec3)
//						  GL_FLOAT, 			// component type
//						  GL_FALSE, 			// normalize
//						  sizeof(DebugLine),	// stride
//						  0); 					// start offset
//	glEnableVertexAttribArray(positionIndex);
//}

void PhysicsDebugDrawer::loadLinesVertexData(const Program& program) {
	
	////AE_LOG->debug("loadLinesVertexData()");
	
	if (m_glLinesVBO == -1) {
		GLuint vbo;
		glGenBuffers(1, &vbo);
		m_glLinesVBO = vbo;
	}
	glBindBuffer(GL_ARRAY_BUFFER, (GLuint)m_glLinesVBO);
	glBufferData(GL_ARRAY_BUFFER, m_lines.size() * sizeof(vec3), &(m_lines[0]), GL_DYNAMIC_DRAW);
	
	if (m_glLinesVAO == -1) {
		GLuint vao;
		glGenVertexArrays(1, &vao);
		m_glLinesVAO = vao;
	}
	glBindVertexArray((GLuint)m_glLinesVAO);
	
	GLuint positionIndex = program.getAttributeLocation("vertex_position");
	glVertexAttribPointer(positionIndex, 		// attrib index
						  3, 					// num components per attrib (3 float in vec3)
						  GL_FLOAT, 			// component type
						  GL_FALSE, 			// normalize
						  sizeof(vec3)*2, 		// stride
						  0); 					// start offset
	glEnableVertexAttribArray(positionIndex);
	
	GLuint colorIndex = program.getAttributeLocation("vertex_color");
	glVertexAttribPointer(colorIndex, 		// attrib index
						  3, 					// num components per attrib (3 float in vec3)
						  GL_FLOAT, 			// component type
						  GL_FALSE, 			// normalize
						  sizeof(vec3)*2, 		// stride
						  (void*)sizeof(vec3));	// start offset
	glEnableVertexAttribArray(colorIndex);
}

