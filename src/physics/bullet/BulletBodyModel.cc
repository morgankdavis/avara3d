//
// Created by mkd on 10/29/23.
//

#include "physics/bullet/BulletBodyModel.h"

#include "diagnostic/logging/Logger.h"


using namespace ae;
using namespace std;


/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

BulletBodyModel::BulletBodyModel():
		_body(nullptr),
		_motionState(nullptr) { }

BulletBodyModel::~BulletBodyModel() {
	AE_LOG_D("Destroying BulletBodyModel {:p}", (void*)this);
}

/*********************************************************************************************
	Public
 *********************************************************************************************/

shared_ptr<btRigidBody> BulletBodyModel::body() {
	return _body;
}

void BulletBodyModel::body(shared_ptr<btRigidBody> body) {
	_body = body;
}

shared_ptr<btDefaultMotionState> BulletBodyModel::motionState() {
	return _motionState;
}

void BulletBodyModel::motionState(shared_ptr<btDefaultMotionState> motionState) {
	_motionState = motionState;
}
