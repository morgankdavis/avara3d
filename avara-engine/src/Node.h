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

#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>


namespace ae {


	class Camera;
	class Geometry;
	class Light;


	class Node : public std::enable_shared_from_this<Node> {
		
	public:

		/***************************************************************************************
	    	 MARK:   Lifecycle
	 	 **************************************************************************************/
		
		Node();
		Node(const std::string& name);
		Node(const std::shared_ptr<Geometry> geometry);
		Node(const std::string& name, const glm::mat4 transform);
		Node(const std::string& name, const glm::mat4 transform, std::shared_ptr<Geometry> geometry);

		/***************************************************************************************
     		MARK:   Public
 		 **************************************************************************************/

		std::string name() const;
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
		
		glm::mat4 worldTransform();
		
		glm::vec3 worldForward();
		glm::vec3 worldUp();
		glm::vec3 worldRight();
		
		//std::weak_ptr<Node>					m_parent;
		
		void addChildNodes(std::vector<std::shared_ptr<Node>> nodes);
		void addChildNode(std::shared_ptr<Node> node);
		void insertChildNode(const Node& node, const int index);
		void removeFromParentNode();
		void replaceChildNode(const Node& replace, const Node& with);
		
		Node* parent() const;
		std::shared_ptr<Node> childNode(const std::string& name, const bool resursive);
		std::vector<std::shared_ptr<Node>> allChildNodes();
		

//		glm::vec3 convertPositionFromNode(const glm::vec3& position, const Node& fromNode);
//		glm::vec3 convertPositionToNode(const glm::vec3& position, const Node& toNode);
//		glm::mat4 convertTransformFromNode(const glm::mat4& transform, const Node& fromNode);
//		glm::mat4 convertTransformToNode(const glm::mat4& transform, const Node& toNode);

		/***************************************************************************************
     		MARK:   Internal
 		 **************************************************************************************/
		
		void parent(Node* parent);
		std::vector<Node*> pathToRoot();
		std::vector<std::shared_ptr<Node>> immediateChildNodes();
		
	private:

		/***************************************************************************************
     		MARK:   Private
 		**************************************************************************************/
		
		std::string							m_name;
		
		std::shared_ptr<Light>				m_light;
		std::shared_ptr<Camera>				m_camera;
		std::shared_ptr<Geometry>			m_geometry;
		
		bool								m_hidden;

		Node*								m_parent;
		std::vector<std::shared_ptr<Node>>	m_childNodes;
		
		glm::vec3							m_position;
		glm::quat							m_orientation;
		glm::vec3							m_scale;
//		glm::mat4							m_pivot;
		
		std::vector<std::shared_ptr<Node>>	allChildNodesRec();
	};
}


#endif /* Node_h */
