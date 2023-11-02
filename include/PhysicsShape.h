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
#include <vector>

#include "Types.h"


namespace ae {
	
	
	class Geometry;
	class Node;
	class PhysicsBody;
	

	class PhysicsShape : public std::enable_shared_from_this<PhysicsShape> {

/*********************************************************************************************
	Public Static
 *********************************************************************************************/

	public:

		static std::shared_ptr<ae::PhysicsShape> 	BoundingBoxShape();
		static std::shared_ptr<ae::PhysicsShape> 	ConvexHullShape();
		static std::shared_ptr<ae::PhysicsShape> 	ConcavePolyhedronShape();
		
/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

	public:

		PhysicsShape(PHYSICS_SHAPE_TYPE type);
		PhysicsShape(PHYSICS_SHAPE_TYPE type, std::shared_ptr<ae::Geometry> geometry);
		PhysicsShape(PHYSICS_SHAPE_TYPE type, std::shared_ptr<ae::Node> node);
		~PhysicsShape();
		
/*********************************************************************************************
	Public
 *********************************************************************************************/

		std::weak_ptr<ae::Geometry> 		sourceGeometry() const;

		std::weak_ptr<ae::Node> 			sourceNode() const;

		PHYSICS_SHAPE_TYPE 					type() const;
		void 								type(PHYSICS_SHAPE_TYPE type);

		std::vector<glm::mat4> 				transforms() const;

/*********************************************************************************************
	Internal
 *********************************************************************************************/

		void								sourceGeometry(std::weak_ptr<ae::Geometry> geometry);
		void 								sourceNode(std::weak_ptr<ae::Node> node);

		void 								attachedToBody(std::shared_ptr<ae::PhysicsBody> body);

		std::weak_ptr<ae::PhysicsBody>		physicsBody() const;
		void 								physicsBody(std::shared_ptr<ae::PhysicsBody> body);

		PHYSICS_SHAPE_DIRTY_BITS 			dirtyBits() const;
		void 								dirtyBits(PHYSICS_SHAPE_DIRTY_BITS bits);

/*********************************************************************************************
	Private
 *********************************************************************************************/

	private:

		std::weak_ptr<ae::Geometry> 		_sourceGeometry;
		std::weak_ptr<ae::Node>				_sourceNode;
		PHYSICS_SHAPE_TYPE 					_type;
		// the array of transforms that was used to create a compound shape.
		std::vector<glm::mat4> 				_transforms;

		std::weak_ptr<PhysicsBody>			_physicsBody;
		
		PHYSICS_SHAPE_DIRTY_BITS 			_dirtyBits;
	};
}


#endif /* PhysicsShape_h */
