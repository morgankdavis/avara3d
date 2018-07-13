//
//  Camera.h
//	avara-engine
//
//  Created by Morgan Davis on 10/21/16.
//  Copyright © 2016 Morgan K Davis. All rights reserved.
//

#ifndef Camera_h
#define Camera_h


#include <memory>
#include <string>

#include <glm/gtc/matrix_transform.hpp>


namespace ae {
	
	
	class Node;
	
	
	class Camera {
		
	public:
		
		/***************************************************************************************
		     Lifecycle
		 ***************************************************************************************/
		
		Camera();
		Camera(float zNear, float zFar, float fov);
		
		/***************************************************************************************
		     Public
		 ***************************************************************************************/
		
		std::string name();
		void name(std::string name);

		void translate(glm::vec3 translation);
		void rotate(glm::vec3 rotation);
		
		float fov();
		void fov(float fov);
		
		float zNear();
		void zNear(float zNear);
		
		float zFar();
		void zFar(float zFar);
		
		float aspectRatio();
		void aspectRatio(float ratio);
		
		glm::mat4 projection();
//		void projection(const glm::mat4 projection);
		
		/***************************************************************************************
		     Internal
		 ***************************************************************************************/
		
		void constructProjectionMat();
		
		std::weak_ptr<Node> node() const;
		//void node(std::shared_ptr<Node> node);
		
		void attachedToNode(std::shared_ptr<Node> node);
		
	private:
		
		/***************************************************************************************
		     Private
		 ***************************************************************************************/
		
		std::string				m_name;
		
		float					m_fov;
		float					m_zNear;
		float					m_zFar;
		float					m_aspectRatio;
		
		glm::mat4				m_projection;
		
		std::weak_ptr<Node>		m_node;
	};
}


#endif /* Camera_h */
