//
//  PhysicsShape.h
//	avara-engine
//
//  Created by Morgan Davis on 1/25/18.
//  Copyright © 2018 Morgan K Davis. All rights reserved.
//

#ifndef PhysicsShape_h
#define PhysicsShape_h


#include <memory>
#include <variant>
#include <vector>

#include "Types.h"


namespace ae {
	
	
	class Geometry;
	class Node;
	class PhysicsBody;
	

	class PhysicsShape {// : public std::enable_shared_from_this<PhysicsShape> {

/*********************************************************************************************
	Public Static
 *********************************************************************************************/

	public:

		// SCNPhysicsShape does NOT have these
//		static std::shared_ptr<PhysicsShape> 	BoundingBoxShape();
//		static std::shared_ptr<PhysicsShape> 	ConvexHullShape();
//		static std::shared_ptr<PhysicsShape> 	ConcavePolyhedronShape();
		
/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

	public:

		PhysicsShape(PHYSICS_SHAPE_TYPE type, std::shared_ptr<Geometry> geometry);
		PhysicsShape(PHYSICS_SHAPE_TYPE type, std::shared_ptr<Node> node);
		~PhysicsShape();

/*********************************************************************************************
	Public
 *********************************************************************************************/

//		std::weak_ptr<Geometry> 			sourceGeometry() const;
//		std::weak_ptr<Node> 				sourceNode() const;

//		std::weak_ptr<std::variant<Geometry, Node>> sourceObject() const;
		std::variant<std::weak_ptr<Geometry>, std::weak_ptr<Node>> sourceObject() const;

		PHYSICS_SHAPE_TYPE 					type() const;
		void 								type(PHYSICS_SHAPE_TYPE type);

/*********************************************************************************************
	Internal
 *********************************************************************************************/

		void sourceObject(std::variant<std::weak_ptr<Geometry>, std::weak_ptr<Node>> sourceObject);

//		void								sourceGeometry(std::weak_ptr<Geometry> geometry);
//		void 								sourceNode(std::weak_ptr<Node> node);

//		void 								attachedToBody(std::shared_ptr<PhysicsBody> body);

//		std::weak_ptr<PhysicsBody>			physicsBody() const;
//		void 								physicsBody(std::shared_ptr<PhysicsBody> body);

		PHYSICS_SHAPE_DIRTY_BITS 			dirtyBits() const;
		void 								dirtyBits(PHYSICS_SHAPE_DIRTY_BITS bits);

/*********************************************************************************************
	Private
 *********************************************************************************************/

	private:

//		std::weak_ptr<Geometry> 			_sourceGeometry;
//		std::weak_ptr<Node>					_sourceNode;

		std::variant<std::weak_ptr<Geometry>, std::weak_ptr<Node>> _sourceObject;


		PHYSICS_SHAPE_TYPE 					_type;
		PHYSICS_SHAPE_DIRTY_BITS 			_dirtyBits;
	};
}


#endif /* PhysicsShape_h */
