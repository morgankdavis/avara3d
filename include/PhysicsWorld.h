//
//  PhysicsWorld.h
//	avara-engine
//
//  Created by Morgan Davis on 1/26/18.
//  Copyright © 2018 Morgan K Davis. All rights reserved.
//

#ifndef PhysicsWorld_h
#define PhysicsWorld_h


#include <functional>
#include <memory>

#include <glm/glm.hpp>

#include "Aliases.h"
#include "Types.h"


namespace ae {
	

	class HitTestResult;
	class PhysicsBody;
	class PhysicsContact;
	class PhysicsDebugDrawer;
	class PhysicsShape;
	class Scene;
	
	
	class PhysicsWorld : public std::enable_shared_from_this<PhysicsWorld> {
		
/*********************************************************************************************
	Types
 *********************************************************************************************/

	public:

		using DidBeginContactFunction = 	std::function<void(PhysicsWorld& world, PhysicsContact& contact)>;
		using DidUpdateContactFunction = 	std::function<void(PhysicsWorld& world, PhysicsContact& contact)>;
		using DidEndContactFunction = 		std::function<void(PhysicsWorld& world, PhysicsContact& contact)>;
		
/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/
		
		PhysicsWorld();
		~PhysicsWorld();
		
/*********************************************************************************************
	Public
 *********************************************************************************************/

		glm::vec3 							gravity() const;
		void 								gravity(glm::vec3 gravity);

		float 								timestep() const;
		void 								timestep(float timestep);

		void 								updateCollisionPairs();

		PhysicsContactSPtr 					contactTest(PhysicsBodySPtr bodyA,
														  PhysicsBodySPtr bodyB); // may add options
		PhysicsContactSPtr 					contactTest(PhysicsBodySPtr body); // may add options

		HitTestResultSPtr 					rayTest(glm::vec3 fromVec, glm::vec3 toVec); // may add options
		PhysicsContactSPtr 					convexSweepTest(PhysicsContactSPtr contact,
															  const glm::mat4& fromMat,
															  const glm::mat4& toMat); // may add options

/*********************************************************************************************
	Internal
 *********************************************************************************************/
		
		void 								attachedToScene(SceneSPtr scene);
		//void 								debugOptions(DEBUG_OPTIONS options);

		PHYSICS_WORLD_DIRTY_BITS 			dirtyBits() const;
		void 								dirtyBits(PHYSICS_WORLD_DIRTY_BITS bits);
		
/*********************************************************************************************
	Private
 *********************************************************************************************/

	private:

		glm::vec3 							_gravity;
		float 								_timestep;
		SceneWPtr 							_scene;
		PHYSICS_WORLD_DIRTY_BITS			_dirtyBits;
	};
}


#endif /* PhysicsWorld_h */
