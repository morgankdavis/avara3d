//
//  SpotLight.h
//  avara3d
//
//  Created by Morgan Davis on 7/31/2024.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_SPOTLIGHT_H
#define AVARA3D_SPOTLIGHT_H


#include "a3d/rendering/light/AttenuatedLight.h"


namespace a3d {


	class Color;
	
	
	class SpotLight : public AttenuatedLight {

		/*********************************************************************************************
	Public Lifecycle Functions
 *********************************************************************************************/

	public:

		explicit SpotLight(const std::string& name);
		explicit SpotLight(const std::shared_ptr<Color>& color);
		SpotLight(const std::string& name, const std::shared_ptr<Color>& color);

/*********************************************************************************************
	Public Member Functions
 *********************************************************************************************/

	public:

		float			innerAngle() const;
		void			innerAngle(float angle);

		float			outerAngle() const;
		void			outerAngle(float angle);

/*********************************************************************************************
	Private Lifecycle Functions
 *********************************************************************************************/

	private:

		SpotLight();

/*********************************************************************************************
	Private Member Variables
 *********************************************************************************************/

		float			_innerAngle;
		float			_outerAngle;
	};
}


#endif /* AVARA3D_SPOTLIGHT_H */
