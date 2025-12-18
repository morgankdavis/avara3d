//
//  PhysicsShape.h
//  avara3d
//
//  Created by Morgan Davis on 1/25/18.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_PHYSICSSHAPE_H
#define AVARA3D_PHYSICSSHAPE_H

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

	public:
		/// Public Types ///

		using Source = std::variant<
				std::monostate,
				std::weak_ptr<Mesh>,
				std::weak_ptr<Node>>;

		// TODO: static MeshShape(), NodeShape()

		/// Public Lifecycle Functions ///

		PhysicsShape(PhysicsShapeType type, const std::shared_ptr<Mesh>& mesh);
		PhysicsShape(PhysicsShapeType type, const std::shared_ptr<Node>& node);
		~PhysicsShape();

		/// Public Member Functions ///

		virtual PhysicsShapeType 			type() const;
		virtual void 						type(PhysicsShapeType type);

		Source 								source() const;

		/// Internal Member Functions ///

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
		/// Protected Lifecycle ///

		PhysicsShape();

		/// Protected Member Functions ///

		PhysicsShapeType 					_type;
		std::unique_ptr<PhysicsShapeProxy>	_proxy;

	private:
		/// Private Member Variables ///

		Source 								_source;
		std::unordered_set<PhysicsBody*>	_bodies;
	};
}

#endif /* AVARA3D_PHYSICSSHAPE_H */
