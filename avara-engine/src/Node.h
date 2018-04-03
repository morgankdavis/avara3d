//
//  Node.h
//	avara-engine
//
//  Created by Morgan Davis on 10/20/16.
//  Copyright © 2016 Morgan K Davis. All rights reserved.
//

#ifndef Node_h
#define Node_h

#include <memory>
#include <string>
#include <vector>

#include <boost/optional.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>


namespace ae {


	class Camera;
	class Geometry;
	class Light;
	class PhysicsBody;
	class Scene;
	
	
	//class DummyInit;


	class Node : public std::enable_shared_from_this<Node> {
		
	public:
		
		/***************************************************************************************
		     MARK:   Static
		 **************************************************************************************/
		
		//static std::shared_ptr<Node> Create();
		
		static std::shared_ptr<Node> GeometryNode(std::shared_ptr<Geometry> geometry);
		static std::shared_ptr<Node> LightNode(std::shared_ptr<Light> light);
		static std::shared_ptr<Node> CameraNode(std::shared_ptr<Camera> camera);

		/***************************************************************************************
	    	 MARK:   Lifecycle
	 	 **************************************************************************************/
		
		Node();
		Node(const std::string& name);
//		Node(const std::shared_ptr<Geometry> geometry);
//		Node(const std::shared_ptr<Light> light);
//		Node(const std::shared_ptr<Camera> camera);
//		Node(const std::string& name, const glm::mat4 transform);
//		Node(const std::string& name, const glm::mat4 transform, std::shared_ptr<Geometry> geometry);
		
		
//		Node(DummyInit& dummy);

		/***************************************************************************************
     		MARK:   Public
 		 **************************************************************************************/

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
		
		int renderOrder() const;
		void renderOrder(const int renderOrder);
		
		bool castsShadow() const;
		void castsShadow(const bool castsShadow);
		
		glm::vec3 position() const;
		void position(const glm::vec3 position);

		glm::vec4 rotation() const; // angle == 4th component
		void rotation(const glm::vec4 rotation);
		
		glm::vec3 eulerAngles() const; // pitch, yaw, roll
		void eulerAngles(const glm::vec3 eulerAngles);
		
		glm::quat orientation() const; // angle == 1st component
		void orientation(const glm::quat orientation);
		
		glm::vec3 scale() const;
		void scale(const glm::vec3 scale);
		
		glm::mat4 transform() const;
		void transform(const glm::mat4 transform);
		
		glm::vec3 worldPosition() const;
		glm::vec4 worldRotation() const; // angle == 4th component
		glm::vec3 worldEulerAngles() const; // pitch, yaw, roll
		glm::quat worldOrientation() const; // angle == 1st component
		glm::vec3 worldScale() const;
		
		glm::vec3 worldForward() const;
		glm::vec3 worldUp() const;
		glm::vec3 worldRight() const;
		
		glm::mat4 worldTransform() const;
		
		void addChildNodes(std::vector<std::shared_ptr<Node>> nodes);
		void addChildNode(std::shared_ptr<Node> node);
		void insertChildNode(const Node& node, int index);
		void removeFromParentNode();
		void replaceChildNode(const Node& replace, const Node& with);
		
		std::weak_ptr<Node> parent() const;
		std::vector<std::shared_ptr<Node>> childNodes(bool resursive);
		std::shared_ptr<Node> childNode(const std::string& name, bool resursive);
		
		std::shared_ptr<PhysicsBody> physicsBody() const;
		void physicsBody(std::shared_ptr<PhysicsBody> body);

//		glm::vec3 convertPositionFromNode(const glm::vec3& position, const Node& fromNode);
//		glm::vec3 convertPositionToNode(const glm::vec3& position, const Node& toNode);
//		glm::mat4 convertTransformFromNode(const glm::mat4& transform, const Node& fromNode);
//		glm::mat4 convertTransformToNode(const glm::mat4& transform, const Node& toNode);

		/***************************************************************************************
     		MARK:   Internal
 		 **************************************************************************************/
		
		std::shared_ptr<Node> root() const;
		
		std::vector<std::shared_ptr<Node>> pathToRoot() const;
		
		bool treeContainsNode(std::shared_ptr<Node> node);
		
		std::weak_ptr<Scene> scene() const;
		//void scene(std::shared_ptr<Scene> scene);
		
		void attachedToScene(std::shared_ptr<Scene> scene);
		
		void attachedToParentNode(std::shared_ptr<Node> parentNode);
		
		//void parent(std::shared_ptr<Node> parent);
		
	private:

		/***************************************************************************************
     		MARK:   Private
 		 **************************************************************************************/
		
		std::vector<std::shared_ptr<Node>>	childNodesRec();
		
		boost::optional<std::string>		m_name;
		
		std::shared_ptr<Light>				m_light;
		std::shared_ptr<Camera>				m_camera;
		std::shared_ptr<Geometry>			m_geometry;
		
		bool								m_hidden;

		std::vector<std::shared_ptr<Node>>	m_childNodes;
		
		glm::vec3							m_position;
		glm::quat							m_orientation;
		glm::vec3							m_scale;
//		glm::mat4							m_pivot;
		
		std::shared_ptr<PhysicsBody>		m_physicsBody;
		
		std::weak_ptr<Scene> 				m_scene;
		std::weak_ptr<Node>					m_parent;
	};
}


#endif /* Node_h */
