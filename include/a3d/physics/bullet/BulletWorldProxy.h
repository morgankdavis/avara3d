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

#include "glm/glm.hpp"

#include "a3d/physics/bullet/BulletStats.h"
#include "a3d/physics/proxy/PhysicalWorldProxy.h"


class btCollisionDispatcher;
struct btDbvtBroadphase;
class btDiscreteDynamicsWorld;
class btSequentialImpulseConstraintSolver;
class btDefaultCollisionConfiguration;


namespace a3d {


	class BulletDebugDrawer;


	class BulletWorldProxy : public PhysicalWorldProxy {

/*********************************************************************************************
	Internal Lifecycle Functions
 *********************************************************************************************/

	public:

		explicit BulletWorldProxy(PhysicalWorld& world);
		~BulletWorldProxy() override;

/*********************************************************************************************
	PhysicalWorldModelProxy Internal Member Functions
 *********************************************************************************************/

		void 	add(PhysicsBody& body) override;
		void 	remove(PhysicsBody& body) override;

		float	gravity() const override;
		void	gravity(float gravity) override;

		void	step(double deltaT,
					 float speed,
					 float timestep,
					 Stats& stats) override;

		void 	updateCollisionPairs() override;

		void 	drawDebug(Renderer &renderer,
						  const glm::mat4 &viewMat,
						  const glm::mat4 &projectionMat,
						  const DebugOptions &debugOptions) override;

/*********************************************************************************************
	 Private Member Variables
 *********************************************************************************************/

	private:

		std::unique_ptr<btDiscreteDynamicsWorld>				_btWorld;
		std::unique_ptr<btDefaultCollisionConfiguration> 		_btCollisionConfiguration;
		std::unique_ptr<btCollisionDispatcher>					_btCollisionDispatcher;
		std::unique_ptr<btDbvtBroadphase>						_btBroadphase;
		std::unique_ptr<btSequentialImpulseConstraintSolver>	_btConstraintSolver;
#ifdef DESKTOP
		std::unique_ptr<BulletDebugDrawer>						_btDebugDrawer;
#endif
		BulletStats												_stats;
	};
}


#endif //AVARA3D_BULLETWORLDPROXY_H
