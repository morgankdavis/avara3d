//
// Created by mkd on 10/29/23.
//

#ifndef BulletBodyResources_h
#define BulletBodyResources_h


#include <memory>

#include "physics/PhysicsBodyModel.h"


struct btDefaultMotionState;

class btRigidBody;


namespace ae {

	class BulletBodyModel : public PhysicsBodyModel {

/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

	public:

		BulletBodyModel(PhysicsBody* body);
		~BulletBodyModel();

/*********************************************************************************************
	PhysicsBodyModel
 *********************************************************************************************/

		PHYSICS_BODY_TYPE	type() const override;
		void				type(PHYSICS_BODY_TYPE type) override;

		PhysicsShapeModel*	shape() const override;
		void				shape(PhysicsShapeModel* shape) override;

//		glm::mat4			worldTransform() const override;
//		void				worldTransform(const glm::mat4& transform) override;

		float				mass() const override;
		void				mass(float mass) override;

		glm::vec3			momentOfInertia() const override;
		void				momentOfInertia(const glm::vec3& moment) override;

		float				friction() const override;
		void				friction(float friction) override;

		float				rollingFriction() const override;
		void				rollingFriction(float friction) override;

		float				restitution() const override;
		void				restitution(float restitution) override;

		glm::vec3			linearVelocity() const override;
		void				linearVelocity(const glm::vec3& velocity) override;

		glm::vec3			angularVelocity() const override;
		void				angularVelocity(const glm::vec3& velocity) override;

		glm::vec3			linearFactor() const override;
		void				linearFactor(const glm::vec3& factor) override;

		glm::vec3			angularFactor() const override;
		void				angularFactor(const glm::vec3& factor) override;

		float				linearDamping() const override;
		void				linearDamping(float damping) override;

		float				angularDamping() const override;
		void				angularDamping(float damping) override;

		float				linearSleepingThreshold() const override;
		void				linearSleepingThreshold(float threshold) override;

		float				angularSleepingThreshold() const override;
		void				angularSleepingThreshold(float threshold) override;

		bool				affectedByGravity() const override;
		void				affectedByGravity(bool affectedByGravity) override;

		bool				allowsResting() const override;
		void				allowsResting(bool allowsResting) override;

/*********************************************************************************************
	Internal
 *********************************************************************************************/

		std::shared_ptr<btRigidBody>			btBody();
		void									btBody(std::shared_ptr<btRigidBody> body);

		std::shared_ptr<btDefaultMotionState>	btMotionState();
		void									btMotionState(std::shared_ptr<btDefaultMotionState> motionState);

/*********************************************************************************************
	 Private
 *********************************************************************************************/

	private:

		PhysicsBody*							_body;
		std::shared_ptr<btRigidBody>			_btBody;
		std::shared_ptr<btDefaultMotionState>	_btMotionState;
	};
}


#endif //BulletBodyResources_h
