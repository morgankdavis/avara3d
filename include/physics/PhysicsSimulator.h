//
//  PhysicsSimulator.h
//	avara-engine
//
//  Created by Morgan Davis on 5/1/18.
//  Copyright © 2018 Morgan K Davis. All rights reserved.
//

#ifndef PhysicsSimulator_h
#define PhysicsSimulator_h


#include <memory>

#include "glm/glm.hpp"

#include "Types.h"


namespace ae {
	
	
	class Node;
	class PhysicalWorld;
	class PhysicsBody;
	class PhysicsShape;
	class Scene;
	

	class PhysicsSimulator {

/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

	public:

		PhysicsSimulator();
		PhysicsSimulator(const PhysicsSimulator& other) = delete; // copy constructor
		PhysicsSimulator& operator=(const PhysicsSimulator& other) = delete; // copy assignment
		virtual ~PhysicsSimulator();

/*********************************************************************************************
	Internal
 *********************************************************************************************/

//		virtual void			setTimestep(PhysicalWorld& world, float timestep);
//		virtual void			setSpeed(PhysicalWorld& world, float speed);
		virtual void			setGravity(PhysicalWorld& world, glm::vec3& gravity);

		virtual void			create(PhysicsBody& body);
		virtual void			remove(PhysicsBody& body);

		virtual void			setType(PhysicsBody& body, PHYSICS_BODY_TYPE type);
		virtual void			setShape(PhysicsBody& body, PhysicsShape& shape);
//		virtual void			setWorldTransform(PhysicsBody& body, const glm::mat4& transform);
		virtual void			setMass(PhysicsBody& body, float mass);
		virtual void			setMomentOfInertia(PhysicsBody& body, const glm::vec3& moment);
		virtual void			setFriction(PhysicsBody& body, float friction);
		virtual void			setRollingFriction(PhysicsBody& body, float friction);
		virtual void			setRestitution(PhysicsBody& body, float restitution);
		virtual void			setLinearVelocity(PhysicsBody& body, const glm::vec3& velocity);
		virtual void			setAngularVelocity(PhysicsBody& body, const glm::vec3& velocity);
		virtual void			setLinearFactor(PhysicsBody& body, const glm::vec3& factor);
		virtual void			setAngularFactor(PhysicsBody& body, const glm::vec3& factor);
		virtual void			setLinearDamping(PhysicsBody& body, float damping);
		virtual void			setAngularDamping(PhysicsBody& body, float damping);
		virtual void			setLinearSleepingThreshold(PhysicsBody& body, float threshold);
		virtual void			setAngularSleepingThreshold(PhysicsBody& body, float threshold);
		virtual void			setAffectedByGravity(PhysicsBody& body, bool flag);
		virtual void			setAllowsResting(PhysicsBody& body, bool flag);

		virtual void			create(PhysicsShape& shape);

		virtual void			update(PhysicalWorld& world, Stats& status) = 0;
		virtual void			step(PhysicalWorld& world, float deltaT);
		virtual void			sync(PhysicalWorld& world) = 0;

//		virtual void			sync(PhysicsBody& body,
//									 glm::mat4& worldTransform);

/*********************************************************************************************
	Protected
 *********************************************************************************************/

	protected:

//		glm::vec3 					_gravity;
//		float 						_speed;
//		float						_timestep;
	};
}


#endif /* PhysicsSimulator_h */
