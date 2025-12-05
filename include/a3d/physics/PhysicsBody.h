//
//  PhysicsBody.h
//  avara3d
//
//  Created by Morgan Davis on 1/26/18.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_PHYSICSBODY_H
#define AVARA3D_PHYSICSBODY_H

#include <memory>

#include "glm/glm.hpp"

#include "a3d/Types.h"

namespace a3d {

	class Mesh;
	class Node;
	class PhysicalWorld;
	class PhysicsBodyProxy;
	class PhysicsShapeProxy;
	class PhysicsShape;
	class Scene;

	class PhysicsBody {

	public:
		/// Public Static Member Functions ///

		static std::unique_ptr<PhysicsBody> 	StaticBody();
		static std::unique_ptr<PhysicsBody> 	DynamicBody();
		static std::unique_ptr<PhysicsBody> 	KinematicBody();

		/// Public Lifecycle Functions ///

		explicit PhysicsBody(PhysicsBodyType type);
		PhysicsBody(PhysicsBodyType type, const std::shared_ptr<PhysicsShape>& shape);
		~PhysicsBody();

		/// Public Member Functions ///

		PhysicsBodyType 					type() const;
		void 								type(PhysicsBodyType type);

		const std::shared_ptr<PhysicsShape>&	shape() const;
		void 									shape(const std::shared_ptr<PhysicsShape>& shape);

		float 								mass() const;
		void 								mass(float mass);
		
		glm::vec3 							momentOfInertia() const;
		void 								momentOfInertia(const glm::vec3& moment);

		glm::vec3							centerOfMass() const;
		void								centerOfMass(const glm::vec3& offset);

		float 								friction() const;
		void 								friction(float friction);
		
		float 								rollingFriction() const;
		void 								rollingFriction(float friction);

		float 								restitution() const;
		void 								restitution(float restitution);

		glm::vec3 							linearVelocity() const;
		void 								linearVelocity(glm::vec3 velocity);

		glm::vec3 							angularVelocity() const;
		void 								angularVelocity(const glm::vec3& velocity);

		glm::vec3							linearFactor() const;
		void 								linearFactor(const glm::vec3& factor);

		glm::vec3							angularFactor() const;
		void 								angularFactor(const glm::vec3& factor);

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

		void 								applyForce(const glm::vec3& force,
													   bool impulse);
		void 								applyForce(const glm::vec3& force,
													   const glm::vec3& location,
													   bool impulse);
		void 								applyTorque(const glm::vec3& torque,
														bool impulse);

		glm::vec3							totalForce() const;
		glm::vec3 							totalTorque() const;

		bool 								affectedByGravity() const;
		void 								affectedByGravity(bool affectedByGravity);

		bool 								allowsResting() const;
		void 								allowsResting(bool allowsResting);

		bool 								resting() const;
		void								resting(bool resting);

		void 								clearForces();

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

		void								physicalWorldReachable(PhysicalWorld& world);
		void								physicalWorldUnreachable(PhysicalWorld& world);

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
//		void								physicalWorldAttachedToScene(PhysicalWorld* world,
//																		 Scene* scene);
//		void								physicalWorldDetachedFromScene(PhysicalWorld* world,
//																		   Scene* scene);

		void 								addedToWorld(PhysicalWorld& world);
		void 								removedFromWorld(PhysicalWorld& world);

		void 								shapeUpdated();

		std::weak_ptr<Node>					node() const;

		// the scene's world, if it exists.  not the same as _world.
		PhysicalWorld*						physicalWorld() const;

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
		PhysicalWorld*						_world;
	};
}

#endif /* AVARA3D_PHYSICSBODY_H */
