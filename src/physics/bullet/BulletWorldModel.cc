//
// Created by mkd on 12/8/23.
//

#include "physics/bullet/BulletWorldModel.h"

#include "btBulletCollisionCommon.h"
#include "btBulletDynamicsCommon.h"
#include "BulletCollision/Gimpact/btGImpactShape.h"

#include "diagnostic/logging/Logger.h"
#include "physics/bullet/BulletDebugDrawer.h"
#include "physics/bullet/BulletPhysicsSimulator.h"


using namespace ae;
using namespace std;


/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

BulletWorldModel::BulletWorldModel(PhysicalWorld* world):
		PhysicalWorldModel(world)/*:
		_collisionConfiguration(make_unique<btDefaultCollisionConfiguration>()),
		_collisionDispatcher(make_unique<btCollisionDispatcher>(_collisionConfiguration.get())),
		_broadphase(make_unique<btDbvtBroadphase>()),
		_constraintSolver(make_unique<btSequentialImpulseConstraintSolver>()),
		_world(make_unique<btDiscreteDynamicsWorld>(_collisionDispatcher.get(),
													_broadphase.get(),
													_constraintSolver.get(),
													_collisionConfiguration.get()))*/ {

	// putting this in the initializer list causes a SEGFAULT at btDiscreteDynamicsWorld::addRigidBody(). (?)

	_btCollisionConfiguration = make_unique<btDefaultCollisionConfiguration>();
	_btCollisionDispatcher = make_unique<btCollisionDispatcher>(_btCollisionConfiguration.get());
	_btBroadphase = make_unique<btDbvtBroadphase>();
	_btConstraintSolver = make_unique<btSequentialImpulseConstraintSolver>();
	_btWorld = make_unique<btDiscreteDynamicsWorld>(_btCollisionDispatcher.get(),
												  _btBroadphase.get(),
												  _btConstraintSolver.get(),
												  _btCollisionConfiguration.get());

	AE_LOG_I("Bullet Physics version: {}",  btGetVersion());

#ifdef OPENGL_CORE
	_btDebugDrawer = make_unique<BulletDebugDrawer>();
	_btWorld.get()->setDebugDrawer(_btDebugDrawer.get());
#endif
}

BulletWorldModel::~BulletWorldModel() {
	AE_LOG_D("Destroying BulletWorldModel {:p}", (void*)this);
}

/*********************************************************************************************
	PhysicalWorldModel
 *********************************************************************************************/

void BulletWorldModel::add(PhysicsBody& body) {
	//_btWorld->addRigidBody(body.resources.)
}

void BulletWorldModel::remove(PhysicsBody& body) {

}

float BulletWorldModel::gravity() const {
	return _btWorld->getGravity().y();
}

void BulletWorldModel::gravity(float gravity) {
	_btWorld->setGravity({0, gravity, 0});
}

/*********************************************************************************************
	Internal
 *********************************************************************************************/

btDiscreteDynamicsWorld* BulletWorldModel::btWorld() const {
	return _btWorld.get();
}

//btDefaultCollisionConfiguration* BulletWorldModel::collisionConfiguration() const {
//	return _btCollisionConfiguration.get();
//}
//
//btCollisionDispatcher* BulletWorldModel::collisionDispatcher() const {
//	return _collisionDispatcher.get();
//}
//
//btDbvtBroadphase* BulletWorldModel::broadphase() const {
//	return _broadphase.get();
//}
//
//btSequentialImpulseConstraintSolver* BulletWorldModel::constraintSolver() const {
//	return _constraintSolver.get();
//}

#ifdef DESKTOP
BulletDebugDrawer* BulletWorldModel::btDebugDrawer() const {
	return _btDebugDrawer.get();
}
#endif
