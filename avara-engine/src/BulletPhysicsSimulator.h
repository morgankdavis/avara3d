//
//  BulletPhysicsSimulator.h
//	avara-engine
//
//  Created by Morgan Davis on 5/1/18.
//  Copyright © 2018 Morgan K Davis. All rights reserved.
//

#ifndef BulletPhysicsSimulator_h
#define BulletPhysicsSimulator_h


#include <map>
#include <memory>

#include "PhysicsSimulator.h"


struct btDbvtBroadphase;

class btCollisionDispatcher;
class btCollisionShape;
class btDefaultCollisionConfiguration;
class btDefaultMotionState;
class btDiscreteDynamicsWorld;
class btRigidBody;
class btSequentialImpulseConstraintSolver;


namespace ae {
	
	
	class PhysicsDebugDrawer;
	
	
	class BulletPhysicsSimulator : public PhysicsSimulator {
		
	public:
		
		/**************************************************************************************
		     Types
		 **************************************************************************************/
		
		typedef std::map<PHYSICS_BODY_ID,
			std::pair<std::shared_ptr<btRigidBody>, std::shared_ptr<btDefaultMotionState>>>
																PhysicsBodyMotionStateIDMapping;
		
		typedef std::map<PHYSICS_SHAPE_ID, std::shared_ptr<btCollisionShape>>
																PhysicsShapeIDMapping;
		
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
		
//		void initialize() override;
		void update(PhysicsWorld& physicsWorld,
					const DEBUG_OPTIONS& debugOptions) override;
		void update(PhysicsBody& physicsBody,
					const DEBUG_OPTIONS& debugOptions) override;
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
		
		PhysicsBodyMotionStateIDMapping							m_bodyMotionStateIDMapping;
		PHYSICS_BODY_ID											m_bodyIDCounter;
		
		PhysicsShapeIDMapping									m_shapeIDMapping;
		PHYSICS_SHAPE_ID										m_shapeIDCounter;
	};
}


#endif /* BulletPhysicsSimulator_h */
