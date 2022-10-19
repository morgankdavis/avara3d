//
//  PhysicsContact.h
//	avara-engine
//
//  Created by Morgan Davis on 1/25/18.
//  Copyright © 2018 Morgan K Davis. All rights reserved.
//

#ifndef PhysicsContact_h
#define PhysicsContact_h


#include <memory>

#include <glm/glm.hpp>

#include "Types.h"


namespace ae {
	
	
	class Node;
	

	class PhysicsContact : public std::enable_shared_from_this<PhysicsContact> {
		
	public:
		
/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/
		
		PhysicsContact();
		
/*********************************************************************************************
	Public
 *********************************************************************************************/
		
		std::shared_ptr<Node> nodeA() const;
		std::shared_ptr<Node> nodeB() const;
		glm::vec3 contactPoint() const;
		glm::vec3 contactNormal() const;
		float collisionImpulse() const;
		float penetrationDistance() const;
		float sweepTestFraction() const;
		
	private:
		
/*********************************************************************************************
	Private
 *********************************************************************************************/
		
		std::shared_ptr<Node> 		m_nodeA;
		std::shared_ptr<Node> 		m_nodeB;
		glm::vec3 					m_contactPoint;
		glm::vec3 					m_contactNormal;
		float 						m_collisionImpulse;
		float 						m_penetrationDistance;
		float 						m_sweepTestFraction;
	};
}


#endif /* PhysicsContact_h */
