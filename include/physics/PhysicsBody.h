//
//  PhysicsBody.h
//	avara-engine
//
//  Created by Morgan Davis on 1/26/18.
//  Copyright © 2018 Morgan K Davis. All rights reserved.
//

#ifndef PhysicsBody_h
#define PhysicsBody_h


#include <memory>

#include "glm/glm.hpp"

#include "Types.h"


namespace ae {
	

	class Geometry;
	class Node;
	class PhysicalWorld;
	class PhysicsBodyModelProxy;
	class PhysicsShape;
	class Scene;
	
	
	class PhysicsBody {
		
/*********************************************************************************************
	Public Static
 *********************************************************************************************/

	public:

		static std::shared_ptr<PhysicsBody> 	StaticBody();
		static std::shared_ptr<PhysicsBody> 	DynamicBody();
		static std::shared_ptr<PhysicsBody> 	KinematicBody();
		
/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/
		
		//PhysicsBody();
		// "If you pass nil for the shape parameter, SceneKit automatically creates a physics
		// shape for the body when you attach it to a node, based on that node’s geometry property."
		PhysicsBody(PHYSICS_BODY_TYPE type);
		PhysicsBody(PHYSICS_BODY_TYPE type, std::shared_ptr<PhysicsShape> shape);
		~PhysicsBody();
		
/*********************************************************************************************
	Public
 *********************************************************************************************/
		
		PHYSICS_BODY_TYPE 					type() const;
		void 								type(PHYSICS_BODY_TYPE type);

		std::shared_ptr<PhysicsShape> 		shape() const;
		void 								shape(std::shared_ptr<PhysicsShape> shape);

		float 								mass() const;
		void 								mass(float mass);
		
		glm::vec3 							momentOfInertia() const;
		void 								momentOfInertia(glm::vec3 moment);

		glm::vec3							centerOfMass() const;
		void								centerOfMass(const glm::vec3 offset);

		float 								friction() const;
		void 								friction(float friction);
		
		float 								rollingFriction() const;
		void 								rollingFriction(float friction);

		float 								restitution() const;
		void 								restitution(float restitution);

		glm::vec3 							linearVelocity() const;
		void 								linearVelocity(glm::vec3 velocity);

		glm::vec3 							angularVelocity() const;
		void 								angularVelocity(glm::vec3 velocity);

		glm::vec3 							linearFactor() const;
		void 								linearFactor(glm::vec3 factor);

		glm::vec3 							angularFactor() const;
		void 								angularFactor(glm::vec3 factor);

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

		void 								applyForce(glm::vec3 force,
													   bool impulse);
		void 								applyForce(glm::vec3 force,
													   glm::vec3 location,
													   bool impulse);
		void 								applyTorque(glm::vec3 torque,
														bool impulse);

		glm::vec3 							totalForce() const;
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

/*********************************************************************************************
	Internal
 *********************************************************************************************/
		
//		void 								resting(bool resting);
		
		void 								attachedToNode(Node* node);
		void 								detachedFromNode(Node* node);

//		void 								nodeAttachedToParent(Node* parent); // owning node's parent
//		void 								nodeDetachedFromParent(Node* parent);
//
//		void 								nodeAttachedToScene(Scene* scene); // owning node
//		void 								nodeDetachedFromScene(Scene* scene);

		void 								geometryAttachedToNode(Geometry* geometry); // owning node's geometry
		void 								geometryDetachedFromNode(Geometry* geometry);

		void								physicalWorldReachable(PhysicalWorld* world);
		void								physicalWorldUnreachable(PhysicalWorld* world);

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

		void 								addedToWorld(PhysicalWorld* world);
		void 								removedFromWorld(PhysicalWorld* world);

		Node*								node() const;

		// the scene's world, if it exists.  not the same as _world.
		PhysicalWorld*						physicalWorld() const;

		PhysicsBodyModelProxy*				proxy() const;

/*********************************************************************************************
	Private
 *********************************************************************************************/

	private:

		void 									checkAutocreateShape(Node* node);
		void 									checkAutocreateShape(Geometry* geometry);

		void 									checkAddToWorld();

		PHYSICS_BODY_TYPE 						_type;
		std::shared_ptr<PhysicsShape>			_shape;
		std::unique_ptr<PhysicsBodyModelProxy>	_proxy;
		Node*									_node;
		// either a pointer to the world we are currently in or null.
		PhysicalWorld*							_world;
	};
}


#endif /* PhysicsBody_h */
