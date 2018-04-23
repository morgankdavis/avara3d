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

#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>
#include <glm/glm.hpp>

#include "Types.h"


namespace ae {
	

	class HitTestResult;
	class PhysicsBody;
	class PhysicsContact;
	class PhysicsDebugDrawer;
	class PhysicsShape;
	class Scene;
	
	
	class PhysicsWorld {
		
		using PhysicsWorldDidBeginContactFunction = std::function<void(PhysicsWorld& world,
																	   PhysicsContact& contact)>;
		using PhysicsWorldDidUpdateContactFunction = std::function<void(PhysicsWorld& world,
																		PhysicsContact& contact)>;
		using PhysicsWorldDidEndContactFunction = std::function<void(PhysicsWorld& world,
																	 PhysicsContact& contact)>;
																		 
	public:
		
		// callbacks
		
		/***************************************************************************************
		     Lifecycle
		 ***************************************************************************************/
		
		PhysicsWorld();
		
		/***************************************************************************************
		     Public
		 ***************************************************************************************/
		
		glm::vec3 gravity() const;
		void gravity(glm::vec3 gravity);
		
		/* REMOVE? */ float speed() const;
		/* REMOVE? */ void speed(float speed);
		
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
		
		/***************************************************************************************
		     Internal
		 ***************************************************************************************/
		
		void attachedToScene(std::shared_ptr<Scene> scene);
		
		void debugOptions(DEBUG_OPTIONS options);
		
		void step();
		
		std::shared_ptr<btDiscreteDynamicsWorld> btWorld() const;
		std::shared_ptr<PhysicsDebugDrawer> debugDrawer() const;
		
	private:
		
		/***************************************************************************************
		     Private
		 ***************************************************************************************/
		
		std::shared_ptr<btDefaultCollisionConfiguration> 		m_btCollisionConfiguration;
		std::shared_ptr<btCollisionDispatcher>					m_btDispatcher;
		std::shared_ptr<btDbvtBroadphase>						m_btBroadphase;
		std::shared_ptr<btSequentialImpulseConstraintSolver>	m_btSolver;
		std::shared_ptr<btDiscreteDynamicsWorld>				m_btWorld;
		std::shared_ptr<PhysicsDebugDrawer>						m_debugDrawer;
		
		glm::vec3 												m_gravity;
		float 													m_speed;
		float 													m_timestep;
		
		std::weak_ptr<Scene>									m_scene;
	};
}


#endif /* PhysicsWorld_h */
