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
#include <optional>
#include <string>

//#include <boost/optional.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Aliases.h"


namespace ae {
	
	
	class Node;
	
	
	class Camera {
		
/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

	public:

		Camera();
		Camera(float zNear, float zFar, float fov);
		
/*********************************************************************************************
	Public
 *********************************************************************************************/
		
		std::optional<std::string> 		name();
		void 							name(std::string name);

		void 							translate(glm::vec3 translation);
		void 							rotate(glm::vec3 rotation);
		
		float 							fov();
		void	 						fov(float fov);
		
		float 							zNear();
		void 							zNear(float zNear);
		
		float 							zFar();
		void				 			zFar(float zFar);
		
		float 							aspectRatio();
		void 							aspectRatio(float ratio);
		
		glm::mat4 						projection();
//		void 							projection(const glm::mat4 projection);
		
/*********************************************************************************************
	Internal
 *********************************************************************************************/
		
		void 							constructProjectionMat();
		
		NodeWPtr 						node() const;
		
		void 							attachedToNode(NodeSPtr node);
		
/*********************************************************************************************
	Private
 *********************************************************************************************/

	private:

		std::optional<std::string>		_name;
		
		float							_fov;
		float							_zNear;
		float							_zFar;
		float							_aspectRatio;
		
		glm::mat4						_projection;

		NodeWPtr						_node;
	};
}


#endif /* Camera_h */
