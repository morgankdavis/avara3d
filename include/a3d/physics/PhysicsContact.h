//
//  PhysicsContact.h
//  avara3d
//
//  Created by Morgan Davis on 1/25/18.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_PHYSICSCONTACT_H
#define AVARA3D_PHYSICSCONTACT_H


#include <memory>

#include "glm/glm.hpp"

#include "a3d/Types.h"


namespace a3d {
	
	
	class Node;
	

	class PhysicsContact {

/*********************************************************************************************
	Public Lifecycle Functions
 *********************************************************************************************/

	public:

		PhysicsContact();

/*********************************************************************************************
	Public Member Functions
 *********************************************************************************************/

		std::weak_ptr<Node>				nodeA() const;
		std::weak_ptr<Node>				nodeB() const;
		const glm::vec3& 				contactPoint() const;
		const glm::vec3& 				contactNormal() const;
		float 							collisionImpulse() const;
		float 							penetrationDistance() const;
		float 							sweepTestFraction() const;

/*********************************************************************************************
	Private Member Variables
 *********************************************************************************************/

	private:

		std::shared_ptr<Node> 			_nodeA;
		std::shared_ptr<Node>			_nodeB;
		glm::vec3 						_contactPoint;
		glm::vec3 						_contactNormal;
		float							_collisionImpulse;
		float							_penetrationDistance;
		float							_sweepTestFraction;
	};
}


#endif /* AVARA3D_PHYSICSCONTACT_H */
