//
//  PhysicalWorld.h
//	avara-engine
//
//  Created by Morgan Davis on 1/26/18.
//  Copyright © 2018 Morgan K Davis. All rights reserved.
//

#ifndef PhysicalWorld_h
#define PhysicalWorld_h


#include <functional>
#include <memory>

#include "glm/glm.hpp"

#include "Types.h"


namespace ae {
	

	class HitTestResult;
	class PhysicsBody;
	class PhysicsContact;
	class PhysicsShape;
	class PhysicalWorldResources;
	class PhysicsSimulator;
	class Scene;
	
	
	class PhysicalWorld {
		
/*********************************************************************************************
	Types
 *********************************************************************************************/

	public:

		using DidSimulateCallback = 	std::function<void(PhysicalWorld& world, double time)>;
		using BeginContactCallback = 	std::function<void(PhysicalWorld& world, PhysicsContact& contact)>;
		using ContinueContactCallback =	std::function<void(PhysicalWorld& world, PhysicsContact& contact)>;
		using EndContactCallback = 		std::function<void(PhysicalWorld& world, PhysicsContact& contact)>;
		
/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/
		
		PhysicalWorld(PHYSICS_SIMULATION_ENGINE engine);
		PhysicalWorld(const PhysicalWorld& other) = delete; // copy constructor
		PhysicalWorld& operator=(const PhysicalWorld& other) = delete; // copy assignment
		~PhysicalWorld();
		
/*********************************************************************************************
	Public
 *********************************************************************************************/

		glm::vec3 							gravity() const;
		void 								gravity(glm::vec3 gravity);

		float								speed() const;
		void 								speed(float speed);

		float 								timestep() const;
		void 								timestep(float timestep);

		std::shared_ptr<PhysicsContact> 	contactTest(std::shared_ptr<PhysicsBody> bodyA,
													   std::shared_ptr<PhysicsBody> bodyB); // may add options
		std::shared_ptr<PhysicsContact> 	contactTest(std::shared_ptr<PhysicsBody> body); // may add options

		std::shared_ptr<HitTestResult> 		rayTest(glm::vec3 fromVec, glm::vec3 toVec); // may add options
		std::shared_ptr<PhysicsContact> 	convexSweepTest(std::shared_ptr<PhysicsContact> contact,
														   const glm::mat4& fromMat,
														   const glm::mat4& toMat); // may add options

		void 								updateCollisionPairs();

		Scene*								scene() const;

		DidSimulateCallback					didSimulate() const;
		void								didSimulate(DidSimulateCallback function);

		BeginContactCallback 				beginContact() const;
		void 								beginContact(PhysicalWorld::BeginContactCallback function);

		ContinueContactCallback				continueContact() const;
		void 								continueContact(PhysicalWorld::ContinueContactCallback function);

		EndContactCallback 					endContact() const;
		void 								endContact(PhysicalWorld::EndContactCallback function);

/*********************************************************************************************
	Internal
 *********************************************************************************************/

		void								attachedToScene(Scene* scene);

		void								simulate(const Scene& scene,
													 double runT,
													 double deltaRunT,
													 Stats& stats);

		PhysicalWorldResources*				resources() const;

		PhysicsSimulator*					simulator() const;

		PHYSICS_WORLD_DIRTY_MASK 			dirtyMask() const;
		void 								dirtyMask(PHYSICS_WORLD_DIRTY_MASK mask);

/*********************************************************************************************
	Private
 *********************************************************************************************/

	private:

		glm::vec3 								_gravity;
		float 									_speed;
		float 									_timestep;
		std::unique_ptr<PhysicalWorldResources>	_resources;
		std::unique_ptr<PhysicsSimulator>		_simulator;
		Scene*									_scene;
		PHYSICS_WORLD_DIRTY_MASK				_dirtyMask;
		DidSimulateCallback						_didSimulate;
		BeginContactCallback					_beginContact;
		ContinueContactCallback					_continueContact;
		EndContactCallback						_endContact;
	};
}


#endif /* PhysicalWorld_h */
