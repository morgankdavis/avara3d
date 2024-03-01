//
// Created by mkd on 10/29/23.
//

#ifndef BulletBodyResources_h
#define BulletBodyResources_h


#include <memory>

#include "a3d/physics/bullet/MotionState.h"
#include "a3d/physics/proxy/PhysicsBodyProxy.h"


class btRigidBody;


namespace a3d {

	class BulletBodyProxy : public PhysicsBodyProxy {

/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

	public:

		BulletBodyProxy(PhysicsBody* body);
		~BulletBodyProxy() override;

/*********************************************************************************************
	PhysicsBodyModelProxy
 *********************************************************************************************/

		PhysicsBodyType		type() const override;
		void					type(PhysicsBodyType type) override;

		PhysicsShapeProxy*		shapeProxy() const override;
		void					shapeProxy(PhysicsShapeProxy* proxy) override;

		float					mass() const override;
		void					mass(float mass) override;

		glm::vec3				momentOfInertia() const override;
		void					momentOfInertia(const glm::vec3& moment) override;

		glm::vec3				centerOfMass() const override;
		void					centerOfMass(const glm::vec3 offset) override;

		float					friction() const override;
		void					friction(float friction) override;

		float					rollingFriction() const override;
		void					rollingFriction(float friction) override;

		float					restitution() const override;
		void					restitution(float restitution) override;

		glm::vec3				linearVelocity() const override;
		void					linearVelocity(const glm::vec3& velocity) override;

		glm::vec3				angularVelocity() const override;
		void					angularVelocity(const glm::vec3& velocity) override;

		glm::vec3				linearFactor() const override;
		void					linearFactor(const glm::vec3& factor) override;

		glm::vec3				angularFactor() const override;
		void					angularFactor(const glm::vec3& factor) override;

		float					linearDamping() const override;
		void					linearDamping(float damping) override;

		float					angularDamping() const override;
		void					angularDamping(float damping) override;

		float					linearSleepingThreshold() const override;
		void					linearSleepingThreshold(float threshold) override;

		float					angularSleepingThreshold() const override;
		void					angularSleepingThreshold(float threshold) override;

		void					applyForce(const glm::vec3& force, const glm::vec3& location) override;
		void					applyCentralForce(const glm::vec3& force) override;
		void					applyImpulse(const glm::vec3& impulse, const glm::vec3& location) override;
		void					applyCentralImpulse(const glm::vec3& impulse) override;

		void					applyTorque(const glm::vec3& torque) override;
		void					applyTorqueImpulse(const glm::vec3& torque) override;

		glm::vec3 				totalForce() const override;
		glm::vec3 				totalTorque() const override;

		bool					affectedByGravity() const override;
		void					affectedByGravity(bool affectedByGravity) override;

		bool					allowsResting() const override;
		void					allowsResting(bool allowsResting) override;

		bool 					resting() const override;
		void					resting(bool resting) override;

//		glm::mat4				worldTransform() const override;
//		void					worldTransform(const glm::mat4& transform) override;

		void					worldTransform(const glm::mat4& transform) override;

		void					clearForces() override;

/*********************************************************************************************
	Internal
 *********************************************************************************************/

		std::shared_ptr<btRigidBody>			btBody();
		//std::shared_ptr<btDefaultMotionState>	btMotionState();
		std::shared_ptr<MotionState>			motionState();

/*********************************************************************************************
	 Private
 *********************************************************************************************/

	private:

		void				calculateMomentOfIntertia();

		std::shared_ptr<btRigidBody>			_btBody;
		//std::shared_ptr<btDefaultMotionState>	_btMotionState;
		std::shared_ptr<MotionState>			_motionState;
	};
}


#endif //BulletBodyResources_h
