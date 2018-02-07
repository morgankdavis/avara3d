//
//  PhysicsDebugDrawer.h
//	avara-engine
//
//  Created by Morgan Davis on 2/6/18.
//  Copyright © 2018 Morgan K Davis. All rights reserved.
//

#ifndef PhysicsDebugDrawer_h
#define PhysicsDebugDrawer_h


#include <tuple>
#include <vector>

#include <glm/glm.hpp>
#include <LinearMath/btIDebugDraw.h>


namespace ae {
	
	
	//typedef std::tuple<btVector3, btVector3, btVector3> DebugLine;
	
	
	class Program;
	
	
	class PhysicsDebugDrawer : public btIDebugDraw {

	public:
		
		/***************************************************************************************
		     MARK:   Lifecycle
		 **************************************************************************************/
		
		PhysicsDebugDrawer();
		~PhysicsDebugDrawer();
		
		/***************************************************************************************
		     MARK:   Internal
		 **************************************************************************************/
		
		void clear();
		void draw(const glm::mat4& viewMat,
				  const glm::mat4& projectionMat);
		
		//std::vector<DebugLine> popDebugLines(); // returns all saved debug lines and clears the buffer

		/***************************************************************************************
		     MARK:   btIDebugDraw
		 **************************************************************************************/

		virtual void drawLine(const btVector3& from,
							  const btVector3& to,
							  const btVector3& color) override;
		
		virtual void drawLine(const btVector3& from,
							  const btVector3& to,
							  const btVector3& fromColor,
							  const btVector3& toColor) override;
		
		virtual void drawSphere (const btVector3& p,
								 btScalar radius,
								 const btVector3& color) override;
		
		virtual void drawTriangle(const btVector3& a,
								  const btVector3& b,
								  const btVector3& c,
								  const btVector3& color,
								  btScalar alpha) override;
		
		virtual void drawContactPoint(const btVector3& PointOnB,
									  const btVector3& normalOnB,
									  btScalar distance,
									  int lifeTime,
									  const btVector3& color) override;
		
		virtual void reportErrorWarning(const char* warningString) override;
		
		virtual void draw3dText(const btVector3& location,
								const char* textString) override;
		
		virtual void setDebugMode(int debugMode) override;
		virtual int getDebugMode() const override;
		
	protected:
		
		/***************************************************************************************
		     MARK:   Protected
		 **************************************************************************************/
		
		int 						m_debugMode;
		
	private:
		
		/***************************************************************************************
		     MARK:   Private
		 **************************************************************************************/
		
		void loadLinesVertexData(const Program& program);
		
		int							m_glLinesVBO;
		int							m_glLinesVAO;
		//std::vector<DebugLine>		m_lines;
		std::vector<glm::vec3>		m_lines; // from, to, color
	};
}

#endif /* PhysicsDebugDrawer_h */
