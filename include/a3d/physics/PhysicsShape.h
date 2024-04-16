//
//  PhysicsShape.h
//	avara3d
//
//  Created by Morgan Davis on 1/25/18.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
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

		using Source = std::variant<
				std::monostate,
				std::weak_ptr<Mesh>,
				std::weak_ptr<Node>>;

		PhysicsShape(PhysicsShapeType type, const std::shared_ptr<Mesh>& mesh);
		PhysicsShape(PhysicsShapeType type, const std::shared_ptr<Node>& node);
		~PhysicsShape();

	protected:

		PhysicsShape();

/*********************************************************************************************
	Public
 *********************************************************************************************/

	public:

		virtual PhysicsShapeType 			type() const;
		virtual void 						type(PhysicsShapeType type);

		Source 								source() const;

/*********************************************************************************************
	Internal
 *********************************************************************************************/

		void 								attachedToBody(PhysicsBody& body);
		void 								detachedFromBody(PhysicsBody& body);

		void								physicalWorldReachable(PhysicalWorld& world);
		void								physicalWorldUnreachable(PhysicalWorld& world);

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

		void 								source(const Source& sourceObject);

		void								checkCreateProxy();

		const std::unordered_set<PhysicsBody*>&	bodies() const;

		PhysicsShapeProxy*					proxy() const;

	protected:

		PhysicsShapeType 					_type;
		std::unique_ptr<PhysicsShapeProxy>	_proxy;

/*********************************************************************************************
	Private
 *********************************************************************************************/

	private:

		Source 								_source;
		std::unordered_set<PhysicsBody*>	_bodies;
	};
}


#endif /* PhysicsShape_h */
