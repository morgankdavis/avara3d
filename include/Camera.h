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

#include <glm/gtc/matrix_transform.hpp>

#include "Types.h"


namespace ae {
	
	
	class Node;
	
	
	class Camera {
		
/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

	public:

		Camera();
		
/*********************************************************************************************
	Public
 *********************************************************************************************/
		
		std::optional<std::string> 		name() const;
		void 							name(std::string name);

		float 							zNear() const;
		void 							zNear(float zNear);
		
		float 							zFar() const;
		void				 			zFar(float zFar);

//		PROJECTION_TYPE					projectionType() const;
//		void							projectionType(PROJECTION_TYPE type);
		
		glm::mat4 						projection() const;
		
/*********************************************************************************************
	Internal
 *********************************************************************************************/
		
		virtual void 					constructProjection() = 0;
		std::weak_ptr<ae::Node> 		node() const;
		void 							attachedToNode(std::shared_ptr<ae::Node> node);

		glm::mat4						_projection;
		
/*********************************************************************************************
	Private
 *********************************************************************************************/

	private:

		std::optional<std::string>		_name;
		float							_zNear;
		float							_zFar;
		std::weak_ptr<ae::Node>			_node;
	};
}


#endif /* Camera_h */
