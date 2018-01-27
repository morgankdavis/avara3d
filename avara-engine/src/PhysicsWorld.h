//
//  PhysicsWorld.h
//	avara-engine
//
//  Created by Morgan Davis on 10/21/16.
//  Copyright © 2016 Morgan K Davis. All rights reserved.
//

#ifndef PhysicsWorld_h
#define PhysicsWorld_h


#include <memory>

#include "btBulletDynamicsCommon.h"
#include <glm/glm.hpp>


namespace ae {
	
	
//	class btDefaultCollisionConfiguration;
//	class btCollisionDispatcher;
//	class btDbvtBroadphase;
//	class btSequentialImpulseConstraintSolver;
//	class btDiscreteDynamicsWorld;
	
	class HitTestResult;
	class PhysicsBody;
	class PhysicsContact;
	class PhysicsShape;
	
	
	class PhysicsWorld {
		
	public:
		
		// callbacks
		
		/***************************************************************************************
		     MARK:   Lifecycle
		 **************************************************************************************/
		
		PhysicsWorld();
		
		/***************************************************************************************
		     MARK:   Public
		 **************************************************************************************/
		
		glm::vec3 gravity() const;
		void gravity(glm::vec3 gravity);
		
		float speed() const;
		void speed(float speed);
		
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
		
	private:
		
		/***************************************************************************************
		     MARK:   Private
		 **************************************************************************************/
		
		std::shared_ptr<btDefaultCollisionConfiguration> 		m_btCollisionConfiguration;
		std::shared_ptr<btCollisionDispatcher>					m_btDispatcher;
		std::shared_ptr<btDbvtBroadphase>						m_btBroadphase;
		std::shared_ptr<btSequentialImpulseConstraintSolver>	m_btSolver;
		std::shared_ptr<btDiscreteDynamicsWorld>				m_btWorld;
	};
	
}


#endif /* PhysicsWorld_h */
