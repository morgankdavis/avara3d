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
	class PhysicsShapeResources;
	class PhysicsSimulator;
	

	class PhysicsShape {

/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

	public:

		PhysicsShape();
		PhysicsShape(PHYSICS_SHAPE_TYPE type);
		PhysicsShape(PHYSICS_SHAPE_TYPE type, std::shared_ptr<Geometry> geometry);
		PhysicsShape(PHYSICS_SHAPE_TYPE type, std::shared_ptr<Node> node);
		~PhysicsShape();

/*********************************************************************************************
	Public
 *********************************************************************************************/

		virtual PHYSICS_SHAPE_TYPE 				type() const;
		virtual void 							type(PHYSICS_SHAPE_TYPE type);

		std::variant<
				std::weak_ptr<Geometry>,
				std::weak_ptr<Node>,
				std::monostate> 				sourceObject() const;

/*********************************************************************************************
	Internal
 *********************************************************************************************/

		void 									sourceObject(std::variant<
															std::weak_ptr<Geometry>,
															std::weak_ptr<Node>,
															std::monostate> sourceObject);

		void									update(PhysicsSimulator& simulator,
													   Node& node,
													   PhysicsBody& body,
													   FrameStats& stats);
		void									sync(PhysicsSimulator& simulator,
													 Node& node,
													 PhysicsBody& body,
													 FrameStats& stats);

		PHYSICS_SHAPE_DIRTY_MASK 				dirtyMask() const;
		void 									dirtyMask(PHYSICS_SHAPE_DIRTY_MASK mask);

		std::shared_ptr<PhysicsShapeResources>	resources();

	protected:

		PHYSICS_SHAPE_TYPE 						_type;

/*********************************************************************************************
	Private
 *********************************************************************************************/

	private:

		std::variant<
				std::weak_ptr<Geometry>,
				std::weak_ptr<Node>,
				std::monostate> 				_sourceObject;
//		PHYSICS_SHAPE_TYPE 						_type;
		std::shared_ptr<PhysicsShapeResources>	_resources;
		PHYSICS_SHAPE_DIRTY_MASK 				_dirtyMask;
	};
}


#endif /* PhysicsShape_h */
