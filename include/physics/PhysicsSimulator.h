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
	class PhysicsBody;
	class PhysicsShape;
	class Scene;
	

	class PhysicsSimulator {

/*********************************************************************************************
	Types
 *********************************************************************************************/

	public:



/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

		PhysicsSimulator();
		PhysicsSimulator(const PhysicsSimulator& other) = delete; // copy constructor
		PhysicsSimulator& operator=(const PhysicsSimulator& other) = delete; // copy assignment
		virtual ~PhysicsSimulator();

/*********************************************************************************************
	Public
 *********************************************************************************************/

		//virtual PHYSICS_SIMULATION_ENGINE	simulationEngine() const = 0;

/*********************************************************************************************
	Internal
 *********************************************************************************************/

		virtual void 				beginUpdate(const Scene& scene);
		virtual void 				endUpdate(const Scene& scene);

		virtual void				update(Scene& scene);
		virtual void				sync(Scene& scene);

		virtual void				update(PhysicsBody& body,
										   Node& node);
		virtual void				sync(PhysicsBody& body,
										 Node& node,
										 glm::mat4& worldTransform);

		virtual void				update(PhysicsShape& shape,
										   PHYSICS_BODY_TYPE bodyType,
										   bool& updated);
		virtual void				sync(PhysicsShape& shape,
										 PHYSICS_BODY_TYPE bodyType);

		virtual void 				step(float time);
		
/*********************************************************************************************
	Protected
 *********************************************************************************************/

	protected:

		glm::vec3 					_gravity;
		float 						_speed;
		float						_timestep;

		//PHYSICS_SIMULATION_ENGINE	_simulationEngine;
	};
}


#endif /* PhysicsSimulator_h */
