//
//  BulletPhysicsSimulator.h
//	avara-engine
//
//  Created by Morgan Davis on 5/1/18.
//  Copyright © 2018 Morgan K Davis. All rights reserved.
//

#ifndef BulletPhysicsSimulator_h
#define BulletPhysicsSimulator_h


#include "glm/glm.hpp"
#include "LinearMath/btQuaternion.h"
#include "LinearMath/btTransform.h"
#include "LinearMath/btVector3.h"
//#include "LinearMath/btIDebugDraw.h"

#include "physics/PhysicsSimulator.h"




namespace ae {


	class Renderer;

	
	class BulletPhysicsSimulator : public PhysicsSimulator {


	public:




//		static btIDebugDraw::DebugDrawModes 	BTDebugDrawModesForAEDebugOptions(const DEBUG_OPTIONS& options);
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



		BulletPhysicsSimulator();
		BulletPhysicsSimulator(const BulletPhysicsSimulator& other) = delete; // copy constructor
		BulletPhysicsSimulator& operator=(const BulletPhysicsSimulator& other) = delete; // copy assignment
		~BulletPhysicsSimulator();
		
/*********************************************************************************************
	Internal
 *********************************************************************************************/

		void 		drawDebug(const PhysicalWorld &world,
							  Renderer &renderer,
							  const glm::mat4 &viewMat,
							  const glm::mat4 &projectionMat,
							  const DEBUG_OPTIONS &debugOptions);

/*********************************************************************************************
	PhysicsSimulator
 *********************************************************************************************/

		void		create(PhysicalWorld& world);
		void 		setGravity(PhysicalWorld& world, glm::vec3& gravity) override;

		void		create(PhysicsBody& body) override;
		void		remove(PhysicsBody& body) override;

		void		setType(PhysicsBody& body, PHYSICS_BODY_TYPE type) override;
		void		setShape(PhysicsBody& body, PhysicsShape& shape) override;
//		void		setWorldTransform(PhysicsBody& body, const glm::mat4& transform) override;
		void		setMass(PhysicsBody& body, float mass) override;
		void		setMomentOfInertia(PhysicsBody& body, const glm::vec3& moment) override;
		void		setFriction(PhysicsBody& body, float friction) override;
		void		setRollingFriction(PhysicsBody& body, float friction) override;
		void		setRestitution(PhysicsBody& body, float restitution) override;
		void		setLinearVelocity(PhysicsBody& body, const glm::vec3& velocity) override;
		void		setAngularVelocity(PhysicsBody& body, const glm::vec3& velocity) override;
		void		setLinearFactor(PhysicsBody& body, const glm::vec3& factor) override;
		void		setAngularFactor(PhysicsBody& body, const glm::vec3& factor) override;
		void		setLinearDamping(PhysicsBody& body, float damping) override;
		void		setAngularDamping(PhysicsBody& body, float damping) override;
		void		setLinearSleepingThreshold(PhysicsBody& body, float threshold) override;
		void		setAngularSleepingThreshold(PhysicsBody& body, float threshold) override;
		void		setAffectedByGravity(PhysicsBody& body, bool flag) override;
		void		setAllowsResting(PhysicsBody& body, bool flag) override;

		void		create(PhysicsShape& shape) override;

		void		update(PhysicalWorld& world, Stats& stats) override;
		void 		step(PhysicalWorld& world, double deltaT) override;
		void		sync(PhysicalWorld& world) override;

//		void		sync(PhysicsBody& body,
//						 glm::mat4& worldTransform) override;

/*********************************************************************************************
	Private
 *********************************************************************************************/

	private:

	};
}


#endif /* BulletPhysicsSimulator_h */
