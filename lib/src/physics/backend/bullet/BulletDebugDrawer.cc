//
//  BulletDebugDrawer.cc
//  avara3d
//
//  Created by Morgan Davis on 2/6/18.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#include "a3d/physics/backend/bullet/BulletDebugDrawer.h"

#include "a3d/mesh/Line.h"
#include "a3d/log/Log.h"
#include "a3d/physics/backend/bullet/BulletUtilities.h"

using namespace a3d::math;
using namespace std;

namespace a3d {
namespace {

    // [Private Constants]

    constexpr float CONTACT_POINT_RADIUS = 0.04f;
    constexpr float CONTACT_NORMAL_LENGTH = 0.15f;

} // namespace

// [Public Lifecycle Functions]

BulletDebugDrawer::BulletDebugDrawer():
    _debugMode {btIDebugDraw::DBG_NoDebug},
    _lines {} {}

BulletDebugDrawer::~BulletDebugDrawer() {}

// [Internal Member Functions]

void BulletDebugDrawer::drawFrame(const btTransform& transform, const btVector3& size, btScalar tipMargin) {

    const btVector3    origin = transform.getOrigin();
    const btMatrix3x3& basis = transform.getBasis();

    const btVector3 x = basis.getColumn(0);
    const btVector3 y = basis.getColumn(1);
    const btVector3 z = basis.getColumn(2);

    const btVector3 xEnd = origin + x * size.x();
    const btVector3 yEnd = origin + y * size.y();
    const btVector3 zEnd = origin + z * size.z();

    drawLine(origin, xEnd, btVector3 {1.0f, 0.3f, 0.3f});
    drawLine(origin, yEnd, btVector3 {0.3f, 1.0f, 0.3f});
    drawLine(origin, zEnd, btVector3 {0.3f, 0.3f, 1.0f});

    const btVector3 yellow {1.0f, 1.0f, 0.0f};

    drawLine(xEnd, xEnd + x * size.x() * tipMargin, yellow);
    drawLine(yEnd, yEnd + y * size.y() * tipMargin, yellow);
    drawLine(zEnd, zEnd + z * size.z() * tipMargin, yellow);
}

void BulletDebugDrawer::clear() {
    _lines.clear();
}

vector<Line> BulletDebugDrawer::lines() {
    return std::move(_lines);
}

// void BulletDebugDrawer::draw(Renderer&            renderer,
//                              const RenderContext& context,
//                              const mat4&          viewMat,
//                              const mat4&          projectionMat) {
//
//     static const auto M4_I = mat4(1.0);
//     if (getDebugMode() != btIDebugDraw::DBG_NoDebug) {
//         //renderer.render(_lines, context, M4_I, viewMat, projectionMat);
//     }
// }
//
// std::vector<Line> BulletDebugDrawer::lines(Renderer&            renderer,
//                                            const RenderContext& context,
//                                            const math::mat4&    viewMat,
//                                            const math::mat4&    projectionMat) {}

// [btIDebugDraw Members]

void BulletDebugDrawer::drawLine(const btVector3& from, const btVector3& to, const btVector3& color) {

    drawLine(from, to, color, color);
}

void BulletDebugDrawer::drawLine(const btVector3& from,
                                 const btVector3& to,
                                 const btVector3& fromColor,
                                 const btVector3& toColor) {

    _lines.push_back(Line(A3DVec3FromBTVector3(from), A3DVec3FromBTVector3(to), A3DVec3FromBTVector3(fromColor),
                          A3DVec3FromBTVector3(toColor)));
}

// void BulletDebugDrawer::drawSphere(btScalar radius, const btTransform& transform, const btVector3& color) {
//     log::w()("(Not implemented)");
// }
//
// void BulletDebugDrawer::drawSphere(const btVector3& p, btScalar radius, const btVector3& color) {
//     log::w()("(Not implemented)");
// }
//
// void BulletDebugDrawer::drawTriangle(const btVector3& v0,
//                                      const btVector3& v1,
//                                      const btVector3& v2,
//                                      const btVector3& color,
//                                      btScalar         alpha) {
//     log::w()("(Not implemented)");
// }

// these three are pure virtual and must be implemented

void BulletDebugDrawer::drawContactPoint(const btVector3& pointOnB,
                                         const btVector3& normalOnB,
                                         btScalar /*distance*/,
                                         int /*lifeTime*/,
                                         const btVector3& color) {

    const btVector3 normal = normalOnB.normalized();

    btVector3 tangent;
    btVector3 bitangent;
    btPlaneSpace1(normal, tangent, bitangent);

    // point marker, lying in the contact plane
    drawLine(pointOnB - tangent * CONTACT_POINT_RADIUS, pointOnB + tangent * CONTACT_POINT_RADIUS, color);
    drawLine(pointOnB - bitangent * CONTACT_POINT_RADIUS, pointOnB + bitangent * CONTACT_POINT_RADIUS, color);

    // normal
    drawLine(pointOnB, pointOnB + normal * CONTACT_NORMAL_LENGTH, color);
}

void BulletDebugDrawer::reportErrorWarning(const char* warningString) {
    log::w()("BULLET WARNING: {}", warningString);
}

// TODO: do this some day
void BulletDebugDrawer::draw3dText(const btVector3& location, const char* textString) {
    log::w()("(Not implemented)");
}

// if implemented, called instead of drawLine()
// void BulletDebugDrawer::drawAabb(const btVector3& from, const btVector3& to, const btVector3& color) {
//     log::w()("(Not implemented)");
// }
//
// void BulletDebugDrawer::drawTransform(const btTransform& transform, btScalar orthoLen) {
//     log::w()("(Not implemented)");
// }
//
// void BulletDebugDrawer::drawArc(const btVector3& center,
//                                 const btVector3& normal,
//                                 const btVector3& axis,
//                                 btScalar         radiusA,
//                                 btScalar         radiusB,
//                                 btScalar         minAngle,
//                                 btScalar         maxAngle,
//                                 const btVector3& color,
//                                 bool             drawSect,
//                                 btScalar         stepDegrees) {
//     log::w()("(Not implemented)");
// }
//
// void BulletDebugDrawer::drawSpherePatch(const btVector3& center,
//                                         const btVector3& up,
//                                         const btVector3& axis,
//                                         btScalar         radius,
//                                         btScalar         minTh,
//                                         btScalar         maxTh,
//                                         btScalar         minPs,
//                                         btScalar         maxPs,
//                                         const btVector3& color,
//                                         btScalar         stepDegrees,
//                                         bool             drawCenter) {
//     log::w()("(Not implemented)");
// }
//
// void BulletDebugDrawer::drawBox(const btVector3& bbMin, const btVector3& bbMax, const btVector3& color) {
//     log::w()("(Not implemented)");
// }
//
// void BulletDebugDrawer::drawBox(const btVector3&   bbMin,
//                                 const btVector3&   bbMax,
//                                 const btTransform& trans,
//                                 const btVector3&   color) {
//     log::w()("(Not implemented)");
// }
//
// void BulletDebugDrawer::drawCapsule(btScalar           radius,
//                                     btScalar           halfHeight,
//                                     int                upAxis,
//                                     const btTransform& transform,
//                                     const btVector3&   color) {
//     log::w()("(Not implemented)");
// }
//
// void BulletDebugDrawer::drawCylinder(btScalar           radius,
//                                      btScalar           halfHeight,
//                                      int                upAxis,
//                                      const btTransform& transform,
//                                      const btVector3&   color) {
//     log::w()("(Not implemented)");
// }
//
// void BulletDebugDrawer::drawCone(btScalar           radius,
//                                  btScalar           height,
//                                  int                upAxis,
//                                  const btTransform& transform,
//                                  const btVector3&   color) {
//     log::w()("(Not implemented)");
// }
//
// void BulletDebugDrawer::drawPlane(const btVector3&   planeNormal,
//                                   btScalar           planeConst,
//                                   const btTransform& transform,
//                                   const btVector3&   color) {
//     log::w()("(Not implemented)");
// }

void BulletDebugDrawer::setDebugMode(int debugMode) {
    _debugMode = debugMode;
}

int BulletDebugDrawer::getDebugMode() const {
    return _debugMode;
}

} // namespace a3d
