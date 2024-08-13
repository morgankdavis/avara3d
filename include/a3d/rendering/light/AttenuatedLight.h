//
//  AttenuatedLight.h
//  avara3d
//
//  Created by Morgan Davis on 7/31/2024.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_ATTENTUATEDLIGHT_H
#define AVARA3D_ATTENTUATEDLIGHT_H


#include <memory>
#include <optional>
#include <string>

#include "a3d/rendering/light/Light.h"


namespace a3d {


	class Color;
	
	
	class AttenuatedLight : public Light {

/*********************************************************************************************
	Public Lifecycle Functions
 *********************************************************************************************/

	public:

//		explicit AttenuatedLight(const std::string& name, float );
//		explicit AttenuatedLight(const std::shared_ptr<Color>& color);
//		AttenuatedLight(const std::string& name, const std::shared_ptr<Color>& color);
		virtual ~AttenuatedLight() = 0;

/*********************************************************************************************
	Public Member Functions
 *********************************************************************************************/

		float			constantAttenuation() const;
		void			constantAttenuation(float factor);

		float			linearAttenuation() const;
		void			linearAttenuation(float factor);

		float			quadraticAttenuation() const;
		void			quadraticAttenuation(float factor);

/*********************************************************************************************
	Protected Lifecycle Functions
 *********************************************************************************************/

	protected:

		AttenuatedLight();

/*********************************************************************************************
	Private Member Variables
 *********************************************************************************************/

	private:

		// Gerdelan:
		// 		A = 1 / (1 - k(d^2))
		// de Vries:
		//		A = 1 / (Kc + (Kl * d) + (Kq * d^2))

		float			_constantAttenuation;
		float			_linearAttenuation;
		float			_quadraticAttenuation;
	};
}


#endif /* AVARA3D_ATTENTUATEDLIGHT_H */
