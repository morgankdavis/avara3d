//
// Created by mkd on 12/8/23.
//

#ifndef AVARA_ENGINE_BULLETWORLDMODEL_H
#define AVARA_ENGINE_BULLETWORLDMODEL_H


#include <memory>

#include "glm/glm.hpp"
#include "LinearMath/btQuaternion.h"
#include "LinearMath/btTransform.h"
#include "LinearMath/btVector3.h"

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

		// move?
/*********************************************************************************************
	Public Static
 *********************************************************************************************/

	public:

		static glm::vec3 							GLMVec3FromBTVector3(const btVector3& from);
		static glm::vec4 							GLMVec4FromBTVector4(const btVector4& from);
		static glm::mat4 							GLMMat4FromBTTransform(const btTransform& from);
		static btVector3 						BTVector3FromGLMVec3(const glm::vec3& from);
		static btVector4 						BTVector4FromGLMVec4(const glm::vec4& from);
		static btQuaternion 					BTQuaternionFromGLMQuat(const glm::quat& from);
		static btTransform 						BTTransformFromGLMMat4(const glm::mat4& from);
		static glm::mat4 							TransformByRemovingScale(const glm::mat4& m, bool& scaled);
		static btTransform&						BTIdentityTransform();

/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

//	public:

		BulletWorldModel(PhysicalWorld* world);
		~BulletWorldModel();

/*********************************************************************************************
	PhysicalWorldModel
 *********************************************************************************************/

		void 	add(PhysicsBody& body) override;
		void 	remove(PhysicsBody& body) override;

		float	gravity() const override;
		void	gravity(float gravity) override;

		void	update(Stats& stats) override;
		void	step(double deltaT, float speed, float timestep) override;
		void	sync() override;

		void 	drawDebug(Renderer &renderer,
						  const glm::mat4 &viewMat,
						  const glm::mat4 &projectionMat,
						  const DEBUG_OPTIONS &debugOptions) override;

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
