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
	class PhysicalWorld;
	class PhysicsShapeResources;
	class PhysicsSimulator;
	class Scene;
	

	class PhysicsShape {

/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

	public:

		PhysicsShape();
		PhysicsShape(PHYSICS_SHAPE_TYPE type);
		PhysicsShape(PHYSICS_SHAPE_TYPE type, Geometry* geometry);
		PhysicsShape(PHYSICS_SHAPE_TYPE type, Node* node);
		~PhysicsShape();

/*********************************************************************************************
	Public
 *********************************************************************************************/

		virtual PHYSICS_SHAPE_TYPE 			type() const;
		virtual void 						type(PHYSICS_SHAPE_TYPE type);

		std::variant<
				Geometry*,
				Node*,
				std::monostate> 			sourceObject() const;

/*********************************************************************************************
	Internal
 *********************************************************************************************/

		void 								attachedToBody(PhysicsBody* body);
		void 								detachedFromBody(PhysicsBody* body);

		void								physicalWorldReachable(PhysicalWorld* world);
		void								physicalWorldUnreachable(PhysicalWorld* world);

//		void 								bodyAttachedToNode(Node* node); // body's owning node
//		void 								bodyDetachedFromNode(Node* node);
//
//		void 								ancestorAttachedToParent(Node* ancestor,
//																	  Node* parent);
//		void 								ancestorDetachedFromParent(Node* node,
//																		Node* parent);
//
//		void								ancestorAttachedToScene(Node* ancestor,
//																	Scene* scene);
//		void								ancestorDetachedFromScene(Node* node,
//																	  Scene* scene);
//
//		void								physicalWorldAttachedToScene(PhysicalWorld* world,
//																		 Scene* scene);
//		void								physicalWorldDetachedFromScene(PhysicalWorld* world,
//																		   Scene* scene);

		void 								sourceObject(
				std::variant<Geometry*, Node*, std::monostate> sourceObject);

//		PhysicalWorld*						physicalWorld() const;

		void								update(PhysicsSimulator& simulator,
												   Node& node,
												   PhysicsBody& body,
												   Stats& stats);
		void								sync(PhysicsSimulator& simulator,
												 Node& node,
												 PhysicsBody& body,
												 Stats& stats);

		PHYSICS_SHAPE_DIRTY_MASK 			dirtyMask() const;
		void 								dirtyMask(PHYSICS_SHAPE_DIRTY_MASK mask);

		std::shared_ptr<PhysicsShapeResources>	resources();

	protected:

		PHYSICS_SHAPE_TYPE 					_type;

/*********************************************************************************************
	Private
 *********************************************************************************************/

	private:

		std::variant<
				Geometry*,
				Node*,
				std::monostate> 			_sourceObject;
		std::shared_ptr<PhysicsShapeResources>	_resources;
		PHYSICS_SHAPE_DIRTY_MASK 			_dirtyMask;
	};
}


#endif /* PhysicsShape_h */
