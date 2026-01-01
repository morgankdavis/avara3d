//
//  BulletDebugDrawer.cc
//  avara3d
//
//  Created by Morgan Davis on 2/6/18.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#ifdef A3D_GL_DESKTOP

#include "a3d/physics/bullet/BulletDebugDrawer.h"

#include "a3d/mesh/Line.h"
#include "a3d/diagnostic/log/Log.h"
#include "a3d/physics/bullet/BulletUtilities.h"
#include "a3d/rendering/renderer/Renderer.h"

using namespace a3d;
using namespace a3d::math;
using namespace std;

/// Public Lifecycle Functions ///

BulletDebugDrawer::BulletDebugDrawer():
	_debugMode{btIDebugDraw::DBG_NoDebug},
	_lines{} { }

BulletDebugDrawer::~BulletDebugDrawer() { }

/// Internal Member Functions ///

void BulletDebugDrawer::clear() {
	_lines.clear();
}

vector<Line> BulletDebugDrawer::lines() {
	return std::move(_lines);
}

//void BulletDebugDrawer::draw(Renderer& renderer,
//							 const RenderContext& context,
//							 const mat4& viewMat,
//							 const mat4& projectionMat) {
//
//	static const auto M4_I = mat4(1.0);
//	if (getDebugMode() != btIDebugDraw::DBG_NoDebug) {
//		//renderer.render(_lines, context, M4_I, viewMat, projectionMat);
//	}
//}

//std::vector<Line> BulletDebugDrawer::lines(Renderer& renderer,
//						const RenderContext& context,
//						const math::mat4& viewMat,
//						const math::mat4& projectionMat) {
//
//
//}

/// btIDebugDraw Members ///

void BulletDebugDrawer::drawLine(const btVector3& from,
								 const btVector3& to,
								 const btVector3& color) {

	drawLine(from, to, color, color);
}

void BulletDebugDrawer::drawLine(const btVector3& from,
								 const btVector3& to,
								 const btVector3& fromColor,
								 const btVector3& toColor) {

	_lines.push_back(Line(A3DVec3FromBTVector3(from),
						  A3DVec3FromBTVector3(to),
						  A3DVec3FromBTVector3(fromColor),
						  A3DVec3FromBTVector3(toColor)));
}

/*
void BulletDebugDrawer::drawSphere(btScalar radius,
								   const btTransform& transform,
								   const btVector3& color) {
	log::w()("(Not implemented)");
}

void BulletDebugDrawer::drawSphere(const btVector3& p,
								   btScalar radius,
								   const btVector3& color) {
	log::w()("(Not implemented)");
}

void BulletDebugDrawer::drawTriangle(const btVector3& v0,
									 const btVector3& v1,
									 const btVector3& v2,
									 const btVector3& color,
									 btScalar alpha) {
	log::w()("(Not implemented)");
}

*/

// these three are pure virtual and must be implemented

void BulletDebugDrawer::drawContactPoint(const btVector3& pointOnB,
										 const btVector3& normalOnB,
										 btScalar distance,
										 int lifeTime,
										 const btVector3& color) {

	const float DISTANCE_EXTENSION = 0.0;
	btVector3 to = pointOnB + normalOnB * (distance + DISTANCE_EXTENSION);
	drawLine(pointOnB, to, color, color);
}

void BulletDebugDrawer::reportErrorWarning(const char* warningString) {
	log::w()(std::format("Bullet warning: {}", warningString));
}

void BulletDebugDrawer::draw3dText(const btVector3& location,
								   const char* textString) {
	log::w()("(Not implemented)");
}

/*

// if implemented, called instead of drawLine()
//void BulletDebugDrawer::drawAabb(const btVector3 &from,
//								  const btVector3 &to,
//								  const btVector3 &color) {
//	log::w()("(Not implemented)");
//}

void BulletDebugDrawer::drawTransform(const btTransform &transform,
									  btScalar orthoLen) {
	log::w()("(Not implemented)");
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
	log::w()("(Not implemented)");
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
	log::w()("(Not implemented)");
}

void BulletDebugDrawer::drawBox(const btVector3& bbMin,
								const btVector3& bbMax,
								const btVector3& color) {
	log::w()("(Not implemented)");
}

void BulletDebugDrawer::drawBox(const btVector3& bbMin,
								const btVector3& bbMax,
								const btTransform& trans,
								const btVector3& color) {
	log::w()("(Not implemented)");
}

void BulletDebugDrawer::drawCapsule(btScalar radius,
									btScalar halfHeight,
									int upAxis,
									const btTransform& transform,
									const btVector3& color) {
	log::w()("(Not implemented)");
}

void BulletDebugDrawer::drawCylinder(btScalar radius,
									 btScalar halfHeight,
									 int upAxis,
									 const btTransform& transform,
									 const btVector3& color) {
	log::w()("(Not implemented)");
}

void BulletDebugDrawer::drawCone(btScalar radius,
								 btScalar height,
								 int upAxis,
								 const btTransform& transform,
								 const btVector3& color) {
	log::w()("(Not implemented)");
}

void BulletDebugDrawer::drawPlane(const btVector3& planeNormal,
								  btScalar planeConst,
								  const btTransform& transform,
								  const btVector3& color) {
	log::w()("(Not implemented)");
}
*/

void BulletDebugDrawer::setDebugMode(int debugMode) {
	_debugMode = debugMode;
}

int BulletDebugDrawer::getDebugMode() const {
	return _debugMode;
}

#endif // A3D_GL_DESKTOP
