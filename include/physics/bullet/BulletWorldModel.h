//
// Created by mkd on 12/8/23.
//

#ifndef AVARA_ENGINE_BULLETWORLDMODEL_H
#define AVARA_ENGINE_BULLETWORLDMODEL_H


#include <memory>

#include "physics/PhysicalWorldModel.h"


class btCollisionDispatcher;
struct btDbvtBroadphase;

class btDiscreteDynamicsWorld;
class btSequentialImpulseConstraintSolver;
class btDefaultCollisionConfiguration;


namespace ae {


#ifdef DESKTOP
	class BulletDebugDrawer;
#endif


	class BulletWorldModel : public PhysicalWorldModel {

/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

	public:

		BulletWorldModel(PhysicalWorld* world);
		~BulletWorldModel();

/*********************************************************************************************
	PhysicalWorldModel
 *********************************************************************************************/

		void 	add(PhysicsBody& body) override;
		void 	remove(PhysicsBody& body) override;

		float	gravity() const override;
		void	gravity(float gravity) override;

/*********************************************************************************************
	Internal
 *********************************************************************************************/

		btDiscreteDynamicsWorld*				btWorld() const;
//		btDefaultCollisionConfiguration*		collisionConfiguration() const;
//		btCollisionDispatcher*					collisionDispatcher() const;
//		btDbvtBroadphase*						broadphase() const;
//		btSequentialImpulseConstraintSolver*	constraintSolver() const;
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


#endif //AVARA_ENGINE_BULLETWORLDMODEL_H
