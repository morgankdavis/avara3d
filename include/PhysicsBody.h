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
		
/*********************************************************************************************
	Public Static
 *********************************************************************************************/

	public:

		static std::shared_ptr<ae::PhysicsBody> 	StaticBody();
		static std::shared_ptr<ae::PhysicsBody> 	DynamicBody();
		static std::shared_ptr<ae::PhysicsBody> 	KinematicBody();
		
/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/
		
		PhysicsBody();
		PhysicsBody(PHYSICS_BODY_TYPE type);
//		PhysicsBody(PHYSICS_BODY_TYPE type, std::shared_ptr<ae::PhysicsShape> shape);
		~PhysicsBody();
		
/*********************************************************************************************
	Public
 *********************************************************************************************/
		
		PHYSICS_BODY_TYPE 					type() const;
		void 								type(PHYSICS_BODY_TYPE type);

		std::shared_ptr<ae::PhysicsShape> 	shape() const;
		void 								shape(std::shared_ptr<ae::PhysicsShape> shape);

		float 								mass() const;
		void 								mass(float mass);
		
		glm::vec3 							momentOfInertia() const;
		void 								momentOfInertia(glm::vec3 moment);

		float 								friction() const;
		void 								friction(float friction);
		
		float 								rollingFriction() const;
		void 								rollingFriction(float friction);
		
		float 								restitution() const;
		void 								restitution(float restitution);
		
		glm::vec3 							linearVelocity() const;
		void 								linearVelocity(glm::vec3 velocity, bool setDirty=true);
		
		glm::vec3 							angularVelocity() const;
		void 								angularVelocity(glm::vec3 velocity, bool setDirty=true);
		
		glm::vec3 							linearFactor() const;
		void 								linearFactor(glm::vec3 factor);
		
		glm::vec3 							angularFactor() const;
		void 								angularFactor(glm::vec3 factor);
		
		float								linearDamping() const;
		void 								linearDamping(float damping);
		
		float								angularDamping() const;
		void 								angularDamping(float damping);

		float								linearSleepingThreshold() const;
		void 								linearSleepingThreshold(float threshold);
		
		float								angularSleepingThreshold() const;
		void 								angularSleepingThreshold(float threshold);
		
		bool 								affectedByGravity() const;
		void 								affectedByGravity(bool flag);
		
		bool 								allowsResting() const;
		void 								allowsResting(bool flag);
		
		bool 								resting() const;
		
		// categoryBitmask
		// contactTestBitmask
		// collisionBitmask
		
		void 								applyForce(glm::vec3 force, bool impulse);
		void 								applyForce(glm::vec3 force, glm::vec3 location, bool impulse);
		void 								applyTorque(glm::vec3 torque, bool impulse);
		void 								clearForces();
		
		void 								resetTransform();
		
/*********************************************************************************************
	Internal
 *********************************************************************************************/
		
		void 							resting(bool resting);
		
		void 							attachedToNode(std::shared_ptr<ae::Node> node);
		void 							geometryAttachedToNode(std::shared_ptr<ae::Geometry> geometry);
		
		std::weak_ptr<ae::Node> 						node() const;
		
		PHYSICS_BODY_DIRTY_BITS 		dirtyBits() const;
		void 							dirtyBits(PHYSICS_BODY_DIRTY_BITS bits);
		
/*********************************************************************************************
	Private
 *********************************************************************************************/

	private:

		void checkShape();
		
		PHYSICS_BODY_TYPE 					_type;
		std::shared_ptr<ae::PhysicsShape>	_shape;
		float 								_mass;
		glm::vec3 							_momentOfInertia;
		float 								_friction;
		float 								_rollingFriction;
		float 								_restitution;
		glm::vec3 							_linearVelocity;
		glm::vec3 							_angularVelocity;
		glm::vec3 							_linearFactor;
		glm::vec3 							_angularFactor;
		float 								_linearDamping;
		float 								_angularDamping;
		float								_linearSleepingThreshold;
		float								_angularSleepingThreshold;
		bool								_affectedByGravity;
		bool 								_allowsResting;
		bool 								_resting;
		
		std::weak_ptr<ae::Node>				_node;

		PHYSICS_BODY_DIRTY_BITS 			_dirtyBits;
	};
}


#endif /* PhysicsBody_h */
