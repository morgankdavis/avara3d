//
//  BulletDebugDrawer.h
//  avara3d
//
//  Created by Morgan Davis on 2/6/18.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_BULLETDEBUGDRAWER_H
#define AVARA3D_BULLETDEBUGDRAWER_H

#include <memory>
#include <tuple>
#include <vector>

#include <bullet/LinearMath/btIDebugDraw.h>

namespace a3d {

	class Line;
	class Renderer;
	class RenderContext;
	
	class BulletDebugDrawer : public btIDebugDraw {

	public:
		/// Public Lifecycle Functions ///

		BulletDebugDrawer();
		~BulletDebugDrawer() override;
		
		/// Internal Member Functions ///

		void 	clear();

		std::vector<Line> lines();

//		std::vector<Line> lines(Renderer& renderer,
//					 const RenderContext& context,
//					 const math::mat4& viewMat,
//					 const math::mat4& projectionMat);
//		void 	draw(Renderer& renderer,
//					 const RenderContext& context,
//					 const math::mat4& viewMat,
//					 const math::mat4& projectionMat);

		/// btIDebugDraw Members ///

		void 	drawLine(const btVector3& from,
						 const btVector3& to,
						 const btVector3& color) override;

		void 	drawLine(const btVector3& from,
						 const btVector3& to,
						 const btVector3& fromColor,
						 const btVector3& toColor) override;

		/*
		void 	drawSphere(btScalar radius,
						   const btTransform& transform,
						   const btVector3& color) override;

		void 	drawSphere (const btVector3& p,
							btScalar radius,
							const btVector3& color) override;

		void drawTriangle(const btVector3& v0,
						  const btVector3& v1,
						  const btVector3& v2,
						  const btVector3& color,
						  btScalar alpha) override;

		 */

		// these three are pure virtual and must be implemented

		void drawContactPoint(const btVector3& PointOnB,
							  const btVector3& normalOnB,
							  btScalar distance,
							  int lifeTime,
							  const btVector3& color) override;

		void	reportErrorWarning(const char* warningString) override;

		void	draw3dText(const btVector3& location,
						   const char* textString) override;

		/*
		void 	drawAabb(const btVector3& from,
						 const btVector3& to,
						 const btVector3& color) override;

		void	drawTransform(const btTransform& transform,
							  btScalar orthoLen) override;

		void 	drawArc(const btVector3& center,
						const btVector3& normal,
						const btVector3& axis,
						btScalar radiusA,
						btScalar radiusB,
						btScalar minAngle,
						btScalar maxAngle,
						const btVector3& color,
						bool drawSect,
						btScalar stepDegrees = btScalar(10.f)) override;

		void 	drawSpherePatch(const btVector3& center,
								const btVector3& up,
								const btVector3& axis,
								btScalar radius,
								btScalar minTh,
								btScalar maxTh,
								btScalar minPs,
								btScalar maxPs,
								const btVector3& color,
								btScalar stepDegrees = btScalar(10.f),
								bool drawCenter = true) override;

		void 	drawBox(const btVector3& bbMin,
						const btVector3& bbMax,
						const btVector3& color) override;

		void 	drawBox(const btVector3& bbMin,
						const btVector3& bbMax,
						const btTransform& trans,
						const btVector3& color) override;

		void 	drawCapsule(btScalar radius,
							btScalar halfHeight,
							int upAxis,
							const btTransform& transform,
							const btVector3& color) override;

		void 	drawCylinder(btScalar radius,
							 btScalar halfHeight,
							 int upAxis,
							 const btTransform& transform,
							 const btVector3& color) override;

		void 	drawCone(btScalar radius,
						 btScalar height,
						 int upAxis,
						 const btTransform& transform,
						 const btVector3& color) override;

		void 	drawPlane(const btVector3& planeNormal,
						  btScalar planeConst,
						  const btTransform& transform,
						  const btVector3& color) override;
		*/

		void 	setDebugMode(int debugMode) override;
		int 	getDebugMode() const override;

	private:
		/// Private Member Variables ///

		int							_debugMode;
		std::vector<Line>			_lines;
	};
}

#endif /* AVARA3D_BULLETDEBUGDRAWER_H */
