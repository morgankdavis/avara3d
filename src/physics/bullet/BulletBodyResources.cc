//
// Created by mkd on 10/29/23.
//

#include "physics/bullet/BulletBodyResources.h"

#include "diagnostic/logging/Logger.h"


using namespace ae;
using namespace std;


/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

BulletBodyResources::BulletBodyResources(shared_ptr<btRigidBody> body,
										 shared_ptr<btDefaultMotionState> motionState):
		_body(body),
		_motionState(motionState) {

}

BulletBodyResources::~BulletBodyResources() {
	AE_LOG_D("Destroying BulletBodyResources {:p}", (void*)this);

//	_simulator->DestroyBodyResources(_body);
}

/*********************************************************************************************
	Public
 *********************************************************************************************/

shared_ptr<btRigidBody> BulletBodyResources::body() {
	return _body;
}

shared_ptr<btDefaultMotionState> BulletBodyResources::motionState() {
	return _motionState;
}

//void BulletBodyResources::update(PhysicsSimulator& simulator,
//								 FrameStats& stats) {
//
//}
