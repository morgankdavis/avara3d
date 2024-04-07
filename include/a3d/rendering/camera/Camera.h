//
//  Camera.h
//	avara3d
//
//  Created by Morgan Davis on 10/21/16.
//  Copyright © 2016 Morgan K Davis. All rights reserved.
//

#ifndef Camera_h
#define Camera_h


#include <memory>
#include <optional>
#include <string>

#include "a3d/Types.h"


namespace a3d {

	class Camera {
		
/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

	public:

		Camera();
		explicit Camera(const std::string& name);
		virtual ~Camera() = 0;
		
/*********************************************************************************************
	Public
 *********************************************************************************************/
		
		const std::optional<std::string>&	name() const;
		void 								name(const std::string& name);

		glm::mat4 							projection() const;

/*********************************************************************************************
	Protected
 *********************************************************************************************/

	protected:

		virtual void 					constructProjectionMatrix() = 0;

		std::optional<std::string>		_name;
		glm::mat4						_projection;
	};
}


#endif /* Camera_h */
