//
//  BulletWorldProxy.h
//  avara3d
//
//  Created by Morgan Davis on 12/8/23.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_BULLETWORLDPROXY_H
#define AVARA3D_BULLETWORLDPROXY_H

#include <memory>
#include <mutex>
#include <vector>

#include "a3d/mesh/Line.h"
#include "a3d/physics/backend/bullet/BulletStats.h"
#include "a3d/physics/proxy/PhysicalWorldProxy.h"

struct btDbvtBroadphase;

class btCollisionDispatcher;
class btConstraintSolver;
class btConstraintSolverPoolMt;
class btDiscreteDynamicsWorld;
class btITaskScheduler;
class btSequentialImpulseConstraintSolver;
class btSequentialImpulseConstraintSolverMt;
class btDefaultCollisionConfiguration;

namespace a3d {

	class BulletDebugDrawer;
	class RenderContext;

	class BulletWorldProxy : public PhysicalWorldProxy {

	public:
		/// Internal Lifecycle Functions ///

		explicit BulletWorldProxy(PhysicalWorld& world);
		~BulletWorldProxy() override;

		/// PhysicalWorldModelProxy Internal Member Functions ///

		void 				add(PhysicsBody& body) override;
		void 				remove(PhysicsBody& body) override;

		float				gravity() const override;
		void				gravity(float gravity) override;

		void				step(double deltaT,
								 float speed,
								 float timestep,
								 FrameStats& stats,
								 Profiler& profiler) override;

		void 				updateCollisionPairs() override;

		void 				appendDebugLines(std::vector<Line>& out,
											 DebugOptions debugOptions) override;

		/// Internal Member Functions ///

		btDiscreteDynamicsWorld* 								btWorld();

	private:
		///  Private Member Variables ///

		// scheduler
		btITaskScheduler* 										_btScheduler = nullptr;
		std::unique_ptr<btITaskScheduler> 						_ownedScheduler;
		btITaskScheduler* 										_prevScheduler = nullptr; // non-owning

		// config/dispatcher/broadphase
		std::unique_ptr<btDefaultCollisionConfiguration> 		_btCollisionConfiguration;
		std::unique_ptr<btCollisionDispatcher> 					_btCollisionDispatcher;
		std::unique_ptr<btDbvtBroadphase> 						_btBroadphase;

		// solvers
		std::unique_ptr<btConstraintSolverPoolMt> 				_btSolverPool;
		std::unique_ptr<btSequentialImpulseConstraintSolverMt> 	_btSolverMt;

#ifdef A3D_GL_DESKTOP
		std::unique_ptr<BulletDebugDrawer> 						_btDebugDrawer;
		std::vector<Line> 										_debugLines;
#endif

		BulletStats 											_stats;

		mutable std::mutex 										_btMutex;

		// MUST be last so destroyed first
		std::unique_ptr<btDiscreteDynamicsWorld> 				_btWorld;
	};
}

#endif //AVARA3D_BULLETWORLDPROXY_H
