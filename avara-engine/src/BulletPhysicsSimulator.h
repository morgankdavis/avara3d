//
//  BulletPhysicsSimulator.h
//	avara-engine
//
//  Created by Morgan Davis on 5/1/18.
//  Copyright © 2018 Morgan K Davis. All rights reserved.
//

#ifndef BulletPhysicsSimulator_h
#define BulletPhysicsSimulator_h


#include <memory>

#include "PhysicsSimulator.h"


class btCollisionDispatcher;
class btDbvtBroadphase;
class btDefaultCollisionConfiguration;
class btDiscreteDynamicsWorld;
class btSequentialImpulseConstraintSolver;


namespace ae {
	
	
	class PhysicsDebugDrawer;
	
	
	class BulletPhysicsSimulator : public PhysicsSimulator {
		
	public:
		
		/***************************************************************************************
		     Lifecycle
		 ***************************************************************************************/
		
		BulletPhysicsSimulator();
		
		BulletPhysicsSimulator(const BulletPhysicsSimulator& other) = delete; // copy constructor
		BulletPhysicsSimulator& operator=(const BulletPhysicsSimulator& other) = delete; // copy assignment
		
		virtual ~BulletPhysicsSimulator();
		
		/**************************************************************************************
		     Public
		 **************************************************************************************/
		
		/**************************************************************************************
		     Internal
		 **************************************************************************************/
		
		/**************************************************************************************
		     Physics Simulator
		 **************************************************************************************/
		
		void initialize(const PhysicsWorld& world) override;
		void step(float time) override;
		
	private:
		
		/**************************************************************************************
		     Private
		 **************************************************************************************/
		
		std::shared_ptr<btDefaultCollisionConfiguration> 		m_btCollisionConfiguration;
		std::shared_ptr<btCollisionDispatcher>					m_btDispatcher;
		std::shared_ptr<btDbvtBroadphase>						m_btBroadphase;
		std::shared_ptr<btSequentialImpulseConstraintSolver>	m_btSolver;
		std::shared_ptr<btDiscreteDynamicsWorld>				m_btWorld;
		std::shared_ptr<PhysicsDebugDrawer>						m_debugDrawer;
	};
}


#endif /* BulletPhysicsSimulator_h */
