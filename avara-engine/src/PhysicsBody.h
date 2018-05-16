//
//  PhysicsBody.h
//	avara-engine
//
//  Created by Morgan Davis on 1/26/18.
//  Copyright © 2018 Morgan K Davis. All rights reserved.
//

#ifndef PhysicsBody_h
#define PhysicsBody_h


#include <memory>

#include <glm/glm.hpp>

#include "Types.h"


namespace ae {
	
	
	class Node;
	class PhysicsShape;
	
	
	class PhysicsBody : public std::enable_shared_from_this<PhysicsBody> {
		
	public:
		
		/***************************************************************************************
		     Public Static
		 ***************************************************************************************/
		
		static std::shared_ptr<PhysicsBody> StaticBody();
		static std::shared_ptr<PhysicsBody> DynamicBody();
		static std::shared_ptr<PhysicsBody> KinematicBody();
		
		/***************************************************************************************
		     Lifecycle
		 ***************************************************************************************/
		
		PhysicsBody();
		PhysicsBody(PHYSICS_BODY_TYPE type);
		PhysicsBody(PHYSICS_BODY_TYPE type, std::shared_ptr<PhysicsShape> shape);
		
		/***************************************************************************************
		     Public
		 ***************************************************************************************/
		
		PHYSICS_BODY_TYPE type() const;
		void type(PHYSICS_BODY_TYPE type);
		
		std::shared_ptr<PhysicsShape> shape() const;
		void shape(std::shared_ptr<PhysicsShape> shape);
		
		glm::vec3 velocityFactor() const;
		void velocityFactor(glm::vec3 factor);
		
		glm::vec3 angularVelocityFactor() const;
		void angularVelocityFactor(glm::vec3 factor);
		
		bool affectedByGravity() const;
		void affectedByGravity(bool flag);
		
		float mass() const;
		void mass(float mass);
		
		/* REMOVE? */ float charge() const;
		/* REMOVE? */ void charge(float charge);
		
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
		
		glm::vec3 angularVelocity() const;
		void angularVelocity(glm::vec3 velocity);

		float linearSleepingThreshold() const;
		void linearSleepingThreshold(float threshold);
		
		float angularSleepingThreshold() const;
		void angularSleepingThreshold(float threshold);
		
		bool allowsResting() const;
		void allowsResting(bool flag);
		
		bool resting() const;
		void resting(bool flag);
		
		// categoryBitmask
		// contactTestBitmask
		// collisionBitmask
		
		void applyForce(glm::vec3 force, bool impulse);
		void applyForce(glm::vec3 force, glm::vec3 location, bool impulse);
		void applyTorque(glm::vec3 torque, bool impulse);
		void clearForces();
		
		/* REMOVE? */ void resetTransform();
		
		/***************************************************************************************
		     Internal
		 ***************************************************************************************/
		
		void attachedToNode(std::shared_ptr<Node> node);
		
		std::weak_ptr<Node> node() const;
		
		PHYSICS_BODY_ID simulationID() const;
		void simulationID(PHYSICS_BODY_ID simID);
		
		PHYSICS_BODY_DIRTY_BITS dirtyBits() const;
		void dirtyBits(PHYSICS_BODY_DIRTY_BITS bits);
		
	private:
		
		/***************************************************************************************
		     Private
		 ***************************************************************************************/

		PHYSICS_BODY_TYPE 						m_type;
		std::shared_ptr<PhysicsShape> 			m_shape;
		glm::vec3 								m_velocityFactor;
		glm::vec3 								m_angularVelocityFactor;
		bool 									m_affectedByGravity;
		float 									m_mass;
		float 									m_charge;
		float 									m_friction;
		float 									m_rollingFriction;
		float 									m_restitution;
		float 									m_damping;
		float 									m_angularDamping;
		glm::vec3 								m_momentOfInertia;
		glm::vec3 								m_velocity;
		glm::vec3 								m_angularVelocity;
		float									m_linearSleepingThreshold;
		float									m_angularSleepingThreshold;
		bool 									m_allowsResting;
		bool 									m_resting;
		
		std::weak_ptr<Node>						m_node;
		
		PHYSICS_BODY_ID 						m_simulationID;
		PHYSICS_BODY_DIRTY_BITS 				m_dirtyBits;
	};
}


#endif /* PhysicsBody_h */
