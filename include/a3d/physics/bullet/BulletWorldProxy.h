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

#include "a3d/mesh/Line.h"
#include "a3d/physics/bullet/BulletStats.h"
#include "a3d/physics/proxy/PhysicalWorldProxy.h"

class btCollisionDispatcher;
struct btDbvtBroadphase;
class btDiscreteDynamicsWorld;
class btSequentialImpulseConstraintSolver;
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

		std::vector<Line> 	debugLines(const DebugOptions &debugOptions) override;

	private:
		///  Private Member Variables ///

		std::unique_ptr<btDiscreteDynamicsWorld>				_btWorld;
		std::unique_ptr<btDefaultCollisionConfiguration> 		_btCollisionConfiguration;
		std::unique_ptr<btCollisionDispatcher>					_btCollisionDispatcher;
		std::unique_ptr<btDbvtBroadphase>						_btBroadphase;
		std::unique_ptr<btSequentialImpulseConstraintSolver>	_btConstraintSolver;
#ifdef A3D_GL_DESKTOP
		std::unique_ptr<BulletDebugDrawer>						_btDebugDrawer;
		std::vector<Line> 										_debugLines;
#endif
		BulletStats												_stats;
	};
}

#endif //AVARA3D_BULLETWORLDPROXY_H
