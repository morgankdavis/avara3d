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
	
	
	class Geometry;
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
		~PhysicsBody();
		
		/***************************************************************************************
		     Public
		 ***************************************************************************************/
		
		PHYSICS_BODY_TYPE type() const;
		void type(PHYSICS_BODY_TYPE type);
		
		std::shared_ptr<PhysicsShape> shape() const;
		void shape(std::shared_ptr<PhysicsShape> shape);

		float mass() const;
		void mass(float mass);
		
		glm::vec3 localInertia() const;
		void localInertia(glm::vec3 moment);

		float friction() const;
		void friction(float friction);
		
		float rollingFriction() const;
		void rollingFriction(float friction);
		
		float restitution() const;
		void restitution(float restitution);
		
		glm::vec3 linearVelocity() const;
		void linearVelocity(glm::vec3 velocity, bool setDirty=true);
		
		glm::vec3 angularVelocity() const;
		void angularVelocity(glm::vec3 velocity, bool setDirty=true);
		
		glm::vec3 linearFactor() const;
		void linearFactor(glm::vec3 factor);
		
		glm::vec3 angularFactor() const;
		void angularFactor(glm::vec3 factor);
		
		float linearDamping() const;
		void linearDamping(float damping);
		
		float angularDamping() const;
		void angularDamping(float damping);

		float linearSleepingThreshold() const;
		void linearSleepingThreshold(float threshold);
		
		float angularSleepingThreshold() const;
		void angularSleepingThreshold(float threshold);
		
		bool allowsResting() const;
		void allowsResting(bool flag);
		
		bool resting() const;
		
		// categoryBitmask
		// contactTestBitmask
		// collisionBitmask
		
		void applyForce(glm::vec3 force, bool impulse);
		void applyForce(glm::vec3 force, glm::vec3 location, bool impulse);
		void applyTorque(glm::vec3 torque, bool impulse);
		void clearForces();
		
		void resetTransform();
		
		/***************************************************************************************
		     Internal
		 ***************************************************************************************/
		
		void resting(bool resting);
		
		void attachedToNode(std::shared_ptr<Node> node);
		void geometryAttachedToNode(std::shared_ptr<Geometry> geometry);
		
		std::weak_ptr<Node> node() const;
		
		PHYSICS_BODY_DIRTY_BITS dirtyBits() const;
		void dirtyBits(PHYSICS_BODY_DIRTY_BITS bits);
		
	private:
		
		/***************************************************************************************
		     Private
		 ***************************************************************************************/

		void checkShape();
		
		PHYSICS_BODY_TYPE 						m_type;
		std::shared_ptr<PhysicsShape> 			m_shape;
		float 									m_mass;
		glm::vec3 								m_localInertia;
		float 									m_friction;
		float 									m_rollingFriction;
		float 									m_restitution;
		glm::vec3 								m_linearVelocity;
		glm::vec3 								m_angularVelocity;
		glm::vec3 								m_linearFactor;
		glm::vec3 								m_angularFactor;
		float 									m_linearDamping;
		float 									m_angularDamping;
		float									m_linearSleepingThreshold;
		float									m_angularSleepingThreshold;
		bool 									m_allowsResting;
		bool 									m_resting;
		
		std::weak_ptr<Node>						m_node;

		PHYSICS_BODY_DIRTY_BITS 				m_dirtyBits;
	};
}


#endif /* PhysicsBody_h */
