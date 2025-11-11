//
//  AmbientLight.h
//  avara3d
//
//  Created by Morgan Davis on 7/31/2024.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_AMBIENTLIGHT_H
#define AVARA3D_AMBIENTLIGHT_H


#include <memory>
#include <optional>
#include <string>

#include "a3d/rendering/light/Light.h"


namespace a3d {


	class Color;
	
	
	class AmbientLight : public Light {

/*********************************************************************************************
	Public Static Member Functions
 *********************************************************************************************/

		static bool classof(const Light* l) {
			return l && l->kind() == Kind::Ambient;
		}

		static bool classof(const Light& l) {
			return classof(&l);
		}

/*********************************************************************************************
	Public Lifecycle Functions
 *********************************************************************************************/

	public:

		AmbientLight();
		explicit AmbientLight(const std::string& name);
		explicit AmbientLight(const std::shared_ptr<Color>& color);
		AmbientLight(const std::string& name, const std::shared_ptr<Color>& color);

/*********************************************************************************************
	Private Lifecycle Functions
 *********************************************************************************************/

//	private:
//
//		AmbientLight();
	};
}


#endif /* AVARA3D_AMBIENTLIGHT_H */
