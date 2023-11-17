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

	class BulletBodyResources : public PhysicsBodyResources {

/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

	public:

		BulletBodyResources();
//		BulletBodyResources(std::shared_ptr<btRigidBody> body,
//							std::shared_ptr<btDefaultMotionState> motionState);
//		BulletBodyResources(std::shared_ptr<btRigidBody> body,
//							std::shared_ptr<btDefaultMotionState> motionState);
		~BulletBodyResources();

/*********************************************************************************************
	Public
 *********************************************************************************************/

		std::shared_ptr<btRigidBody>			body();
		void									body(std::shared_ptr<btRigidBody> body);

		std::shared_ptr<btDefaultMotionState>	motionState();
		void									motionState(std::shared_ptr<btDefaultMotionState> motionState);

//		void	update(PhysicsSimulator& simulator,
//					   FrameStats& stats) override;

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
