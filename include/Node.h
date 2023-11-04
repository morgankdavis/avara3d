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
#include <stack>
#include <string>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include "Types.h"


namespace ae {


	class Camera;
	class Geometry;
	class Light;
	class PhysicsBody;
	class Scene;

	
	class Node : public std::enable_shared_from_this<Node> {
		
/*********************************************************************************************
	Public Static
 *********************************************************************************************/

	public:

		static std::shared_ptr<ae::Node> 		GeometryNode(std::shared_ptr<ae::Geometry> geometry);
		static std::shared_ptr<ae::Node> 		LightNode(std::shared_ptr<ae::Light> light);
		static std::shared_ptr<ae::Node> 		CameraNode(std::shared_ptr<ae::Camera> camera);

/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/
		
		Node();
		Node(const std::string& name);
		~Node();

/*********************************************************************************************
	Public
 *********************************************************************************************/

		std::optional<std::string> 				name() const;
		void 									name(const std::string& name);

		std::shared_ptr<ae::Light> 				light() const;
		void 									light(const std::shared_ptr<ae::Light> light);

		std::shared_ptr<ae::Camera> 			camera() const;
		void 									camera(const std::shared_ptr<ae::Camera> camera);

		std::shared_ptr<ae::Geometry> 			geometry() const;
		void 									geometry(const std::shared_ptr<ae::Geometry> geometry);

		bool 									hidden() const;
		void 									hidden(const bool hidden);

		float 									opacity() const;
		void				 					opacity(const float opacity);

		/* REMOVE? */ int 						renderOrder() const;
		/* REMOVE? */ void 						renderOrder(const int renderOrder);

		/* REMOVE? */ bool 						castsShadow() const;
		/* REMOVE? */ void			 			castsShadow(const bool castsShadow);

		glm::vec3 								position() const;
		void 									position(const glm::vec3 position);

		glm::vec4 								rotation() const; // axis-angle
		void 									rotation(const glm::vec4 rotation);

		glm::vec3 								eulerAngles() const; // pitch, yaw, roll
		void 									eulerAngles(const glm::vec3 eulerAngles);

		glm::quat 								orientation() const; // angle == 1st component
		void 									orientation(const glm::quat orientation);

		glm::vec3 								scale() const;
		void 									scale(const glm::vec3 scale);

		glm::mat4 								transform() const;
		void 									transform(const glm::mat4 transform);

		glm::vec3 								worldPosition();
		glm::vec4 								worldRotation(); // axis-angle
		glm::vec3 								worldEulerAngles(); // pitch, yaw, roll
		glm::quat 								worldOrientation(); // angle == 1st component
		glm::vec3 								worldScale();

		glm::vec3 								worldForward();
		glm::vec3 								worldUp();
		glm::vec3 								worldRight();

		glm::mat4 								worldTransform();

		void 									addChildren(std::vector<std::shared_ptr<ae::Node>> nodes);
		void 									addChild(std::shared_ptr<ae::Node> node);
		void 									insertChild(const Node& node, int index);
		void 									removeFromParent();
		void 									replaceChild(const Node& replace, const Node& with);

		std::weak_ptr<Node> 					parent() const;
		std::vector<std::shared_ptr<Node>> 		children(bool resursive);
		std::shared_ptr<Node> 					child(const std::string& name, bool resursive);

		std::shared_ptr<PhysicsBody> 			physicsBody() const;
		void 									physicsBody(std::shared_ptr<ae::PhysicsBody> body);

//		glm::vec3 								convertPositionFromNode(const glm::vec3& position, const Node& fromNode);
//		glm::vec3 								convertPositionToNode(const glm::vec3& position, const Node& toNode);
//		glm::mat4 								convertTransformFromNode(const glm::mat4& transform, const Node& fromNode);
//		glm::mat4 								convertTransformToNode(const glm::mat4& transform, const Node& toNode);

/*********************************************************************************************
	Internal
 *********************************************************************************************/
		
		void	 								unrollWorldTransform(glm::mat4 transform);
		
		void 									updateWorldTransform();
		bool 									containsChild(std::shared_ptr<ae::Node> node);
//		void 									attachedToScene(std::shared_ptr<ae::Scene> scene);
		void 									attachedToParent(std::shared_ptr<ae::Node> parentNode);
		
//		std::shared_ptr<ae::Node> 				root() const;
//		std::weak_ptr<ae::Scene> 				scene() const;

//		std::weak_ptr<ae::Node> 				model() const;
//		void 									attachedToModel(std::shared_ptr<ae::Node> model);

/*********************************************************************************************
	Private
 *********************************************************************************************/

	private:

		std::vector<std::shared_ptr<ae::Node>> 	pathToRoot() const;
		void			 						addDirtyBitsRecursive(NODE_DIRTY_BITS bits);
		std::vector<std::shared_ptr<ae::Node>> 	topologicalChildren(std::shared_ptr<ae::Node> top);
		void 									topologicalChildrenRec(std::shared_ptr<ae::Node> node,
																	   std::map<std::shared_ptr<ae::Node>, bool>& visited,
																	   std::stack<std::shared_ptr<ae::Node>>& stack);

		//void 									checkPhysicsScale(const glm::vec3& oldScale, const glm::vec3& newScale);
		
		NODE_DIRTY_BITS 						dirtyBits() const;
		void 									dirtyBits(NODE_DIRTY_BITS bits);
		
		std::optional<std::string>				_name;
		
		std::shared_ptr<ae::Light>				_light;
		std::shared_ptr<ae::Camera>				_camera;
		std::shared_ptr<ae::Geometry>			_geometry;
		
		bool									_hidden;

		std::vector<std::shared_ptr<ae::Node>>	_children;
		
		glm::vec3								_position;
		glm::quat								_orientation;
		glm::vec3								_scale;
//		glm::mat4								_pivot;
		glm::mat4								_worldTransform;
		
		std::shared_ptr<ae::PhysicsBody>		_physicsBody;
		
//		std::weak_ptr<ae::Scene> 				_scene;
		std::weak_ptr<ae::Node>					_parent;
		
		NODE_DIRTY_BITS							_dirtyBits;
	};
}


#endif /* Node_h */
