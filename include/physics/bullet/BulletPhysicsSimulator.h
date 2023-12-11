//
//  BulletPhysicsSimulator.h
//	avara-engine
//
//  Created by Morgan Davis on 5/1/18.
//  Copyright © 2018 Morgan K Davis. All rights reserved.
//

#ifndef BulletPhysicsSimulator_h
#define BulletPhysicsSimulator_h


#include "physics/PhysicsSimulator.h"


namespace ae {


	class Renderer;

	
	class BulletPhysicsSimulator : public PhysicsSimulator {

/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

	public:

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
