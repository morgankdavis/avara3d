//
//  BulletDebugDrawer.h
//	avara-engine
//
//  Created by Morgan Davis on 2/6/18.
//  Copyright © 2018 Morgan K Davis. All rights reserved.
//

#ifndef BulletDebugDrawer_h
#define BulletDebugDrawer_h

#ifdef DESKTOP


#include <memory>
#include <tuple>
#include <vector>

#include <glm/glm.hpp>
#include <LinearMath/btIDebugDraw.h>

#include "Types.h"


namespace ae {
	

	class Renderer;
	
	
	class BulletDebugDrawer : public btIDebugDraw {

	public:
		
/*********************************************************************************************
     Lifecycle
 *********************************************************************************************/

		BulletDebugDrawer();
		~BulletDebugDrawer();
		
/*********************************************************************************************
     Internal
 *********************************************************************************************/
		
		void clear();
		void draw(Renderer& renderer,
				  const glm::mat4& viewMat,
				  const glm::mat4& projectionMat);

/*********************************************************************************************
     btIDebugDraw
 *********************************************************************************************/

		virtual void drawLine(const btVector3& from,
							  const btVector3& to,
							  const btVector3& color) override;
		
		virtual void drawLine(const btVector3& from,
							  const btVector3& to,
							  const btVector3& fromColor,
							  const btVector3& toColor) override;
		
		/*
		virtual void drawSphere(btScalar radius,
								const btTransform& transform,
								const btVector3& color) override;
		
		virtual void drawSphere (const btVector3& p,
								 btScalar radius,
								 const btVector3& color) override;
		
		virtual void drawTriangle(const btVector3& v0,
								  const btVector3& v1,
								  const btVector3& v2,
								  const btVector3& color,
								  btScalar alpha) override;
		*/
		
		virtual void drawContactPoint(const btVector3& PointOnB,
									  const btVector3& normalOnB,
									  btScalar distance,
									  int lifeTime,
									  const btVector3& color) override;
		
		virtual void reportErrorWarning(const char* warningString) override;
		
		virtual void draw3dText(const btVector3& location,
								const char* textString) override;
		
		/*
		virtual void drawAabb(const btVector3& from,
							  const btVector3& to,
							  const btVector3& color) override;
		
		virtual void drawTransform(const btTransform& transform,
								   btScalar orthoLen) override;
		
		virtual void drawArc(const btVector3& center,
							 const btVector3& normal,
							 const btVector3& axis,
							 btScalar radiusA,
							 btScalar radiusB,
							 btScalar minAngle,
							 btScalar maxAngle,
							 const btVector3& color,
							 bool drawSect,
							 btScalar stepDegrees = btScalar(10.f)) override;
		
		virtual void drawSpherePatch(const btVector3& center,
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
		
		virtual void drawBox(const btVector3& bbMin,
							 const btVector3& bbMax,
							 const btVector3& color) override;
		
		virtual void drawBox(const btVector3& bbMin,
							 const btVector3& bbMax,
							 const btTransform& trans,
							 const btVector3& color) override;
		
		virtual void drawCapsule(btScalar radius,
								 btScalar halfHeight,
								 int upAxis,
								 const btTransform& transform,
								 const btVector3& color) override;
		
		virtual void drawCylinder(btScalar radius,
								  btScalar halfHeight,
								  int upAxis,
								  const btTransform& transform,
								  const btVector3& color) override;
		
		virtual void drawCone(btScalar radius,
							  btScalar height,
							  int upAxis,
							  const btTransform& transform,
							  const btVector3& color) override;
		
		virtual void drawPlane(const btVector3& planeNormal,
							   btScalar planeConst,
							   const btTransform& transform,
							   const btVector3& color) override;
		 */

		virtual void setDebugMode(int debugMode) override;
		virtual int getDebugMode() const override;
		
	protected:
		
/*********************************************************************************************
     Protected
 *********************************************************************************************/
		
		int							m_debugMode;
		
	private:
		
/*********************************************************************************************
     Private
 *********************************************************************************************/
	
		std::shared_ptr<LineSet>	m_lineSet;
	};
}

#endif // DESKTOP

#endif /* BulletDebugDrawer_h */
