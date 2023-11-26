//
//  PhysicalWorld.h
//	avara-engine
//
//  Created by Morgan Davis on 1/26/18.
//  Copyright © 2018 Morgan K Davis. All rights reserved.
//

#ifndef PhysicsWorld_h
#define PhysicsWorld_h


#include <functional>
#include <memory>

#include "glm/glm.hpp"

#include "Types.h"


namespace ae {
	

	class HitTestResult;
	class PhysicsBody;
	class PhysicsContact;
	class PhysicsShape;
	class PhysicsSimulator;
	class Scene;
	
	
	class PhysicalWorld {
		
/*********************************************************************************************
	Types
 *********************************************************************************************/

	public:

		using DidSimulateCallback = 	std::function<void(PhysicalWorld& world, float time)>;
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

		float 								timestep() const;
		void 								timestep(float timestep);

		void 								updateCollisionPairs();

		std::shared_ptr<PhysicsContact> 	contactTest(std::shared_ptr<PhysicsBody> bodyA,
													   std::shared_ptr<PhysicsBody> bodyB); // may add options
		std::shared_ptr<PhysicsContact> 	contactTest(std::shared_ptr<PhysicsBody> body); // may add options

		std::shared_ptr<HitTestResult> 		rayTest(glm::vec3 fromVec, glm::vec3 toVec); // may add options
		std::shared_ptr<PhysicsContact> 	convexSweepTest(std::shared_ptr<PhysicsContact> contact,
														   const glm::mat4& fromMat,
														   const glm::mat4& toMat); // may add options

		DidSimulateCallback						didSimulate() const;
		void									didSimulate(DidSimulateCallback function);

		PhysicalWorld::BeginContactCallback 	beginContact() const;
		void 									beginContact(PhysicalWorld::BeginContactCallback function);

		PhysicalWorld::ContinueContactCallback	continueContact() const;
		void 									continueContact(PhysicalWorld::ContinueContactCallback function);

		PhysicalWorld::EndContactCallback 		endContact() const;
		void 									endContact(PhysicalWorld::EndContactCallback function);

		std::weak_ptr<Scene>					scene() const;
		void									scene(std::weak_ptr<Scene> scene);

/*********************************************************************************************
	Internal
 *********************************************************************************************/
		
//		void 								attachedToScene(std::shared_ptr<Scene> scene);
		//void 								debugOptions(DEBUG_OPTIONS options);

		PHYSICS_WORLD_DIRTY_MASK 			dirtyMask() const;
		void 								dirtyMask(PHYSICS_WORLD_DIRTY_MASK mask);

		std::shared_ptr<PhysicsSimulator>	simulator() const;
		void								simulator(std::shared_ptr<PhysicsSimulator> simulator);

/*********************************************************************************************
	Private
 *********************************************************************************************/

	private:

		glm::vec3 							_gravity;
		float 								_timestep;
//		std::weak_ptr<Scene> 				_scene;
		PHYSICS_WORLD_DIRTY_MASK			_dirtyMask;

		DidSimulateCallback					_didSimulate;
		BeginContactCallback				_beginContact;
		ContinueContactCallback				_continueContact;
		EndContactCallback					_endContact;

		std::shared_ptr<PhysicsSimulator>	_simulator;

		std::weak_ptr<Scene>				_scene;
	};
}


#endif /* PhysicsWorld_h */
