//
//  PhysicsShape.h
//	avara3d
//
//  Created by Morgan Davis on 1/25/18.
//  Copyright © 2018 Morgan K Davis. All rights reserved.
//

#ifndef PhysicsShape_h
#define PhysicsShape_h


#include <memory>
#include <unordered_set>
#include <variant>
#include <vector>

#include "a3d/Types.h"


namespace a3d {
	
	
	class Mesh;
	class Node;
	class PhysicsBody;
	class PhysicalWorld;
	class PhysicsShapeProxy;
	class Scene;
	

	class PhysicsShape {

/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

	public:

		using SourceObject = std::variant<
				std::monostate,
				Mesh*,
				Node*>;

		PhysicsShape(PhysicsShapeType type, Mesh* mesh);
		PhysicsShape(PhysicsShapeType type, Node* node);
		~PhysicsShape();

	protected:

		PhysicsShape();

/*********************************************************************************************
	Public
 *********************************************************************************************/

	public:

		virtual PhysicsShapeType 			type() const;
		virtual void 						type(PhysicsShapeType type);

		SourceObject 						sourceObject() const;

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

		void 								sourceObject(SourceObject sourceObject);

		void								checkCreateProxy();

		std::unordered_set<PhysicsBody*>	bodies() const;

		PhysicsShapeProxy*					proxy() const;

	protected:

		PhysicsShapeType 					_type;
		std::unique_ptr<PhysicsShapeProxy>	_proxy;

/*********************************************************************************************
	Private
 *********************************************************************************************/

	private:

		SourceObject 						_sourceObject;
		std::unordered_set<PhysicsBody*>	_bodies;
//		std::unique_ptr<PhysicsShapeModelProxy>	_proxy;
	};
}


#endif /* PhysicsShape_h */
