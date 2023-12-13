//
// Created by mkd on 12/8/23.
//

#include "physics/bullet/BulletWorldModel.h"

#include "btBulletCollisionCommon.h"
#include "btBulletDynamicsCommon.h"
#include "BulletCollision/Gimpact/btGImpactShape.h"

#include "diagnostic/logging/Logger.h"
#include "physics/bullet/BulletDebugDrawer.h"


using namespace ae;
using namespace std;


/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

BulletWorldModel::BulletWorldModel()/*:
		_collisionConfiguration(make_unique<btDefaultCollisionConfiguration>()),
		_collisionDispatcher(make_unique<btCollisionDispatcher>(_collisionConfiguration.get())),
		_broadphase(make_unique<btDbvtBroadphase>()),
		_constraintSolver(make_unique<btSequentialImpulseConstraintSolver>()),
		_world(make_unique<btDiscreteDynamicsWorld>(_collisionDispatcher.get(),
													_broadphase.get(),
													_constraintSolver.get(),
													_collisionConfiguration.get()))*/ {

	// putting this in the initializer list causes a SEGFAULT at btDiscreteDynamicsWorld::addRigidBody(). (?)

	_collisionConfiguration = make_unique<btDefaultCollisionConfiguration>();
	_collisionDispatcher = make_unique<btCollisionDispatcher>(_collisionConfiguration.get());
	_broadphase = make_unique<btDbvtBroadphase>();
	_constraintSolver = make_unique<btSequentialImpulseConstraintSolver>();
	_world = make_unique<btDiscreteDynamicsWorld>(_collisionDispatcher.get(),
												  _broadphase.get(),
												  _constraintSolver.get(),
												  _collisionConfiguration.get());

	AE_LOG_I("Bullet Physics version: {}",  btGetVersion());

#ifdef OPENGL_CORE
	_debugDrawer = make_unique<BulletDebugDrawer>();
	_world.get()->setDebugDrawer(_debugDrawer.get());
#endif
}

BulletWorldModel::~BulletWorldModel() {
	AE_LOG_D("Destroying BulletWorldModel {:p}", (void*)this);
}

/*********************************************************************************************
	Internal
 *********************************************************************************************/

btDiscreteDynamicsWorld* BulletWorldModel::world() const {
	return _world.get();
}

btDefaultCollisionConfiguration* BulletWorldModel::collisionConfiguration() const {
	return _collisionConfiguration.get();
}

btCollisionDispatcher* BulletWorldModel::collisionDispatcher() const {
	return _collisionDispatcher.get();
}

btDbvtBroadphase* BulletWorldModel::broadphase() const {
	return _broadphase.get();
}

btSequentialImpulseConstraintSolver* BulletWorldModel::constraintSolver() const {
	return _constraintSolver.get();
}

#ifdef DESKTOP
BulletDebugDrawer* BulletWorldModel::debugDrawer() const {
	return _debugDrawer.get();
}
#endif
