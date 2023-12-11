//
//  Node.h
//	avara-engine
//
//  Created by Morgan Davis on 10/20/16.
//  Copyright © 2016 Morgan K Davis. All rights reserved.
//

#ifndef Node_h
#define Node_h


#include <map>
#include <memory>
#include <optional>
#include <list>
#include <stack>
#include <string>
#include <vector>

#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"

#include "Scene.h"
#include "Types.h"


namespace ae {


	class Camera;
	class Geometry;
	class Light;
	class PhysicsBody;
	class PhysicsSimulator;

	
	class Node : public std::enable_shared_from_this<Node> {
		
/*********************************************************************************************
	Public Static
 *********************************************************************************************/

	public:

		static std::shared_ptr<Node> 			NamedNode(std::string name);
		static std::shared_ptr<Node> 			GeometryNode(std::shared_ptr<Geometry> geometry);
		static std::shared_ptr<Node> 			LightNode(std::shared_ptr<Light> light);
		static std::shared_ptr<Node> 			CameraNode(std::shared_ptr<Camera> camera);

/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/
		
		Node();
		Node(const std::string& name);
		Node(std::shared_ptr<Geometry> geometry);
		Node(std::shared_ptr<Light> light);
		Node(std::shared_ptr<Camera> camera);
//		Node(const Node& other) = delete; // copy constructor
//		Node& operator=(const Node& other) = delete; // copy assignment
		~Node();

/*********************************************************************************************
	Public
 *********************************************************************************************/

		std::optional<std::string> 				name() const;
		void 									name(const std::string& name);

		std::shared_ptr<Light> 					light() const;
		void 									light(const std::shared_ptr<Light> light);

		std::shared_ptr<Camera> 				camera() const;
		void 									camera(const std::shared_ptr<Camera> camera);

		std::shared_ptr<Geometry> 				geometry() const;
		void 									geometry(const std::shared_ptr<Geometry> geometry);

		bool 									hidden() const;
		void 									hidden(const bool hidden);

		glm::vec3 								position() const;
		void 									position(const glm::vec3& position);

		glm::vec4 								rotation() const;
		void 									rotation(const glm::vec3& axis, float angle);

		glm::vec3 								eulerAngles() const; // pitch, yaw, roll
		void 									eulerAngles(const glm::vec3& eulerAngles);

		glm::quat 								orientation() const; // angle == 1st component
		void 									orientation(const glm::quat& orientation);

		glm::vec3 								scale() const;
		void 									scale(const glm::vec3& scale);

		glm::mat4 								transform() const;
		void 									transform(const glm::mat4& transform);

		glm::vec3 								worldPosition() const;
		glm::vec4 								worldRotation() const; // axis-angle
		glm::vec3 								worldEulerAngles() const; // pitch, yaw, roll
		glm::quat 								worldOrientation() const; // angle == 1st component
		glm::vec3 								worldScale() const;

		glm::vec3 								worldForward() const;
		glm::vec3 								worldUp() const;
		glm::vec3 								worldRight() const;

		glm::mat4 								worldTransform() const;

		void 									addChildren(std::vector<std::shared_ptr<Node>> nodes);
		void 									addChild(std::shared_ptr<Node> node);
		void 									insertChild(const Node& node, int index);
		void 									removeFromParent();
		void 									replaceChild(const Node& replace, const Node& with);

		std::vector<std::shared_ptr<Node>>		children(bool resursive = false);
		std::shared_ptr<Node> 					childNamed(const std::string& name, bool resursive = false);

		std::shared_ptr<PhysicsBody> 			physicsBody() const;
		void 									physicsBody(std::shared_ptr<PhysicsBody> body);

		Scene*									scene() const;

