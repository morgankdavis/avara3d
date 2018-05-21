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
#include <set>

#include "PhysicsSimulator.h"


struct btDefaultMotionState;
struct btDbvtBroadphase;

class btCollisionDispatcher;
class btCollisionShape;
class btDefaultCollisionConfiguration;
class btDiscreteDynamicsWorld;
class btRigidBody;
class btSequentialImpulseConstraintSolver;


namespace ae {
	
	
	class Node;
	class PhysicsBody;
	class PhysicsDebugDrawer;
	class PhysicsShape;
	class Scene;
	
	
	class BulletPhysicsSimulator : public PhysicsSimulator {
		
	public:
		
		/**************************************************************************************
		     Types
		 **************************************************************************************/
		
		/* <ae_obj : <bt_rigidBody, bt_motionState>> */
		using PhysicsBodyBTMapping =
			std::map<std::shared_ptr<PhysicsBody>, std::pair<std::shared_ptr<btRigidBody>, 
												   			 std::shared_ptr<btDefaultMotionState>>>;
		
		/* <ae_obj : <bt_collisionShape, bt_childShapes>> */
		using PhysicsShapeBTMapping =
			std::map<std::shared_ptr<PhysicsShape>, std::pair<std::shared_ptr<btCollisionShape>,
															  std::vector<std::shared_ptr<btCollisionShape>>>>;
		
		/***************************************************************************************
		     Lifecycle
		 ***************************************************************************************/
		
		BulletPhysicsSimulator();
		
		BulletPhysicsSimulator(const BulletPhysicsSimulator& other) = delete; // copy constructor
		BulletPhysicsSimulator& operator=(const BulletPhysicsSimulator& other) = delete; // copy assignment
		
		virtual ~BulletPhysicsSimulator();
		
		/**************************************************************************************
		     Physics Simulator
		 **************************************************************************************/
		
		void beginUpdate(PASS pass,
						 const Scene& scene) override;
		void endUpdate(PASS pass,
					   const Scene& scene) override;

		void update(PASS pass,
					std::shared_ptr<Scene> scene,
					const DEBUG_OPTIONS& debugOptions) override;
		void update(PASS pass,
					std::shared_ptr<Node> node,
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
		
		PhysicsBodyBTMapping									m_bodyBTMapping;
		PhysicsShapeBTMapping									m_shapeBTMapping;
		
		std::set<std::shared_ptr<PhysicsBody>>					m_activeBodies;
		std::set<std::shared_ptr<PhysicsShape>>					m_activeShapes;
	};
}


#endif /* BulletPhysicsSimulator_h */
