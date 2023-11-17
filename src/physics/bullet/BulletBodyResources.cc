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

BulletBodyResources::BulletBodyResources():
		_body(nullptr),
		_motionState(nullptr) { }

//BulletBodyResources::BulletBodyResources(shared_ptr<btRigidBody> body,
//										 shared_ptr<btDefaultMotionState> motionState):
//		_body(body),
//		_motionState(motionState) {
//
//}

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

void BulletBodyResources::body(shared_ptr<btRigidBody> body) {
	_body = body;
}

shared_ptr<btDefaultMotionState> BulletBodyResources::motionState() {
	return _motionState;
}

void BulletBodyResources::motionState(shared_ptr<btDefaultMotionState> motionState) {
	_motionState = motionState;
}

//void BulletBodyResources::update(PhysicsSimulator& simulator,
//								 FrameStats& stats) {
//
//}
