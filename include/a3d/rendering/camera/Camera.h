//
//  Camera.h
//  avara3d
//
//  Created by Morgan Davis on 10/21/16.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_CAMERA_H
#define AVARA3D_CAMERA_H


#include <memory>
#include <optional>
#include <string>

#include "a3d/Types.h"


namespace a3d {

	class Camera {

	public:

/*********************************************************************************************
	Public
 *********************************************************************************************/

		Camera();
		explicit Camera(const std::string& name);
		virtual ~Camera() = 0;

		const std::optional<std::string>&	name() const;
		void 								name(const std::string& name);

		glm::mat4 							projection() const;

	protected:

/*********************************************************************************************
	Protected
 *********************************************************************************************/

		virtual void 					constructProjectionMatrix() = 0;

		std::optional<std::string>		_name;
		glm::mat4						_projection;
	};
}


#endif /* AVARA3D_CAMERA_H */
