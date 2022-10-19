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

#include "Types.h"


namespace ae {
	

	class HitTestResult;
	class PhysicsBody;
	class PhysicsContact;
	class PhysicsDebugDrawer;
	class PhysicsShape;
	class Scene;
	
	
	class PhysicsWorld : public std::enable_shared_from_this<PhysicsWorld> {
	
																		 
	public:
		
/*********************************************************************************************
	Types
 *********************************************************************************************/
		
		using DidBeginContactFunction = std::function<void(PhysicsWorld& world, PhysicsContact& contact)>;
		using DidUpdateContactFunction = std::function<void(PhysicsWorld& world, PhysicsContact& contact)>;
		using DidEndContactFunction = std::function<void(PhysicsWorld& world, PhysicsContact& contact)>;
		
/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/
		
		PhysicsWorld();
		~PhysicsWorld();
		
/*********************************************************************************************
	Public
 *********************************************************************************************/
		
		glm::vec3 gravity() const;
		void gravity(glm::vec3 gravity);

		float timestep() const;
		void timestep(float timestep);
		
		void updateCollisionPairs();
		
		std::shared_ptr<PhysicsContact> contactTest(std::shared_ptr<PhysicsBody> bodyA,
													std::shared_ptr<PhysicsBody> bodyB); // may add options
		std::shared_ptr<PhysicsContact> contactTest(std::shared_ptr<PhysicsBody> body); // may add options
		
		std::shared_ptr<HitTestResult> rayTest(glm::vec3 fromVec, glm::vec3 toVec); // may add options
		std::shared_ptr<PhysicsContact> convexSweepTest(std::shared_ptr<PhysicsContact> contact,
														const glm::mat4& fromMat,
														const glm::mat4& toMat); // may add options
		
/*********************************************************************************************
	Internal
 *********************************************************************************************/
		
		void attachedToScene(std::shared_ptr<Scene> scene);
		//void debugOptions(DEBUG_OPTIONS options);

		PHYSICS_WORLD_DIRTY_BITS dirtyBits() const;
		void dirtyBits(PHYSICS_WORLD_DIRTY_BITS bits);

	private:
		
/*********************************************************************************************
	Private
 *********************************************************************************************/

		glm::vec3 												m_gravity;
		float 													m_timestep;
		std::weak_ptr<Scene>									m_scene;
		PHYSICS_WORLD_DIRTY_BITS								m_dirtyBits;
	};
}


#endif /* PhysicsWorld_h */
