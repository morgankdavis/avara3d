//
//  BulletDebugDrawer.cc
//	avara-engine
//
//  Created by Morgan Davis on 2/6/18.
//  Copyright © 2018 Morgan K Davis. All rights reserved.
//

#ifdef DESKTOP

#include "BulletDebugDrawer.h"

#include <set>

#include <GL/glew.h>

#include "Color.h"
#include "Line.h"
#include "Logger.h"
#include "Program.h"
#include "Renderer.h"
#include "Utilities.h"


using namespace ae;
using namespace ae::utils;
using namespace glm;
using namespace std;


/**************************************************************************************
     Static Prototypes
 **************************************************************************************/

static vec3 GLMVec3FromBTVector3(const btVector3& from);
static vec4 GLMVec4FromBTVector4(const btVector4& from);
static btVector3 BTVector3FromGLMVec3(const vec3& from);
static btVector4 BTVector4FromGLMVec4(const vec4& from);

/***************************************************************************************
     Lifecycle
 ***************************************************************************************/

BulletDebugDrawer::BulletDebugDrawer():
	m_debugMode(btIDebugDraw::DBG_NoDebug),
	m_lineSet(make_shared<LineSet>()) {

}

BulletDebugDrawer::~BulletDebugDrawer() {

}

/***************************************************************************************
     Internal
 ***************************************************************************************/

void BulletDebugDrawer::clear() {
	// have to replace shared_ptr for Renderer to reload the data
	m_lineSet = make_shared<LineSet>();
}

void BulletDebugDrawer::draw(Renderer& renderer,
							 const mat4& viewMat,
							 const mat4& projectionMat) {
	
	if (getDebugMode() != btIDebugDraw::DBG_NoDebug) {
		AE_LOG_T("BulletDebugDrawer::draw()");

		renderer.render(m_lineSet, mat4(1.0), viewMat, projectionMat);
	}
}

/***************************************************************************************
     btIDebugDraw
 ***************************************************************************************/

void BulletDebugDrawer::drawLine(const btVector3& from,
								  const btVector3& to,
								  const btVector3& color) {
	//AE_LOG_D("drawLine() - 1 color");
	
	drawLine(from, to, color, color);
}

void BulletDebugDrawer::drawLine(const btVector3& from,
								  const btVector3& to,
								  const btVector3& fromColor,
								  const btVector3& toColor) {
	//AE_LOG_D("drawLine() - 2 colors");

	m_lineSet->emplace(make_shared<Line>(GLMVec3FromBTVector3(from),
										 GLMVec3FromBTVector3(to),
										 make_shared<Color>(fromColor.x(), fromColor.y(), fromColor.z(), 1.0),
										 make_shared<Color>(toColor.x(), toColor.y(), toColor.z(), 1.0)));
}

/*
void BulletDebugDrawer::drawSphere(btScalar radius,
									const btTransform& transform,
									const btVector3& color) {
	//AE_LOG_D(AE_FUNC);
}

void BulletDebugDrawer::drawSphere (const btVector3& p,
									 btScalar radius,
									 const btVector3& color) {
	//AE_LOG_D(AE_FUNC);
}

void BulletDebugDrawer::drawTriangle(const btVector3& v0,
									  const btVector3& v1,
									  const btVector3& v2,
									  const btVector3& color,
									  btScalar alpha) {
	//AE_LOG_D(AE_FUNC);
}
*/

void BulletDebugDrawer::drawContactPoint(const btVector3& pointOnB,
										  const btVector3& normalOnB,
										  btScalar distance,
										  int lifeTime,
										  const btVector3& color) {
	//AE_LOG_D(AE_FUNC);
	
	const float DISTANCE_EXTENSION = 0.0;
	btVector3 to = pointOnB + normalOnB * (distance + DISTANCE_EXTENSION);
	drawLine(pointOnB, to, color, color);
}

void BulletDebugDrawer::reportErrorWarning(const char* warningString) {
	AE_LOG_W("Bullet warning: {}", warningString);
}

void BulletDebugDrawer::draw3dText(const btVector3& location,
									const char* textString) {
	//AE_LOG_D(AE_FUNC);
}

/*
void BulletDebugDrawer::drawAabb(const btVector3 &from,
								  const btVector3 &to,
								  const btVector3 &color) {
	//AE_LOG_D(AE_FUNC);
}

void BulletDebugDrawer::drawTransform(const btTransform &transform,
									   btScalar orthoLen) {
	//AE_LOG_D(AE_FUNC);
}

void BulletDebugDrawer::drawArc(const btVector3& center,
								 const btVector3& normal,
								 const btVector3& axis,
								 btScalar radiusA,
								 btScalar radiusB,
								 btScalar minAngle,
								 btScalar maxAngle,
								 const btVector3& color,
								 bool drawSect,
								 btScalar stepDegrees) {
	//AE_LOG_D(AE_FUNC);
}

void BulletDebugDrawer::drawSpherePatch(const btVector3& center,
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
	//AE_LOG_D(AE_FUNC);
}

void BulletDebugDrawer::drawBox(const btVector3& bbMin,
								 const btVector3& bbMax,
								 const btVector3& color) {
	//AE_LOG_D(AE_FUNC);
}

void BulletDebugDrawer::drawBox(const btVector3& bbMin,
								 const btVector3& bbMax,
								 const btTransform& trans,
								 const btVector3& color) {
	//AE_LOG_D(AE_FUNC);
}

void BulletDebugDrawer::drawCapsule(btScalar radius,
									 btScalar halfHeight,
									 int upAxis,
									 const btTransform& transform,
									 const btVector3& color) {
	//AE_LOG_D(AE_FUNC);
}

void BulletDebugDrawer::drawCylinder(btScalar radius,
									  btScalar halfHeight,
									  int upAxis,
									  const btTransform& transform,
									  const btVector3& color) {
	//AE_LOG_D(AE_FUNC);
}

void BulletDebugDrawer::drawCone(btScalar radius,
								  btScalar height,
								  int upAxis,
								  const btTransform& transform,
								  const btVector3& color) {
	//AE_LOG_D(AE_FUNC);
}

void BulletDebugDrawer::drawPlane(const btVector3& planeNormal,
								   btScalar planeConst,
								   const btTransform& transform,
								   const btVector3& color) {
	//AE_LOG_D(AE_FUNC);
}
*/

void BulletDebugDrawer::setDebugMode(int debugMode) {
	m_debugMode = debugMode;
}

int BulletDebugDrawer::getDebugMode() const {
	return m_debugMode;
}

/**************************************************************************************
     Static
 **************************************************************************************/

vec3 GLMVec3FromBTVector3(const btVector3& from) {
	return vec3(from.x(), from.y(), from.z());
}

vec4 GLMVec4FromBTVector4(const btVector4& from) {
	return vec4(from.x(), from.y(), from.z(), from.w());
}

btVector3 BTVector3FromGLMVec3(const vec3& from) {
	return btVector3(from.x, from.y, from.z);
}

btVector4 BTVector4FromGLMVec4(const vec4& from) {
	return btVector4(from.x, from.y, from.z, from.w);
}

#endif // DESKTOP
