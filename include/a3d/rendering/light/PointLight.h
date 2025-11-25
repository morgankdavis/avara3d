//
//  PointLight.h
//  avara3d
//
//  Created by Morgan Davis on 7/31/2024.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_POINTLIGHT_H
#define AVARA3D_POINTLIGHT_H


#include <memory>
#include <optional>
#include <string>

#include "a3d/rendering/light/AttenuatedLight.h"


namespace a3d {


	class Color;
	
	
	class PointLight : public AttenuatedLight {

/*********************************************************************************************
	Public Static Member Functions
 *********************************************************************************************/

	public:

		static bool classof(const Light* o);
		static bool classof(const Light& o);

/*********************************************************************************************
	Public Lifecycle Functions
 *********************************************************************************************/

		PointLight();
		explicit PointLight(const std::string& name);
		explicit PointLight(const std::shared_ptr<Color>& color);
		PointLight(const std::string& name, const std::shared_ptr<Color>& color);
	};
}


#endif /* AVARA3D_POINTLIGHT_H */
