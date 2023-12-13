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
	class PhysicsBodyModel;
	class PhysicsShape;
	class PhysicsSimulator;
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

		float 								friction() const;
		void 								friction(float friction);
		
		float 								rollingFriction() const;
		void 								rollingFriction(float friction);

		float 								restitution() const;
		void 								restitution(float restitution);

		glm::vec3 							linearVelocity() const;
		void 								linearVelocity(glm::vec3 velocity,
															   bool setDirty=true); // move

		glm::vec3 							angularVelocity() const;
		void 								angularVelocity(glm::vec3 velocity,
																bool setDirty=true); // move

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

		bool 								affectedByGravity() const;
		void 								affectedByGravity(bool flag);

		bool 								allowsResting() const;
		void 								allowsResting(bool flag);
		
		bool 								resting() const;
		
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

		void 								clearForces();

		void 								resetTransform();

/*********************************************************************************************
	Internal
 *********************************************************************************************/
		
		void 								resting(bool resting);
		
		void 								attachedToNode(Node* node); // owning node
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

		void								modelCreated(PhysicsShape& shape);

		Node*								node() const;

		PhysicalWorld*						physicalWorld() const;
		PhysicsSimulator*					physicsSimulator() const;

		PhysicsBodyModel*				resources() const;
		void								resources(std::shared_ptr<PhysicsBodyModel> resources);

		PHYSICS_BODY_DIRTY_MASK 			dirtyMask() const;
		void 								dirtyMask(PHYSICS_BODY_DIRTY_MASK mask);

/*********************************************************************************************
	Private
 *********************************************************************************************/

	private:

		void 								checkCreateModel();
		void 								checkAutocreateShape(Node* node);
		void 								checkAutocreateShape(Geometry* geometry);
		
		PHYSICS_BODY_TYPE 					_type;
		std::shared_ptr<PhysicsShape>		_shape;
		float 								_mass;
		glm::vec3 							_momentOfInertia;
		float 								_friction;
		float 								_rollingFriction;
		float 								_restitution;
		glm::vec3 							_linearVelocity;
		glm::vec3 							_angularVelocity;
		glm::vec3 							_linearFactor;
		glm::vec3 							_angularFactor;
		float 								_linearDamping;
		float 								_angularDamping;
		float								_linearSleepingThreshold;
		float								_angularSleepingThreshold;
		bool								_affectedByGravity;
		bool 								_allowsResting;
		bool 								_resting;

		Node*								_node;

		PHYSICS_BODY_DIRTY_MASK 			_dirtyMask;

		std::shared_ptr<PhysicsBodyModel>	_resources;
	};
}


#endif /* PhysicsBody_h */
