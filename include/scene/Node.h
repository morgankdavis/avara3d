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

		float 									opacity() const;
		void				 					opacity(const float opacity);

		/* REMOVE? */ int 						renderOrder() const;
		/* REMOVE? */ void 						renderOrder(const int renderOrder);

		/* REMOVE? */ bool 						castsShadow() const;
		/* REMOVE? */ void			 			castsShadow(const bool castsShadow);

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

		glm::vec3 								worldPosition();
		glm::vec4 								worldRotation(); // axis-angle
		glm::vec3 								worldEulerAngles(); // pitch, yaw, roll
		glm::quat 								worldOrientation(); // angle == 1st component
		glm::vec3 								worldScale();

		glm::vec3 								worldForward();
		glm::vec3 								worldUp();
		glm::vec3 								worldRight();

		glm::mat4 								worldTransform();

		void 									addChildren(std::vector<std::shared_ptr<Node>> nodes);
		void 									addChild(std::shared_ptr<Node> node);
		void 									insertChild(const Node& node, int index);
		void 									removeFromParent();
		void 									replaceChild(const Node& replace, const Node& with);

		std::weak_ptr<Node> 					parent() const;
		std::vector<std::shared_ptr<Node>>		children(bool resursive);
		std::shared_ptr<Node> 					child(const std::string& name, bool resursive);

		std::shared_ptr<PhysicsBody> 			physicsBody() const;
		void 									physicsBody(std::shared_ptr<PhysicsBody> body);

//		glm::vec3 								convertPositionFromNode(const glm::vec3& position, const Node& fromNode);
//		glm::vec3 								convertPositionToNode(const glm::vec3& position, const Node& toNode);
//		glm::mat4 								convertTransformFromNode(const glm::mat4& transform, const Node& fromNode);
//		glm::mat4 								convertTransformToNode(const glm::mat4& transform, const Node& toNode);

/*********************************************************************************************
	Internal
 *********************************************************************************************/

		//void 									updateWorldTransform();
//		void 									updateWorldTransform(glm::mat4& parentWorldTransform);
		bool 									containsChild(std::shared_ptr<Node> node);
//		void 									attachedToScene(std::shared_ptr<Scene> scene);
		void 									attachedToParent(std::weak_ptr<Node> parent);
		
//		std::shared_ptr<Node> 					root() const;
//		std::weak_ptr<Scene> 					scene() const;

//		std::weak_ptr<Node> 					model() const;
//		void 									attachedToModel(std::shared_ptr<Node> model);

		void 									update(PhysicsSimulator& simulator,
													   FrameStats& stats,
													   std::map<std::shared_ptr<Node>, bool>& visited);

		void 									sync(PhysicsSimulator& simulator,
													 FrameStats& stats,
													 std::map<std::shared_ptr<Node>, bool>& visited);

		void 									draw(Renderer& renderer,
													 const glm::mat4& viewMat,
													 const glm::mat4& projectionMat,
													 const DEBUG_OPTIONS& debugOptions,
													 FrameStats& stats,
													 std::map<std::shared_ptr<Node>, bool>& visited);

		void									_debugPrint(); // testing
		void									_debugPrintRec(Node& node,
															   int level);

//		void									_debugPrint(); // testing
//		void									_debugPrintRec(std::shared_ptr<Node> node,
//																  int level,
//																  std::map<std::shared_ptr<Node>, bool>& visited);

/*********************************************************************************************
	Private
 *********************************************************************************************/

	private:

		void	 								unrollWorldTransform(glm::mat4 transform);

//		std::vector<std::shared_ptr<Node>> 		pathToRoot() const;

//		void			 						addChildrenDirtyMask(NODE_DIRTY_MASK mask);
//		void			 						addChildrenDirtyMaskRec(NODE_DIRTY_MASK mask,
//																		std::shared_ptr<Node> node,
//																		std::list<std::shared_ptr<Node>>& list);

//		void			 						addChildrenDirtyMask(NODE_DIRTY_MASK mask);
//		void			 						addChildrenDirtyMaskRec(NODE_DIRTY_MASK mask,
//																		std::shared_ptr<Node> node,
//																		std::map<std::shared_ptr<Node>, bool>& visited,
//																		std::stack<std::shared_ptr<Node>>& stack);

		std::vector<std::shared_ptr<Node>>		children(std::shared_ptr<Node> root);
		void 									childrenRec(std::shared_ptr<Node> node,
															std::list<std::shared_ptr<Node>>& list);

//		std::vector<std::shared_ptr<Node>>		preorderChildren(std::shared_ptr<Node> root);
//		void 									preorderChildrenRec(std::shared_ptr<Node> node,
//																	std::map<std::shared_ptr<Node>, bool>& visited,
//																	std::stack<std::shared_ptr<Node>>& stack);

//		std::vector<std::shared_ptr<Node>>		preorderChildren(std::shared_ptr<Node> root);
//		void 									preorderChildrenRec(std::shared_ptr<Node> node,
//																	std::map<std::shared_ptr<Node>, bool>& visited,
//																	std::stack<std::shared_ptr<Node>>& stack);

		//void 									checkPhysicsScale(const glm::vec3& oldScale, const glm::vec3& newScale);
		
		NODE_DIRTY_MASK 						dirtyMask() const;
		void 									dirtyMask(NODE_DIRTY_MASK mask);
//
		std::optional<std::string>				_name;
		
		std::shared_ptr<Light>					_light;
		std::shared_ptr<Camera>					_camera;
		std::shared_ptr<Geometry>				_geometry;
		
		bool									_hidden;

		std::vector<std::shared_ptr<Node>>		_children;

		glm::vec3								_position;
		glm::quat								_orientation;
		glm::vec3								_scale;
//		glm::mat4								_pivot;
//		glm::mat4								_worldTransform;
		
		std::shared_ptr<PhysicsBody>			_physicsBody;
		
//		std::weak_ptr<Scene> 					_scene;
		std::weak_ptr<Node>						_parent;
		
		NODE_DIRTY_MASK							_dirtyMask;
	};
}


#endif /* Node_h */
