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
#include <optional>
#include <unordered_set>
#include <utility>
#include <vector>

#include "PhysicsSimulator.h"


struct btDefaultMotionState;
struct btDbvtBroadphase;

class btBvhTriangleMeshShape;
class btCollisionDispatcher;
class btCollisionShape;
class btDefaultCollisionConfiguration;
class btDiscreteDynamicsWorld;
class btRigidBody;
class btSequentialImpulseConstraintSolver;
class btTriangleIndexVertexArray;
class btTriangleMesh;


namespace ae {
	

	class BulletBodyResources;
#ifdef DESKTOP
	class BulletDebugDrawer;
#endif
	class BulletShapeResources;
	class Node;
	class PhysicsBody;
	class PhysicsShape;
	class Renderer;
	class Scene;
	
	
	class BulletPhysicsSimulator : public PhysicsSimulator {
		
/*********************************************************************************************
	Types
 *********************************************************************************************/

	public:

		using PhysicsBodyBTMapping =
				std::map<std::shared_ptr<PhysicsBody>, std::shared_ptr<BulletBodyResources>>;

		using PhysicsShapeBTMapping =
				std::map<std::shared_ptr<PhysicsShape>, std::shared_ptr<BulletShapeResources>>;

/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/
		
		BulletPhysicsSimulator();
		
		BulletPhysicsSimulator(const BulletPhysicsSimulator& other) = delete; // copy constructor
		BulletPhysicsSimulator& operator=(const BulletPhysicsSimulator& other) = delete; // copy assignment
		
		virtual ~BulletPhysicsSimulator();
		
/*********************************************************************************************
	Internal
 *********************************************************************************************/

		void 		drawDebug(Renderer& renderer,
							  const glm::mat4& viewMat,
							  const glm::mat4& projectionMat,
							  const DEBUG_OPTIONS& debugOptions);

/*********************************************************************************************
	PhysicsSimulator
 *********************************************************************************************/

		void 		beginUpdate(PASS pass,
								const Scene& scene) override;
		void 		endUpdate(PASS pass,
							  const Scene& scene) override;

		void 		update(PASS pass,
						   Scene& scene,
						   const DEBUG_OPTIONS& debugOptions) override;
		void 		update(PASS pass,
						   std::shared_ptr<Node> node,
						   const DEBUG_OPTIONS& debugOptions) override;
		
		void 		step(float time) override;
		
/*********************************************************************************************
	Private
 *********************************************************************************************/

	private:

		std::shared_ptr<btDefaultCollisionConfiguration> 		_btCollisionConfiguration;
		std::shared_ptr<btCollisionDispatcher>					_btDispatcher;
		std::shared_ptr<btDbvtBroadphase>						_btBroadphase;
		std::shared_ptr<btSequentialImpulseConstraintSolver>	_btSolver;
		std::shared_ptr<btDiscreteDynamicsWorld>				_btWorld;
#ifdef DESKTOP
		std::shared_ptr<BulletDebugDrawer>						_debugDrawer;
#endif
		
		PhysicsBodyBTMapping									_bodyBTMapping;
		PhysicsShapeBTMapping									_shapeBTMapping;
		
		std::unordered_set<std::shared_ptr<ae::PhysicsBody>>	_activeBodies;
		std::unordered_set<std::shared_ptr<ae::PhysicsShape>>	_activeShapes;
	};
}


#endif /* BulletPhysicsSimulator_h */
