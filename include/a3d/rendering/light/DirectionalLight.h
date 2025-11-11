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
	Public Static Member Functions
 *********************************************************************************************/

		static bool classof(const Light* l) {
			return l && l->kind() == Kind::Directional;
		}

		static bool classof(const Light& l) {
			return classof(&l);
		}

/*********************************************************************************************
	Public Lifecycle Functions
 *********************************************************************************************/

	public:

		DirectionalLight();
		explicit DirectionalLight(const std::string& name);
		explicit DirectionalLight(const std::shared_ptr<Color>& color);
		DirectionalLight(const std::string& name, const std::shared_ptr<Color>& color);
//		~DirectionalLight();

/*********************************************************************************************
	Public Member Functions
 *********************************************************************************************/

//		const glm::vec3&	direction() const;
//		void				direction(const glm::vec3& direction);

/*********************************************************************************************
	Protected Lifecycle Functions
 *********************************************************************************************/

//	protected:
//
//		DirectionalLight();

/*********************************************************************************************
	Private Member Variables
 *********************************************************************************************/

//	private:
//
//		glm::vec3			_direction; // TODO: INIT ME
	};
}


#endif /* AVARA3D_DIRECTIONALLIGHT_H */
