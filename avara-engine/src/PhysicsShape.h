//
//  PhysicsShape.h
//	avara-engine
//
//  Created by Morgan Davis on 1/25/18.
//  Copyright © 2018 Morgan K Davis. All rights reserved.
//

#ifndef PhysicsShape_h
#define PhysicsShape_h


#include <memory>
#include <vector>

#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>

#include "Types.h"


namespace ae {
	
	
	class Geometry;
	class Node;
	class PhysicsBody;
	

	class PhysicsShape {
		
	public:
		
		/***************************************************************************************
		     MARK:   Lifecycle
		 **************************************************************************************/
		
		PhysicsShape(std::shared_ptr<Geometry> geometry, PhysicsShapeType type);
		//PhysicsShape(std::shared_ptr<Node> node, PhysicsShapeType type);
		
		/***************************************************************************************
		     MARK:   Public
		 **************************************************************************************/
		
		std::shared_ptr<Geometry> sourceGeometry() const;
		PhysicsShapeType type() const;
		/* ? */ std::vector<glm::mat4> transforms() const;
		
		/***************************************************************************************
		     MARK:   Internal
		 **************************************************************************************/
		
		void attachedToBody(PhysicsBody& body);
		
		PhysicsBody* physicsBody() const;
		void physicsBody(PhysicsBody* body);
		
		std::shared_ptr<btCollisionShape> btShape() const;
		
	private:
		
		/***************************************************************************************
		     MARK:   Private
		 **************************************************************************************/
		
		void createBTShape();
		
		std::shared_ptr<Geometry> 			m_sourceGeometry;
		PhysicsShapeType 					m_type;
		std::vector<glm::mat4> 				m_transforms;
		
		std::shared_ptr<btCollisionShape>	m_btShape;
		std::shared_ptr<btTriangleMesh>		m_btTriangleMesh;
		
		PhysicsBody*						m_physicsBody;
	};
}


#endif /* PhysicsShape_h */
