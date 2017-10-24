//
//  Camera.h
//	avara-engine
//
//  Created by Morgan Davis on 10/21/16.
//  Copyright © 2016 Morgan K Davis. All rights reserved.
//

#ifndef Camera_h
#define Camera_h


#include <string>

#include <glm/gtc/matrix_transform.hpp>


namespace ae {
	
	
	class Node;
	
	
	class Camera {
		
	public:
		
		/***************************************************************************************
		     MARK:   Lifecycle
		 **************************************************************************************/
		
		Camera();
		Camera(const float zNear, const float zFar, const float fov);
		
		/***************************************************************************************
		     MARK:   Public
		 **************************************************************************************/
		
		std::string name();
		void name(const std::string name);
		
//		glm::vec3 forward() const;
//		glm::vec3 up() const;
//		glm::vec3 right() const;
		
		void translate(const glm::vec3 translation);
		void rotate(const glm::vec3 rotation);
		
		float fov();
		void fov(const float fov);
		
		float zNear();
		void zNear(const float zNear);
		
		float zFar();
		void zFar(const float zFar);
		
		glm::mat4 projection();
		void projection(const glm::mat4 projection);
		
		/***************************************************************************************
		     MARK:   Internal
		 **************************************************************************************/
		
		Node* node() const;
		void node(Node* node);
		
	private:
		
		/***************************************************************************************
		     MARK:   Private
		 **************************************************************************************/
		
		std::string				m_name;
		
		float					m_fov;
		float					m_zNear;
		float					m_zFar;
		
		glm::mat4				m_projection;
		
		Node*					m_node;
	};
}


#endif /* Camera_h */
