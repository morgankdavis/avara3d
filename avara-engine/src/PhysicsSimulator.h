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

#include <glm/glm.hpp>

#include "Types.h"


namespace ae {
	
	
	class PhysicsBody;
	class PhysicsWorld;
	

	class PhysicsSimulator : public std::enable_shared_from_this<PhysicsSimulator> {
		
	public:
		
		/***************************************************************************************
		     Lifecycle
		 ***************************************************************************************/
		
		PhysicsSimulator();
		
		PhysicsSimulator(const PhysicsSimulator& other) = delete; // copy constructor
		PhysicsSimulator& operator=(const PhysicsSimulator& other) = delete; // copy assignment
		
		virtual ~PhysicsSimulator();
		
		/**************************************************************************************
		     Internal
		 **************************************************************************************/
		
		//virtual void initialize();
		virtual void update(PhysicsWorld& physicsWorld,
							const DEBUG_OPTIONS& debugOptions);
		virtual void update(PhysicsBody& physicsBody,
							const DEBUG_OPTIONS& debugOptions);
		virtual void step(float time);
		
	protected:
		
		/**************************************************************************************
		     Protected
		 **************************************************************************************/
		
		glm::vec3 												m_gravity;
		float 													m_speed;
		float 													m_timestep;
	};
}


#endif /* PhysicsSimulator_h */
