//
//  Node.h
//	avara3d
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

#include "a3d/Types.h"


namespace a3d {


	class Camera;
	class Light;
	class Mesh;
	class Renderer;
	class Scene;
	class PhysicalWorld;
	class PhysicsBody;
	class VisualWorld;

	
	class Node {// : public std::enable_shared_from_this<Node> {
		
/*********************************************************************************************
	Public Static
 *********************************************************************************************/

	public:

		static std::shared_ptr<Node> 			NamedNode(std::string name);
		static std::shared_ptr<Node> 			MeshNode(std::shared_ptr<Mesh> geometry);
		static std::shared_ptr<Node> 			LightNode(std::shared_ptr<Light> light);
		static std::shared_ptr<Node> 			CameraNode(std::shared_ptr<Camera> camera);

/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/
		
		Node();
		Node(const std::string& name);
		Node(std::shared_ptr<Mesh> mesh);
		Node(std::shared_ptr<Light> light);
		Node(std::shared_ptr<Camera> camera);
		/*testing*/ Node(const Node& other) = delete; // copy constructor
		/*testing*/ Node& operator=(const Node& other) = delete; // copy assignment
		~Node();

/*********************************************************************************************
	Public
 *********************************************************************************************/

		std::optional<std::string> 				name() const;
		void 									name(const std::string& name);

		std::shared_ptr<Light> 					light() const;
		void 									light(std::shared_ptr<Light> light);

		std::shared_ptr<Camera> 				camera() const;
		void 									camera(std::shared_ptr<Camera> camera);

		std::shared_ptr<Mesh> 					mesh() const;
		void 									mesh(std::shared_ptr<Mesh> mesh);
		void 									mesh(std::unique_ptr<Mesh> mesh);

		bool 									hidden() const;
		void 									hidden(bool hidden);

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
		void 									removeFromParent();

		std::vector<Node*>						children(bool resursive = false);
		Node* 									childNamed(const std::string& name, bool resursive = false);

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

//		void									meshAttachedToNode(Mesh* mesh, Node* node);
//		void 									meshDetachedFromNode(MEsh* mesh, Node* node);

		void									visualWorldAttachedToScene(VisualWorld* world, Scene* scene);
		void									visualWorldDetachedFromScene(VisualWorld* world, Scene* scene);

		void									physicalWorldAttachedToScene(PhysicalWorld* world, Scene* scene);
		void									physicalWorldDetachedFromScene(PhysicalWorld* world, Scene* scene);

		VisualWorld*							visualWorld() const;
		PhysicalWorld*							physicalWorld() const;

		void									checkNotifyPhysicsBodyOfReachablePhysicalWorld() const;
		void									checkNotifyPhysicsBodyOfUnreachablePhysicalWorld() const;

		bool 									containsChild(Node* node);

		AABB									aabb();
		glm::vec3								extent();

		void 									draw(Renderer& renderer,
													 const glm::mat4& viewMat,
													 const glm::mat4& projectionMat,
													 const DebugOptions& debugOptions,
													 Stats& stats);

		void									_debugPrint();
		void									_debugPrintRec(Node& node,
															   int level);

		void	 								applyPhysicsTransform(glm::mat4 transform);

/*********************************************************************************************
	Private
 *********************************************************************************************/

	private:

		void									getAABBRec(AABB& aabb);

		std::vector<Node*>						children(Node* root);
		void 									childrenRec(Node* node,
															std::list<Node*>& list);

		NodeDirtyMask 							dirtyMask() const;
		void 									dirtyMask(NodeDirtyMask mask);

		std::optional<std::string>				_name;
		
		std::shared_ptr<Light>					_light; // TODO: make unique
		std::shared_ptr<Camera>					_camera; // TODO: make unique
		std::shared_ptr<Mesh>					_mesh;
		
		bool									_hidden;

		std::vector<std::shared_ptr<Node>>		_children;

		glm::vec3								_position;
		glm::quat								_orientation;
		glm::vec3								_scale;
		
		std::shared_ptr<PhysicsBody>			_physicsBody; // TODO: make unique

		NodeDirtyMask							_dirtyMask;

		Scene*									_scene;

		Node*									_parent;
	};
}


#endif /* Node_h */
