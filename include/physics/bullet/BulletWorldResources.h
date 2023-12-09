//
// Created by mkd on 12/8/23.
//

#ifndef AVARA_ENGINE_BULLETWORLDRESOURCES_H
#define AVARA_ENGINE_BULLETWORLDRESOURCES_H


#include <memory>

#include "physics/PhysicalWorldResources.h"


class btCollisionDispatcher;
struct btDbvtBroadphase;

class btDiscreteDynamicsWorld;
class btSequentialImpulseConstraintSolver;
class btDefaultCollisionConfiguration;


namespace ae {


#ifdef DESKTOP
	class BulletDebugDrawer;
#endif


	class BulletWorldResources : public PhysicalWorldResources {

/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

	public:

		BulletWorldResources();
		~BulletWorldResources();

/*********************************************************************************************
	Internal
 *********************************************************************************************/

		btDiscreteDynamicsWorld&				world() const;
		btDefaultCollisionConfiguration&		collisionConfiguration() const;
		btCollisionDispatcher&					collisionDispatcher() const;
		btDbvtBroadphase&						broadphase() const;
		btSequentialImpulseConstraintSolver&	constraintSolver() const;
#ifdef DESKTOP
		BulletDebugDrawer&						debugDrawer() const;
#endif

/*********************************************************************************************
	 Private
 *********************************************************************************************/

	private:

		std::unique_ptr<btDiscreteDynamicsWorld>				_world;
		std::unique_ptr<btDefaultCollisionConfiguration> 		_collisionConfiguration;
		std::unique_ptr<btCollisionDispatcher>					_collisionDispatcher;
		std::unique_ptr<btDbvtBroadphase>						_broadphase;
		std::unique_ptr<btSequentialImpulseConstraintSolver>	_constraintSolver;
#ifdef DESKTOP
		std::unique_ptr<BulletDebugDrawer>						_debugDrawer;
#endif
	};
}


#endif //AVARA_ENGINE_BULLETWORLDRESOURCES_H
