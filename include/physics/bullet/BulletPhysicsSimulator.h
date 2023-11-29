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

#include "physics/PhysicsSimulator.h"


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
	Lifecycle
 *********************************************************************************************/

	public:

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

		void 		beginUpdate(const Scene& scene) override;
		void 		endUpdate(const Scene& scene) override;

		void		update(Scene& scene) override;
		void		sync(Scene& scene) override;

		void		update(PhysicsBody& body,
						   Node& node) override;
		void		sync(PhysicsBody& body,
						 Node& node,
						 glm::mat4& worldTransform) override;

		void		update(PhysicsShape& shape,
						   PHYSICS_BODY_TYPE bodyType,
						   bool& updated) override;
		void		sync(PhysicsShape& shape,
						 PHYSICS_BODY_TYPE bodyType) override;

		void 		step(float deltaT) override;
		
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
	};
}


#endif /* BulletPhysicsSimulator_h */
