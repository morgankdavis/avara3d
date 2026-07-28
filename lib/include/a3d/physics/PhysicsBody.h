//
//  PhysicsBody.h
//  avara3d
//
//  Created by Morgan Davis on 1/26/18.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_PHYSICS_PHYSICSBODY_H
#define AVARA3D_PHYSICS_PHYSICSBODY_H

#include <memory>

#include "a3d/Math.h"

namespace a3d {

	class Mesh;
	class Node;
	class PhysicsWorld;
	class PhysicsBodyProxy;
	class PhysicsShapeProxy;
	class PhysicsShape;
	class Scene;

	class PhysicsBody {

	public:
		/// Public Types ///

		enum class Type : uint8_t {
			Static,
			Dynamic,
			Kinematic
		};

		/// Public Static Member Functions ///

		static std::unique_ptr<PhysicsBody> 	StaticBody();
		static std::unique_ptr<PhysicsBody> 	DynamicBody();
		static std::unique_ptr<PhysicsBody> 	KinematicBody();

		/// Public Lifecycle Functions ///

		explicit PhysicsBody(Type type);
		PhysicsBody(Type type, const std::shared_ptr<PhysicsShape>& shape);

		PhysicsBody(const PhysicsBody&) = delete;
		PhysicsBody& operator=(const PhysicsBody&) = delete;

		PhysicsBody(PhysicsBody&&) = delete;
		PhysicsBody& operator=(PhysicsBody&&) = delete;

		~PhysicsBody();

		/// Public Member Functions ///

		Type 								type() const;
		void 								type(Type type);

		const std::shared_ptr<PhysicsShape>&	shape() const;
		void 									shape(const std::shared_ptr<PhysicsShape>& shape);

		float 								mass() const;
		void 								mass(float mass);
		
		math::vec3 							momentOfInertia() const;
		void 								momentOfInertia(const math::vec3& moment);

		math::vec3							centerOfMass() const;
		void								centerOfMass(const math::vec3& offset);

		float 								friction() const;
		void 								friction(float friction);
		
		float 								rollingFriction() const;
		void 								rollingFriction(float friction);

		float 								restitution() const;
		void 								restitution(float restitution);

		math::vec3 							linearVelocity() const;
		void 								linearVelocity(math::vec3 velocity);

		math::vec3 							angularVelocity() const;
		void 								angularVelocity(const math::vec3& velocity);

		math::vec3							linearFactor() const;
		void 								linearFactor(const math::vec3& factor);

		math::vec3							angularFactor() const;
		void 								angularFactor(const math::vec3& factor);

		float								linearDamping() const;
		void 								linearDamping(float damping);

		float								angularDamping() const;
		void 								angularDamping(float damping);

		float								linearSleepingThreshold() const;
		void 								linearSleepingThreshold(float threshold);

		float								angularSleepingThreshold() const;
		void 								angularSleepingThreshold(float threshold);
		
		// categoryBitmask
		// contactTestBitmask
		// collisionBitmask

		void 								applyForce(const math::vec3& force,
													   bool impulse);
		void 								applyForce(const math::vec3& force,
													   const math::vec3& location,
													   bool impulse);
		void 								applyTorque(const math::vec3& torque,
														bool impulse);

		math::vec3							totalForce() const;
		math::vec3 							totalTorque() const;

		void 								clearForces();

		void								ccdEnabled(bool enabled);
		bool								ccdEnabled() const;

		void								ccdMotionThreshold(float distance);
		float								ccdMotionThreshold() const;

		void								ccdSweptSphereRadius(float radius);
		float								ccdSweptSphereRadius() const;

		bool 								affectedByGravity() const;
		void 								affectedByGravity(bool affectedByGravity);

		bool 								allowsResting() const;
		void 								allowsResting(bool allowsResting);

		bool 								resting() const;
		void								resting(bool resting);

		bool								autocalculatesMomentOfInertia() const;
		void								autocalculatesMomentOfInertia(bool autocalculate);

		/// Internal Member Functions ///
		
//		void 								resting(bool resting);
		
		void 								attachedToNode(const std::shared_ptr<Node>& node);
		void 								detachedFromNode(const std::shared_ptr<Node>& node);

//		void 								nodeAttachedToParent(Node* parent); // owning node's parent
//		void 								nodeDetachedFromParent(Node* parent);
//
//		void 								nodeAttachedToScene(Scene* scene); // owning node
//		void 								nodeDetachedFromScene(Scene* scene);

		void 								meshAttachedToNode(const std::shared_ptr<Mesh>& mesh); // owning node's mesh
		void 								meshDetachedFromNode(const std::shared_ptr<Mesh>& mesh);

		void								physicsWorldReachable(PhysicsWorld& world);
		void								physicsWorldUnreachable(PhysicsWorld& world);

//		void 								ancestorAttachedToParent(Node* ancestor,
//																	 Node* parent);
//		void 								ancestorDetachedFromParent(Node* ancestor,
//																	   Node* parent);
//
//		void								ancestorAttachedToScene(Node* ancestor,
//																	Scene* scene);
//		void								ancestorDetachedFromScene(Node* ancestor,
//																	  Scene* scene);
//
//		void								physicsWorldAttachedToScene(PhysicsWorld* world,
//																		 Scene* scene);
//		void								physicsWorldDetachedFromScene(PhysicsWorld* world,
//																		   Scene* scene);

		void 								addedToWorld(PhysicsWorld& world);
		void 								removedFromWorld(PhysicsWorld& world);

		void 								shapeWillUpdate();
		void 								shapeDidUpdate();

		std::weak_ptr<Node>					node() const;

		// the scene's world, if it exists.  not the same as _world.
		PhysicsWorld*						physicsWorld() const;

		PhysicsBodyProxy*					proxy() const;

	private:
		/// Private Member Functions ///

		void 								checkAutocreateShape(const std::shared_ptr<Node>& node);
		void 								checkAutocreateShape(const std::shared_ptr<Mesh>& mesh);

		void 								checkAddToWorld();

		/// Private Member Variables ///

		std::shared_ptr<PhysicsShape>		_shape;
		std::unique_ptr<PhysicsBodyProxy>	_proxy;
		std::weak_ptr<Node>					_node;
		// either a pointer to the world we are currently in or null.
		PhysicsWorld*						_world;
	};
}

#endif /* AVARA3D_PHYSICS_PHYSICSBODY_H */
