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

#include "a3d/physics/proxy/PhysicalWorldProxy.h"


class btCollisionDispatcher;
struct btDbvtBroadphase;
class btDiscreteDynamicsWorld;
class btSequentialImpulseConstraintSolver;
class btDefaultCollisionConfiguration;


namespace a3d {


#ifdef DESKTOP
	class BulletDebugDrawer;
#endif


	class BulletWorldProxy : public PhysicalWorldProxy {

/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

	public:

		explicit BulletWorldProxy(PhysicalWorld& world);
		~BulletWorldProxy() override;

/*********************************************************************************************
	PhysicalWorldModelProxy
 *********************************************************************************************/

		void 	add(PhysicsBody& body) override;
		void 	remove(PhysicsBody& body) override;

		float	gravity() const override;
		void	gravity(float gravity) override;

		void	step(double deltaT, float speed, float timestep) override;

		void 	updateCollisionPairs() override;

		void 	drawDebug(Renderer &renderer,
						  const glm::mat4 &viewMat,
						  const glm::mat4 &projectionMat,
						  const DebugOptions &debugOptions) override;

/*********************************************************************************************
	Internal
 *********************************************************************************************/

		btDiscreteDynamicsWorld*		btWorld() const;
#ifdef DESKTOP
		BulletDebugDrawer*				btDebugDrawer() const;
#endif

/*********************************************************************************************
	 Private
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
	};
}


#endif //AVARA3D_BULLETWORLDPROXY_H
