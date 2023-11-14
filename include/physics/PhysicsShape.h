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
	class PhysicsShapeResourses;
	class PhysicsSimulator;
	

	class PhysicsShape {

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

		std::variant<std::weak_ptr<Geometry>, std::weak_ptr<Node>> 	sourceObject() const;

		PHYSICS_SHAPE_TYPE 					type() const;
		void 								type(PHYSICS_SHAPE_TYPE type);

/*********************************************************************************************
	Internal
 *********************************************************************************************/

		void sourceObject(std::variant<std::weak_ptr<Geometry>, std::weak_ptr<Node>> sourceObject);

		PHYSICS_SHAPE_DIRTY_MASK 			dirtyMask() const;
		void 								dirtyMask(PHYSICS_SHAPE_DIRTY_MASK mask);

		void								update(PhysicsSimulator& simulator,
												   RenderStats& stats);

/*********************************************************************************************
	Private
 *********************************************************************************************/

	private:

		std::variant<std::weak_ptr<Geometry>, std::weak_ptr<Node>> 	_sourceObject;

		PHYSICS_SHAPE_TYPE 						_type;
		PHYSICS_SHAPE_DIRTY_MASK 				_dirtyMask;

		std::shared_ptr<PhysicsShapeResourses>	_resources;
	};
}


#endif /* PhysicsShape_h */
