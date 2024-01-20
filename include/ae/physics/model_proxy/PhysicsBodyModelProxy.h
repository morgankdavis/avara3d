//
// Created by mkd on 11/13/23.
//

#ifndef AVARA_ENGINE_PHYSICSBODYMODELPROXY_H
#define AVARA_ENGINE_PHYSICSBODYMODELPROXY_H


#include "glm/glm.hpp"

#include "ae/Types.h"


namespace ae {


	class PhysicsBody;
	class PhysicsShapeModelProxy;


	class PhysicsBodyModelProxy {

/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

	public:

		PhysicsBodyModelProxy(PhysicsBody* body);
		//virtual PhysicsBodyModelProxyProxy();
		~PhysicsBodyModelProxy();

/*********************************************************************************************
	Internal
 *********************************************************************************************/

		virtual PHYSICS_BODY_TYPE		type() const = 0;
		virtual void					type(PHYSICS_BODY_TYPE type) = 0;

		virtual PhysicsShapeModelProxy*	shapeProxy() const = 0;
		virtual void					shapeProxy(PhysicsShapeModelProxy* proxy) = 0;

//		virtual glm::mat4				worldTransform() const = 0;
//		virtual void					worldTransform(const glm::mat4& transform) = 0;

		virtual float					mass() const = 0;
		virtual void					mass(float mass) = 0;

		virtual glm::vec3				momentOfInertia() const = 0;
		virtual void					momentOfInertia(const glm::vec3& moment) = 0;

		virtual glm::vec3				centerOfMass() const = 0;
		virtual void					centerOfMass(const glm::vec3 offset) = 0;

		virtual float					friction() const = 0;
		virtual void					friction(float friction) = 0;

		virtual float					rollingFriction() const = 0;
		virtual void					rollingFriction(float friction) = 0;

		virtual float					restitution() const = 0;
		virtual void					restitution(float restitution) = 0;

		virtual glm::vec3				linearVelocity() const = 0;
		virtual void					linearVelocity(const glm::vec3& velocity) = 0;

		virtual glm::vec3				angularVelocity() const = 0;
		virtual void					angularVelocity(const glm::vec3& velocity) = 0;

		virtual glm::vec3				linearFactor() const = 0;
		virtual void					linearFactor(const glm::vec3& factor) = 0;

		virtual glm::vec3				angularFactor() const = 0;
		virtual void					angularFactor(const glm::vec3& factor) = 0;

		virtual float					linearDamping() const = 0;
		virtual void					linearDamping(float damping) = 0;

		virtual float					angularDamping() const = 0;
		virtual void					angularDamping(float damping) = 0;

		virtual float					linearSleepingThreshold() const = 0;
		virtual void					linearSleepingThreshold(float threshold) = 0;

		virtual float					angularSleepingThreshold() const = 0;
		virtual void					angularSleepingThreshold(float threshold) = 0;

		virtual void					applyForce(const glm::vec3& force, const glm::vec3& location) = 0;
		virtual void					applyCentralForce(const glm::vec3& force) = 0;
		virtual void					applyImpulse(const glm::vec3& impulse, const glm::vec3& location) = 0;
		virtual void					applyCentralImpulse(const glm::vec3& impulse) = 0;

		virtual void					applyTorque(const glm::vec3& torque) = 0;
		virtual void					applyTorqueImpulse(const glm::vec3& torque) = 0;

		virtual glm::vec3 				totalForce() const = 0;
		virtual glm::vec3 				totalTorque() const = 0;

		virtual bool					affectedByGravity() const = 0;
		virtual void					affectedByGravity(bool affectedByGravity) = 0;

		virtual bool					allowsResting() const = 0;
		virtual void					allowsResting(bool allowsResting) = 0;

		virtual bool 					resting() const = 0;
		virtual void					resting(bool resting) = 0;

		virtual void					clearForces() = 0;

		virtual void					worldTransform(const glm::mat4& worldTransform) = 0;

		virtual bool					autocalculatesMomentOfInertia() const;
		virtual void					autocalculatesMomentOfInertia(bool autocalculate);

		void							attachedToBody(PhysicsBody* body);
		void							detachedFromBody(PhysicsBody* body);

/*********************************************************************************************
	Protected
 *********************************************************************************************/

	protected:

		PhysicsBody*					_body;
		PhysicsShapeModelProxy*			_shapeModel;
		bool							_autocalculatesMomentOfInertia;
	};
}


#endif //AVARA_ENGINE_PHYSICSBODYMODELPROXY_H
