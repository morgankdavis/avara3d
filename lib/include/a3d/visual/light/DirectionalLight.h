//
//  DirectionalLight.h
//  avara3d
//
//  Created by Morgan Davis on 7/31/2024.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_VISUAL_LIGHT_DIRECTIONALLIGHT_H
#define AVARA3D_VISUAL_LIGHT_DIRECTIONALLIGHT_H

#include <memory>
#include <string>

#include "a3d/visual/light/Light.h"

namespace a3d {

	class Color;
	class Node;
	
	class DirectionalLight : public Light {

	public:
		/// Public Lifecycle Functions ///

		DirectionalLight();
		explicit DirectionalLight(const std::string& name);
		explicit DirectionalLight(const std::shared_ptr<Color>& color);
		DirectionalLight(const std::string& name, const std::shared_ptr<Color>& color);
	};
}

#endif //AVARA3D_VISUAL_LIGHT_DIRECTIONALLIGHT_H */
