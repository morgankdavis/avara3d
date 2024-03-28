//
//  BulletDebugDrawer.cc
//	avara3d
//
//  Created by Morgan Davis on 2/6/18.
//  Copyright © 2018 Morgan K Davis. All rights reserved.
//

#ifdef OPENGL_DESKTOP

#include "a3d/physics/bullet/BulletDebugDrawer.h"

#include <set>

#include "a3d/Color.h"
#include "a3d/mesh/Line.h"
#include "a3d/diagnostic/logging/Logger.h"
#include "a3d/rendering/Renderer.h"


using namespace a3d;
using namespace glm;
using namespace std;


/*********************************************************************************************
	Static Prototypes
 *********************************************************************************************/

static vec3 		GLMVec3FromBTVector3(const btVector3& from);
static vec4 		GLMVec4FromBTVector4(const btVector4& from);
static btVector3 	BTVector3FromGLMVec3(const vec3& from);
static btVector4 	BTVector4FromGLMVec4(const vec4& from);

/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

BulletDebugDrawer::BulletDebugDrawer():
	_debugMode{btIDebugDraw::DBG_NoDebug},
	_lineSet{make_unique<LineSet>()} {

}

BulletDebugDrawer::~BulletDebugDrawer() {

}

/*********************************************************************************************
	Internal
 *********************************************************************************************/

void BulletDebugDrawer::clear() {
	// have to replace shared_ptr for Renderer to reload the data
	_lineSet = make_unique<LineSet>();
}

void BulletDebugDrawer::draw(Renderer& renderer,
							 const mat4& viewMat,
							 const mat4& projectionMat) {
	
	if (getDebugMode() != btIDebugDraw::DBG_NoDebug) {
		A3D_LOG_T("BulletDebugDrawer::update()");

		renderer.render(*_lineSet, mat4(1.0), viewMat, projectionMat);
	}
}

/*********************************************************************************************
	btIDebugDraw
 *********************************************************************************************/

void BulletDebugDrawer::drawLine(const btVector3& from,
								 const btVector3& to,
								 const btVector3& color) {
	//A3D_LOG_D("drawLine() - 1 color");
	
	drawLine(from, to, color, color);
}

void BulletDebugDrawer::drawLine(const btVector3& from,
								 const btVector3& to,
								 const btVector3& fromColor,
								 const btVector3& toColor) {
	//A3D_LOG_D("drawLine() - 2 colors");

//	auto line = Line(GLMVec3FromBTVector3(from),
//					 GLMVec3FromBTVector3(to),
//					 make_shared<Color>(fromColor.x(), fromColor.y(), fromColor.z(), 1.0),
//					 make_shared<Color>(toColor.x(), toColor.y(), toColor.z(), 1.0));
//
//	_lineSet->emplace(line);
}

/*
void BulletDebugDrawer::drawSphere(btScalar radius,
									const btTransform& transform,
									const btVector3& color) {
	//A3D_LOG_D(A3D_FUNC);
}

void BulletDebugDrawer::drawSphere (const btVector3& p,
									 btScalar radius,
									 const btVector3& color) {
	//A3D_LOG_D(A3D_FUNC);
}

void BulletDebugDrawer::drawTriangle(const btVector3& v0,
									  const btVector3& v1,
									  const btVector3& v2,
									  const btVector3& color,
									  btScalar alpha) {
	//A3D_LOG_D(A3D_FUNC);
}
*/

void BulletDebugDrawer::drawContactPoint(const btVector3& pointOnB,
										 const btVector3& normalOnB,
										 btScalar distance,
										 int lifeTime,
										 const btVector3& color) {
	//A3D_LOG_D(A3D_FUNC);
	
	const float DISTANCE_EXTENSION = 0.0;
	btVector3 to = pointOnB + normalOnB * (distance + DISTANCE_EXTENSION);
	drawLine(pointOnB, to, color, color);
}

void BulletDebugDrawer::reportErrorWarning(const char* warningString) {
	A3D_LOG_W("Bullet warning: {}", warningString);
}

void BulletDebugDrawer::draw3dText(const btVector3& location,
								   const char* textString) {
	//A3D_LOG_D(A3D_FUNC);
}

/*
void BulletDebugDrawer::drawAabb(const btVector3 &from,
								  const btVector3 &to,
								  const btVector3 &color) {
	//A3D_LOG_D(A3D_FUNC);
}

void BulletDebugDrawer::drawTransform(const btTransform &transform,
									   btScalar orthoLen) {
	//A3D_LOG_D(A3D_FUNC);
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
	//A3D_LOG_D(A3D_FUNC);
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
	//A3D_LOG_D(A3D_FUNC);
}

void BulletDebugDrawer::drawBox(const btVector3& bbMin,
								 const btVector3& bbMax,
								 const btVector3& color) {
	//A3D_LOG_D(A3D_FUNC);
}

void BulletDebugDrawer::drawBox(const btVector3& bbMin,
								 const btVector3& bbMax,
								 const btTransform& trans,
								 const btVector3& color) {
	//A3D_LOG_D(A3D_FUNC);
}

void BulletDebugDrawer::drawCapsule(btScalar radius,
									 btScalar halfHeight,
									 int upAxis,
									 const btTransform& transform,
									 const btVector3& color) {
	//A3D_LOG_D(A3D_FUNC);
}

void BulletDebugDrawer::drawCylinder(btScalar radius,
									  btScalar halfHeight,
									  int upAxis,
									  const btTransform& transform,
									  const btVector3& color) {
	//A3D_LOG_D(A3D_FUNC);
}

void BulletDebugDrawer::drawCone(btScalar radius,
								  btScalar height,
								  int upAxis,
								  const btTransform& transform,
								  const btVector3& color) {
	//A3D_LOG_D(A3D_FUNC);
}

void BulletDebugDrawer::drawPlane(const btVector3& planeNormal,
								   btScalar planeConst,
								   const btTransform& transform,
								   const btVector3& color) {
	//A3D_LOG_D(A3D_FUNC);
}
*/

void BulletDebugDrawer::setDebugMode(int debugMode) {
	_debugMode = debugMode;
}

int BulletDebugDrawer::getDebugMode() const {
	return _debugMode;
}

/*********************************************************************************************
	Static
 *********************************************************************************************/

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

#endif // OPENGL_DESKTOP
