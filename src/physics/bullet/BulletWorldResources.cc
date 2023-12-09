//
// Created by mkd on 12/8/23.
//

#include "physics/bullet/BulletWorldResources.h"

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

BulletWorldResources::BulletWorldResources():
		_collisionConfiguration(make_unique<btDefaultCollisionConfiguration>()),
		_collisionDispatcher(make_unique<btCollisionDispatcher>(_collisionConfiguration.get())),
		_broadphase(make_unique<btDbvtBroadphase>()),
		_constraintSolver(make_unique<btSequentialImpulseConstraintSolver>()),
		_world(make_unique<btDiscreteDynamicsWorld>(_collisionDispatcher.get(),
													_broadphase.get(),
													_constraintSolver.get(),
													_collisionConfiguration.get())) {

#ifdef OPENGL_CORE
	_debugDrawer = make_unique<BulletDebugDrawer>();
	_world.get()->setDebugDrawer(_debugDrawer.get());
#endif
}

BulletWorldResources::~BulletWorldResources() {
	AE_LOG_D("Destroying BulletWorldResources {:p}", (void*)this);
}

/*********************************************************************************************
	Internal
 *********************************************************************************************/

btDiscreteDynamicsWorld& BulletWorldResources::world() const {
	return *_world;
}

btDefaultCollisionConfiguration& BulletWorldResources::collisionConfiguration() const {
	return *_collisionConfiguration;
}

btCollisionDispatcher& BulletWorldResources::collisionDispatcher() const {
	return *_collisionDispatcher;
}

btDbvtBroadphase& BulletWorldResources::broadphase() const {
	return *_broadphase;
}

btSequentialImpulseConstraintSolver& BulletWorldResources::constraintSolver() const {
	return *_constraintSolver;
}

#ifdef DESKTOP
BulletDebugDrawer& BulletWorldResources::debugDrawer() const {
	return *_debugDrawer;
}
#endif
