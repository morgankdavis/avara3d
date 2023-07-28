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
		
/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

	public:

		PhysicsContact();
		
/*********************************************************************************************
	Public
 *********************************************************************************************/

		std::shared_ptr<ae::Node>	nodeA() const;
		std::shared_ptr<ae::Node> 	nodeB() const;
		glm::vec3 					contactPoint() const;
		glm::vec3 					contactNormal() const;
		float 						collisionImpulse() const;
		float 						penetrationDistance() const;
		float 						sweepTestFraction() const;
		
/*********************************************************************************************
	Private
 *********************************************************************************************/

	private:

		std::shared_ptr<ae::Node> 	_nodeA;
		std::shared_ptr<ae::Node>	_nodeB;
		glm::vec3 					_contactPoint;
		glm::vec3 					_contactNormal;
		float						_collisionImpulse;
		float						_penetrationDistance;
		float						_sweepTestFraction;
	};
}


#endif /* PhysicsContact_h */
