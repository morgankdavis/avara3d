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

#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>
#include <glm/glm.hpp>

#include "Types.h"


namespace ae {
	
	
	class Node;
	class PhysicsShape;
	
	
	class PhysicsBody {
		
	public:
		
		/***************************************************************************************
		     MARK:   Static
		 **************************************************************************************/
		
		static std::shared_ptr<PhysicsBody> StaticBody();
		static std::shared_ptr<PhysicsBody> DynamicBody();
		static std::shared_ptr<PhysicsBody> KinematicBody();
		
		/***************************************************************************************
		     MARK:   Lifecycle
		 **************************************************************************************/
		
		PhysicsBody();
		PhysicsBody(PhysicsBodyType type);
		PhysicsBody(PhysicsBodyType type, std::shared_ptr<PhysicsShape> shape);
		
		/***************************************************************************************
		     MARK:   Public
		 **************************************************************************************/
		
		PhysicsBodyType type() const;
		void type(PhysicsBodyType type);
		
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

		bool resting() const;
		void resting(bool flag);
		
		float linearSleepingThreshold() const;
		void setLinearSleepingThreshold(float threshold);
		
		float angularSleepingThreshold() const;
		void setAngularSleepingThreshold(float threshold);
		
		bool allowsResting() const;
		void allowsResting(bool flag);
		
		// categoryBitmask
		// contactTestBitmask
		// collisionBitmask
		
		void applyForce(glm::vec3 force, bool impulse);
		void applyForce(glm::vec3 force, glm::vec3 location, bool impulse);
		void applyTorque(glm::vec3 torque, bool impulse);
		void clearForces();
		
		void resetTransform();
		
		/***************************************************************************************
		     MARK:   Internal
		 **************************************************************************************/
		
		void attachedToNode(Node& node);
		std::shared_ptr<btDefaultMotionState> btMotionState() const;
		
	private:
		
		/***************************************************************************************
		     MARK:   Private
		 **************************************************************************************/

		PhysicsBodyType 						m_type;
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
		bool 									m_resting;
		bool 									m_allowsResting;
		
		Node*									m_node;
		
		std::shared_ptr<btDefaultMotionState>	m_btMotionState;
		std::shared_ptr<btRigidBody>			m_btRigidBody;
	};
}


#endif /* PhysicsBody_h */
