//
//  DirectionalLight.h
//  avara3d
//
//  Created by Morgan Davis on 7/31/2024.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_DIRECTIONALLIGHT_H
#define AVARA3D_DIRECTIONALLIGHT_H


#include <memory>
#include <optional>
#include <string>

#include "glm/glm.hpp"

#include "a3d/rendering/light/Light.h"


namespace a3d {


	class Color;
	class Node;
	
	
	class DirectionalLight : public Light {

/*********************************************************************************************
	Public Lifecycle Functions
 *********************************************************************************************/

	public:

		DirectionalLight();
		explicit DirectionalLight(const std::string& name);
		explicit DirectionalLight(const std::shared_ptr<Color>& color);
		DirectionalLight(const std::string& name, const std::shared_ptr<Color>& color);

/*********************************************************************************************
	Protected Lifecycle Functions
 *********************************************************************************************/

//	protected:
//
//		DirectionalLight();
	};
}


#endif /* AVARA3D_DIRECTIONALLIGHT_H */
