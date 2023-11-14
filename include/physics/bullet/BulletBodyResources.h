//
// Created by mkd on 10/29/23.
//

#ifndef BulletBodyResources_h
#define BulletBodyResources_h


#include <memory>

#include "physics/PhysicsBodyResources.h"


struct btDefaultMotionState;

class btRigidBody;


namespace ae {


//	class BulletPhysicsSimulator;


	class BulletBodyResources : PhysicsBodyResources {

/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

	public:

		BulletBodyResources(std::shared_ptr<btRigidBody> body,
							std::shared_ptr<btDefaultMotionState> motionState);
		~BulletBodyResources();

/*********************************************************************************************
	Public
 *********************************************************************************************/

		std::shared_ptr<btRigidBody>&			body();
		std::shared_ptr<btDefaultMotionState>&	motionState();

/*********************************************************************************************
	 Private
 *********************************************************************************************/

	private:

		std::shared_ptr<btRigidBody>			_body;
		std::shared_ptr<btDefaultMotionState>	_motionState;

//		std::weak_ptr<BulletPhysicsSimulator>	_simulator;
	};
}


#endif //BulletBodyResources_h
