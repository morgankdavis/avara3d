//
//  PointLight.h
//  avara3d
//
//  Created by Morgan Davis on 7/31/2024.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_POINTLIGHT_H
#define AVARA3D_POINTLIGHT_H


#include "a3d/rendering/light/AttenuatedLight.h"


namespace a3d {


	class Color;
	
	
	class PointLight : public AttenuatedLight {

/*********************************************************************************************
	Public Lifecycle Functions
 *********************************************************************************************/

	public:

		explicit PointLight(const std::string& name);
		explicit PointLight(const std::shared_ptr<Color>& color);
		PointLight(const std::string& name, const std::shared_ptr<Color>& color);

/*********************************************************************************************
	Private Lifecycle Functions
 *********************************************************************************************/

	private:

		PointLight();
	};
}


#endif /* AVARA3D_POINTLIGHT_H */
