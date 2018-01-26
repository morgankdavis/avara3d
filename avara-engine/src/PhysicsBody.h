//
//  PhysicsBody.h
//	avara-engine
//
//  Created by Morgan Davis on 10/21/16.
//  Copyright © 2016 Morgan K Davis. All rights reserved.
//

#ifndef PhysicsBody_h
#define PhysicsBody_h


#include <memory>

#include <glm/glm.hpp>

#include "Types.h"


namespace ae {
	
	
	class PhysicsShape;
	
	
	class PhysicsBody {
		
	public:
		
		/***************************************************************************************
		     MARK:   Lifecycle
		 **************************************************************************************/
		
		PhysicsBody(PhysicsBodyType type);
		PhysicsBody(PhysicsBodyType type, std::shared_ptr<PhysicsShape> shape);
		
		/***************************************************************************************
		     MARK:   Public
		 **************************************************************************************/
		
		std::shared_ptr<PhysicsShape> shape() const;
		void shape(std::shared_ptr<PhysicsShape> shape);
		
		PhysicsBodyType type() const;
		void type(PhysicsBodyType type);
		
		glm::vec3 velocityFactor() const;
		void velocityFactor(glm::vec3 factor);
		
		glm::vec3 angularVelocityFactor() const;
		void angularVelocityFactor(glm::vec3 factor);
		
		bool affectedByGravity() const;
		void affectedByGravity(bool flag);
		
		float mass() const;
		void mass(float mass);
		
		float charge() const;
		void charge(float charge);
		
		float friction() const;
		void friction(float friction);
		
		float rollingFriction() const;
		void rollingFriction(float friction);
		
		float restitution() const;
		void restitution(float restitution);
		
		float damping() const;
		void damping(float damping);
		
		float angularDamping() const;
		void angularDamping(float damping);
		
		glm::vec3 momentOfInertia() const;
		void momentOfInertia(glm::vec3 moment);
		
		glm::vec3 velocity() const;
		void velocity(glm::vec3 velocity);
		
		glm::vec4 angularVelocity() const;
		void angularVelocity(glm::vec4 velocity);
		
		bool resting() const;
		void resting(bool flag);
		
		bool allowsResting() const;
		void allowsResting(bool flag);
		
		// categoryBitmask
		// contactTestBitmask
		// collisionBitmask
		
		void applyForce(glm::vec3 force, bool impulse);
		void applyForce(glm::vec3 force, glm::vec3 location, bool impulse);
		void applyTorque(glm::vec4 force, bool impulse);
		void clearForces();
		
		void resetTransform();
	};
}


#endif /* PhysicsBody_h */
