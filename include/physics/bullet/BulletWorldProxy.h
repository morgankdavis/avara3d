//
// Created by mkd on 12/8/23.
//

#ifndef AVARA_ENGINE_BULLETWORLDPROXY_H
#define AVARA_ENGINE_BULLETWORLDPROXY_H


#include <memory>

#include "glm/glm.hpp"

#include "physics/model_proxy/PhysicalWorldModelProxy.h"


class btCollisionDispatcher;
struct btDbvtBroadphase;

class btDiscreteDynamicsWorld;
class btSequentialImpulseConstraintSolver;
class btDefaultCollisionConfiguration;


namespace ae {


#ifdef DESKTOP
	class BulletDebugDrawer;
#endif


	class BulletWorldProxy : public PhysicalWorldModelProxy {

/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

	public:

		BulletWorldProxy(PhysicalWorld* world);
		//BulletWorldProxy() override;
		~BulletWorldProxy();

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
						  const DEBUG_OPTIONS &debugOptions) override;

/*********************************************************************************************
	Internal
 *********************************************************************************************/

		btDiscreteDynamicsWorld*				btWorld() const;
#ifdef DESKTOP
		BulletDebugDrawer*						btDebugDrawer() const;
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


#endif //AVARA_ENGINE_BULLETWORLDPROXY_H
