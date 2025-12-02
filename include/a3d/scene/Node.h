//
//  Node.h
//  avara3d
//
//  Created by Morgan Davis on 10/20/16.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_NODE_H
#define AVARA3D_NODE_H


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
	class RenderContext;
	class Scene;
	class PhysicalWorld;
	class PhysicsBody;
	class VisualWorld;

	
	class Node : public std::enable_shared_from_this<Node> {

/*********************************************************************************************
	Public Static Member Functions
 *********************************************************************************************/

	public:

		static std::shared_ptr<Node> 		NamedNode(const std::string& name);
		static std::shared_ptr<Node> 		MeshNode(const std::shared_ptr<Mesh>& geometry);
		static std::shared_ptr<Node> 		LightNode(const std::shared_ptr<Light>& light);
		static std::shared_ptr<Node> 		CameraNode(const std::shared_ptr<Camera>& camera);

/************************************4*********************************************************
	Public Lifecycle Functions
 *********************************************************************************************/

		Node();
		explicit Node(const std::string& name);
		explicit Node(const std::shared_ptr<Mesh>& mesh);
		explicit Node(const std::shared_ptr<Light>& light);
		explicit Node(const std::shared_ptr<Camera>& camera);
//		/*testing*/ Node(const Node& other) = delete; // copy
//		/*testing*/ Node& operator=(const Node& other) = delete; // copy assignment
//		/*testing*/ Node(Node&& other) noexcept = delete; // move
//		/*testing*/ Node& operator=(Node&& other) noexcept = delete; // move assignment
		~Node();

/*********************************************************************************************
	Public Member Functions
 *********************************************************************************************/

		const std::optional<std::string>&	name() const;
		void 								name(const std::string& name);

		const std::shared_ptr<Light>& 		light() const;
		void 								light(const std::shared_ptr<Light>& light);

		const std::shared_ptr<Camera>& 		camera() const;
		void 								camera(const std::shared_ptr<Camera>& camera);

		const std::shared_ptr<Mesh>& 		mesh() const;
		void 								mesh(const std::shared_ptr<Mesh>& mesh);

		glm::vec3 							position() const;
		void 								position(const glm::vec3& position);

		glm::vec4 							rotation() const;
		void 								rotation(const glm::vec3& axis, float angle);

		glm::vec3 							eulerAngles() const; // pitch, yaw, roll
		void 								eulerAngles(const glm::vec3& eulerAngles);

		glm::quat 							orientation() const; // angle == 1st component
		void 								orientation(const glm::quat& orientation);

		glm::vec3 							scale() const;
		void 								scale(const glm::vec3& scale);

		glm::vec3 							forward() const;
		glm::vec3 							up() const;
		glm::vec3 							right() const;

		glm::mat4 							transform() const;
		void 								transform(const glm::mat4& transform);

		glm::vec3 							worldPosition() const;
		glm::vec4 							worldRotation() const; // axis-angle
		glm::vec3 							worldEulerAngles() const; // pitch, yaw, roll
		glm::quat 							worldOrientation() const; // angle == 1st component
		glm::vec3 							worldScale() const;

		glm::vec3 							worldForward() const;
		glm::vec3 							worldUp() const;
		glm::vec3 							worldRight() const;

		glm::mat4 							worldTransform() const;

		void 								addChild(const std::shared_ptr<Node>& node);
		void 								addChildren(const std::vector<std::shared_ptr<Node>>& nodes);
		void 								removeFromParent();

		std::vector<std::shared_ptr<Node>>	children(bool resursive = false) const;
		std::shared_ptr<Node> 				childNamed(const std::string& name, bool resursive = false) const;

		PhysicsBody* 						physicsBody() const;
		void 								physicsBody(std::unique_ptr<PhysicsBody> body);

		bool 								hidden() const;
		void 								hidden(bool hidden);

		Scene*								scene() const;

		std::weak_ptr<Node>					parent() const;

//		glm::vec3 							convertFrom(const glm::vec3& pos, const Node& from);
//		glm::vec3 							convertTo(const glm::vec3& pos, const Node& to);
//		glm::mat4 							convertFrom(const glm::mat4& t, const Node& from);
//		glm::mat4 							convertTo(const glm::mat4& t, const Node& to);

/*********************************************************************************************
	Internal Member Functions
 *********************************************************************************************/

		void 								attachedToParent(Node& parent);
		void 								detachedFromParent(Node& parent);

		void								attachedToScene(Scene& scene);
		void								detachedFromScene(Scene& scene);

		void 								ancestorAttachedToParent(Node& ancestor, Node& parent);
		void 								ancestorDetachedFromParent(Node& ancestor, Node& parent);

		void								ancestorAttachedToScene(Node& ancestor, Scene& scene);
		void								ancestorDetachedFromScene(Node& ancestor, Scene& scene);

		void								visualWorldAttachedToScene(VisualWorld& world, Scene& scene);
		void								visualWorldDetachedFromScene(VisualWorld& world, Scene& scene);

		void								physicalWorldAttachedToScene(PhysicalWorld& world, Scene& scene);
		void								physicalWorldDetachedFromScene(PhysicalWorld& world, Scene& scene);

		VisualWorld*						visualWorld() const;
		PhysicalWorld*						physicalWorld() const;

		void								checkNotifyPhysicsBodyOfReachablePhysicalWorld() const;
		void								checkNotifyPhysicsBodyOfUnreachablePhysicalWorld() const;

		bool 								containsChild(const std::shared_ptr<Node>& node);

		AABB								aabb();
		glm::vec3							extent();

		void	 							applyPhysicsTransform(const glm::mat4& transform);

		void 								draw(Renderer& renderer,
												 const RenderContext& context,
												 const glm::mat4& viewMat,
												 const glm::mat4& projectionMat,
												 const DebugOptions& debugOptions,
												 std::vector<Node*>& lightNodes,
												 Stats& stats);

		void								_debugPrint();
		void								_debugPrintRec(Node& node,
															   unsigned level);

/*********************************************************************************************
	Private Member Functions
 *********************************************************************************************/

	private:

		void								getAABBRec(AABB& aabb);

		std::vector<std::shared_ptr<Node>>	children(const Node& root) const;
		void 								childrenRec(const std::shared_ptr<Node>& node,
														std::vector<std::shared_ptr<Node>>& children) const;

		NodeDirtyMask 						dirtyMask() const;
		void 								dirtyMask(NodeDirtyMask mask);

/*********************************************************************************************
	Private Member Variables
 *********************************************************************************************/

		std::optional<std::string>			_name;
		std::shared_ptr<Light>				_light;
		std::shared_ptr<Camera>				_camera;
		std::shared_ptr<Mesh>				_mesh;
		std::vector<std::shared_ptr<Node>>	_children;
		glm::vec3							_position;
		glm::quat							_orientation;
		glm::vec3							_scale;
		std::unique_ptr<PhysicsBody>		_physicsBody;
		bool								_hidden;
		Scene*								_scene;
		std::weak_ptr<Node>					_parent;
		NodeDirtyMask						_dirtyMask;
	};
}


#endif /* AVARA3D_NODE_H */
