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
	Public Static Member Functions
 *********************************************************************************************/

	public:

		static bool classof(const Light* o);
		static bool classof(const Light& o);

/*********************************************************************************************
	Public Lifecycle Functions
 *********************************************************************************************/

//		explicit AttenuatedLight(const std::string& name, float );
//		explicit AttenuatedLight(const std::shared_ptr<Color>& color);
//		AttenuatedLight(const std::string& name, const std::shared_ptr<Color>& color);
		virtual ~AttenuatedLight();

/*********************************************************************************************
	Public Member Functions
 *********************************************************************************************/

		// I = 1 / (Kc + (Kl * d) + (Kq * d^2))
		//
		// default:
		//	Kc = 1.0
		//	Kl = 0.0
		//	Kq = 0.1

		float			constantAttenuation() const;
		void			constantAttenuation(float factor);

		float			linearAttenuation() const;
		void			linearAttenuation(float factor);

		float			quadraticAttenuation() const;
		void			quadraticAttenuation(float factor);

		// NOT implemented.
		// see notes in Types.h
		LightCutoff		cutoff() const;
		void			cutoff(LightCutoff cutoff);

/*********************************************************************************************
	Protected Lifecycle Functions
 *********************************************************************************************/

	protected:

		explicit AttenuatedLight(Kind k);

/*********************************************************************************************
	Private Member Variables
 *********************************************************************************************/

	private:

		float			_constantAttenuation;
		float			_linearAttenuation;
		float			_quadraticAttenuation;

//		LightCutoff		_cutoff;
	};
}


#endif /* AVARA3D_ATTENTUATEDLIGHT_H */
