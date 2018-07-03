//
//  PresentationNode.h
//	avara-engine
//
//  Created by Morgan Davis on 7/2/18.
//  Copyright © 2018 Morgan K Davis. All rights reserved.
//

#ifndef PresentationNode_h
#define PresentationNode_h


#include "Node.h"


namespace ae {

	class PresentationNode : public Node {
		
	public:
		
		/***************************************************************************************
	    	 Lifecycle
	 	 ***************************************************************************************/
		
		PresentationNode(std::shared_ptr<Node> owner);
		~PresentationNode();

		/***************************************************************************************
     		Public
 		 ***************************************************************************************/

		glm::vec3 position() const override;
		glm::vec4 rotation() const override; // axis-angle
		glm::vec3 eulerAngles() const override; // pitch, yaw, roll
		glm::quat orientation() const override; // angle == 1st component
		glm::vec3 scale() const override;
		glm::mat4 transform() const override;
		
//		glm::vec3 worldPosition() override;
//		glm::vec4 worldRotation() override; // axis-angle
//		glm::vec3 worldEulerAngles() override; // pitch, yaw, roll
//		glm::quat worldOrientation() override; // angle == 1st component
//		glm::vec3 worldScale() override;
//		
//		glm::vec3 worldForward() override;
//		glm::vec3 worldUp() override;
//		glm::vec3 worldRight() override;
		
		glm::mat4 worldTransform() override;
		
		/***************************************************************************************
     		Internal
 		 ***************************************************************************************/
		
		void worldTransform(glm::mat4 transform) override;
		
		std::weak_ptr<Node> owner() const;
//		void attachedToModel(std::shared_ptr<Node> model);
		
	private:

		/***************************************************************************************
     		Private
 		 ***************************************************************************************/
		
		void transform(const glm::mat4 transform) override;
		
		glm::vec3							m_position;
		glm::quat							m_orientation;
		glm::vec3							m_scale;
		//glm::mat4							m_worldTransform;
		
		std::weak_ptr<Node> 				m_owner;
	};
}


#endif /* PresentationNode_h */