		Node* 									parent() const;

//		glm::vec3 								convertPositionFromNode(const glm::vec3& position, const Node& fromNode);
//		glm::vec3 								convertPositionToNode(const glm::vec3& position, const Node& toNode);
//		glm::mat4 								convertTransformFromNode(const glm::mat4& transform, const Node& fromNode);
//		glm::mat4 								convertTransformToNode(const glm::mat4& transform, const Node& toNode);

/*********************************************************************************************
	Internal
 *********************************************************************************************/

		void 									attachedToParent(Node* parent);
		void 									detachedFromParent(Node* parent);

		void									attachedToScene(Scene* scene);
		void									detachedFromScene(Scene* scene);

		void 									ancestorAttachedToParent(Node* ancestor, Node* parent);
		void 									ancestorDetachedFromParent(Node* ancestor, Node* parent);

		void									ancestorAttachedToScene(Node* ancestor, Scene* scene);
		void									ancestorDetachedFromScene(Node* ancestor, Scene* scene);

//		void									geometryAttachedToNode(Geometry* geometry, Node* node);
//		void 									geometryDetachedFromNode(Geometry* geometry, Node* node);

		void									visualWorldAttachedToScene(VisualWorld* world, Scene* scene);
		void									visualWorldDetachedFromScene(VisualWorld* world, Scene* scene);

		void									physicalWorldAttachedToScene(PhysicalWorld* world, Scene* scene);
		void									physicalWorldDetachedFromScene(PhysicalWorld* world, Scene* scene);

		VisualWorld*							visualWorld() const;
		PhysicalWorld*							physicalWorld() const;

		void									checkNotifyPhysicsBodyOfReachablePhysicalWorld() const;
		void									checkNotifyPhysicsBodyOfUnreachablePhysicalWorld() const;

//		void									checkNotifyPhysicsBodyOfTransformUpdate() const;

		bool 									containsChild(std::shared_ptr<Node> node);

		AABB									aabb();
		glm::vec3								extent();

//		void 									update(PhysicsSimulator& simulator,
//													   Stats& stats);

		void 									sync(PhysicsSimulator& simulator,
													 Stats& stats);

		void 									draw(Renderer& renderer,
													 const glm::mat4& viewMat,
													 const glm::mat4& projectionMat,
													 const DEBUG_OPTIONS& debugOptions,
													 Stats& stats);

		void									_debugPrint(); // testing
		void									_debugPrintRec(Node& node,
															   int level);

		void	 								applyPhysicsTransform(glm::mat4 transform);

//		void									_debugPrint(); // testing
//		void									_debugPrintRec(std::shared_ptr<Node> node,
//																  int level,
//																  std::map<std::shared_ptr<Node>, bool>& visited);

/*********************************************************************************************
	Private
 *********************************************************************************************/

	private:

		void									getAABBRec(AABB& aabb);

		std::vector<std::shared_ptr<Node>>		children(std::shared_ptr<Node> root);
		void 									childrenRec(std::shared_ptr<Node> node,
															std::list<std::shared_ptr<Node>>& list);

//		std::vector<std::shared_ptr<Node>>		preorderChildren(std::shared_ptr<Node> root);
//		void 									preorderChildrenRec(std::shared_ptr<Node> node,
//																	std::map<std::shared_ptr<Node>, bool>& visited,
//																	std::stack<std::shared_ptr<Node>>& stack);

		NODE_DIRTY_MASK 						dirtyMask() const;
		void 									dirtyMask(NODE_DIRTY_MASK mask);

		std::optional<std::string>				_name;
		
		std::shared_ptr<Light>					_light;
		std::shared_ptr<Camera>					_camera;
		std::shared_ptr<Geometry>				_geometry;
		
		bool									_hidden;

		std::vector<std::shared_ptr<Node>>		_children;

		glm::vec3								_position;
		glm::quat								_orientation;
		glm::vec3								_scale;
		
		std::shared_ptr<PhysicsBody>			_physicsBody;

		NODE_DIRTY_MASK							_dirtyMask;

		Scene*									_scene;

		Node*									_parent;
	};
}


#endif /* Node_h */
