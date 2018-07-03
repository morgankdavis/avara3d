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
#include <stack>
#include <string>
#include <vector>

#include <boost/optional.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include "Types.h"


namespace ae {


	class Camera;
	class Geometry;
	class Light;
	class PhysicsBody;
	class PresentationNode;
	class Scene;

	
	class Node : public std::enable_shared_from_this<Node> {
		
	public:
		
		/***************************************************************************************
		     Public Static
		 ***************************************************************************************/

		static std::shared_ptr<Node> GeometryNode(std::shared_ptr<Geometry> geometry);
		static std::shared_ptr<Node> LightNode(std::shared_ptr<Light> light);
		static std::shared_ptr<Node> CameraNode(std::shared_ptr<Camera> camera);

		/***************************************************************************************
	    	 Lifecycle
	 	 ***************************************************************************************/
		
		Node();
		Node(const std::string& name);
		~Node();

		/***************************************************************************************
     		Public
 		 ***************************************************************************************/

		boost::optional<std::string> name() const;
		void name(const std::string& name);
		
		std::shared_ptr<Light> light() const;
		void light(const std::shared_ptr<Light> light);
		
		std::shared_ptr<Camera> camera() const;
		void camera(const std::shared_ptr<Camera> camera);
		
		std::shared_ptr<Geometry> geometry() const;
		void geometry(const std::shared_ptr<Geometry> geometry);

		bool hidden() const;
		void hidden(const bool hidden);
		
		float opacity() const;
		void opacity(const float opacity);
		
		/* REMOVE? */ int renderOrder() const;
		/* REMOVE? */ void renderOrder(const int renderOrder);
		
		/* REMOVE? */ bool castsShadow() const;
		/* REMOVE? */ void castsShadow(const bool castsShadow);
		
		virtual glm::vec3 position() const;
		void position(const glm::vec3 position);

		virtual glm::vec4 rotation() const; // axis-angle
		void rotation(const glm::vec4 rotation);
		
		virtual glm::vec3 eulerAngles() const; // pitch, yaw, roll
		void eulerAngles(const glm::vec3 eulerAngles);
		
		virtual glm::quat orientation() const; // angle == 1st component
		void orientation(const glm::quat orientation);
		
		virtual glm::vec3 scale() const;
		void scale(const glm::vec3 scale);
		
		virtual glm::mat4 transform() const;
		virtual void transform(const glm::mat4 transform);
		
		glm::vec3 worldPosition();
		glm::vec4 worldRotation(); // axis-angle
		glm::vec3 worldEulerAngles(); // pitch, yaw, roll
		glm::quat worldOrientation(); // angle == 1st component
		glm::vec3 worldScale();
		
		glm::vec3 worldForward();
		glm::vec3 worldUp();
		glm::vec3 worldRight();
		
		virtual glm::mat4 worldTransform();
		
		void addChildren(std::vector<std::shared_ptr<Node>> nodes);
		void addChild(std::shared_ptr<Node> node);
		void insertChild(const Node& node, int index);
		void removeFromParent();
		void replaceChild(const Node& replace, const Node& with);
		
		std::weak_ptr<Node> parent() const;
		std::vector<std::shared_ptr<Node>> children(bool resursive);
		std::shared_ptr<Node> child(const std::string& name, bool resursive);
		
		std::shared_ptr<PhysicsBody> physicsBody() const;
		void physicsBody(std::shared_ptr<PhysicsBody> body);

//		glm::vec3 convertPositionFromNode(const glm::vec3& position, const Node& fromNode);
//		glm::vec3 convertPositionToNode(const glm::vec3& position, const Node& toNode);
//		glm::mat4 convertTransformFromNode(const glm::mat4& transform, const Node& fromNode);
//		glm::mat4 convertTransformToNode(const glm::mat4& transform, const Node& toNode);
		
		std::shared_ptr<PresentationNode> presentation();

		/***************************************************************************************
     		Internal
 		 ***************************************************************************************/
		
#warning TEMPORARY before physics unroll
		virtual void worldTransform(glm::mat4 transform);
		
		void updateWorldTransform();
		bool containsChild(std::shared_ptr<Node> node);
		void attachedToScene(std::shared_ptr<Scene> scene);
		void attachedToParent(std::shared_ptr<Node> parentNode);
		
		std::shared_ptr<Node> root() const;
		std::weak_ptr<Scene> scene() const;

//		std::weak_ptr<Node> model() const;
//		void attachedToModel(std::shared_ptr<Node> model);
		
	private:

		/***************************************************************************************
     		Private
 		 ***************************************************************************************/
		
		std::vector<std::shared_ptr<Node>> pathToRoot() const;
		void addDirtyBitsRecursive(NODE_DIRTY_BITS bits);
		std::vector<std::shared_ptr<Node>> topologicalChildren(std::shared_ptr<Node> top);
		void topologicalChildrenRec(std::shared_ptr<Node> node,
									std::map<std::shared_ptr<Node>, bool>& visited,
									std::stack<std::shared_ptr<Node>>& stack);
		
		NODE_DIRTY_BITS dirtyBits() const;
		void dirtyBits(NODE_DIRTY_BITS bits);
		
		boost::optional<std::string>		m_name;
		
		std::shared_ptr<Light>				m_light;
		std::shared_ptr<Camera>				m_camera;
		std::shared_ptr<Geometry>			m_geometry;
		
		bool								m_hidden;

		std::vector<std::shared_ptr<Node>>	m_children;
		
		glm::vec3							m_position;
		glm::quat							m_orientation;
		glm::vec3							m_scale;
//		glm::mat4							m_pivot;
		glm::mat4							m_worldTransform;
		
		std::shared_ptr<PhysicsBody>		m_physicsBody;
		
		std::weak_ptr<Scene> 				m_scene;
		std::weak_ptr<Node>					m_parent;
		
		NODE_DIRTY_BITS						m_dirtyBits;
		
		std::shared_ptr<PresentationNode>	m_presentation;
//		std::weak_ptr<Node> 				m_model;
	};
}


#endif /* Node_h */
