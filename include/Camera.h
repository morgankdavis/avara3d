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

#include "Types.h"


namespace ae {
	
	
	class Node;
	
	
	class Camera {
		
/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

	public:

		Camera();
		Camera(std::optional<std::string> name);
		
/*********************************************************************************************
	Public
 *********************************************************************************************/
		
		std::optional<std::string> 		name() const;
		void 							name(std::string name);

		glm::mat4 						projection() const;
		
/*********************************************************************************************
	Internal
 *********************************************************************************************/

		std::weak_ptr<Node> 		node() const;
//		void 							attachedToNode(std::shared_ptr<Node> node);

	protected:

		virtual void 					constructProjectionMatrix() = 0;
		glm::mat4						_projection;
		
/*********************************************************************************************
	Private
 *********************************************************************************************/

	private:

		std::optional<std::string>		_name;
//		std::weak_ptr<Node>			_node;
	};
}


#endif /* Camera_h */
