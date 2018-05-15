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


#include "Types.h"


namespace ae {
	
	
	class Geometry;
	class Node;
	class PhysicsBody;
	

	class PhysicsShape {
		
	public:
		
		/***************************************************************************************
		     Lifecycle
		 ***************************************************************************************/
		
		PhysicsShape(std::shared_ptr<Geometry> geometry, PHYSICS_SHAPE_TYPE type);
		PhysicsShape(std::shared_ptr<Node> node, PHYSICS_SHAPE_TYPE type);
		
		/***************************************************************************************
		     Public
		 ***************************************************************************************/
		
		std::shared_ptr<Geometry> sourceGeometry() const;
		std::shared_ptr<Node> sourceNode() const;
		PHYSICS_SHAPE_TYPE type() const;
		std::vector<glm::mat4> transforms() const;
		
		/***************************************************************************************
		     Internal
		 ***************************************************************************************/
		
		void attachedToBody(std::shared_ptr<PhysicsBody> body);
		
#warning may want this
//		std::vector<std::shared_ptr<btCollisionShape>>& childShapes();
		
		std::weak_ptr<PhysicsBody> physicsBody() const;
		void physicsBody(std::shared_ptr<PhysicsBody> body);

		PHYSICS_SHAPE_ID simulationID() const;
		void simulationID(PHYSICS_SHAPE_ID simID);
		
		PHYSICS_SHAPE_DIRTY_BITS dirtyBits() const;
		void dirtyBits(PHYSICS_SHAPE_DIRTY_BITS bits);
		
	private:
		
		/***************************************************************************************
		     Private
		 ***************************************************************************************/
		
		std::shared_ptr<Geometry> 						m_sourceGeometry;
		std::shared_ptr<Node> 							m_sourceNode;
		PHYSICS_SHAPE_TYPE 								m_type;
//		std::vector<std::shared_ptr<btCollisionShape>>	m_childShapes;
		// The array of transforms that was used to create a compound shape.
		std::vector<glm::mat4> 							m_transforms;

		std::weak_ptr<PhysicsBody>						m_physicsBody;
		
		PHYSICS_SHAPE_ID 								m_simulationID;
		PHYSICS_SHAPE_DIRTY_BITS 						m_dirtyBits;
	};
}


#endif /* PhysicsShape_h */
