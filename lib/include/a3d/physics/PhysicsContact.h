//
//  PhysicsContact.h
//  avara3d
//
//  Created by Morgan Davis on 1/25/18.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_PHYSICS_PHYSICSCONTACT_H
#define AVARA3D_PHYSICS_PHYSICSCONTACT_H

#include <memory>

#include "a3d/Math.h"

namespace a3d {

	class Node;

	class PhysicsContact {

	public:
		/// Public Lifecycle Functions ///

		PhysicsContact();

		/// Public Member Functions ///

		std::weak_ptr<Node>				nodeA() const;
		std::weak_ptr<Node>				nodeB() const;
		const math::vec3& 				contactPoint() const;
		const math::vec3& 				contactNormal() const;
		float 							collisionImpulse() const;
		float 							penetrationDistance() const;
		float 							sweepTestFraction() const;

	private:
		/// Private Member Variables ///

		std::shared_ptr<Node> 			_nodeA;
		std::shared_ptr<Node>			_nodeB;
		math::vec3 						_contactPoint;
		math::vec3 						_contactNormal;
		float							_collisionImpulse;
		float							_penetrationDistance;
		float							_sweepTestFraction;
	};
}

#endif /* AVARA3D_PHYSICS_PHYSICSCONTACT_H */
