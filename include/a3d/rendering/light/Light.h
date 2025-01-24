//
//  Light.h
//  avara3d
//
//  Created by Morgan Davis on 10/21/16.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_LIGHT_H
#define AVARA3D_LIGHT_H


#include <memory>
#include <optional>
#include <string>

#include "a3d/Types.h"


namespace a3d {


	class Color;
	
	
	class Light {

/*********************************************************************************************
	Public Lifecycle Functions
 *********************************************************************************************/

	public:

		Light();
		explicit Light(const std::string& name);
		explicit Light(const std::shared_ptr<Color>& color);
		Light(const std::string& name, const std::shared_ptr<Color>& color);
		virtual ~Light() = 0;

/*********************************************************************************************
	Public Member Functions
 *********************************************************************************************/

		const std::optional<std::string>&	name() const;
		void 								name(const std::string& name);

		const std::shared_ptr<Color>&		color() const;
		void 								color(const std::shared_ptr<Color>& color);


		LightType type() const;
		void type(LightType type);
		LightType _type;


/*********************************************************************************************
	Protected Lifecycle Functions
 *********************************************************************************************/

//	protected:
//
//		Light();

/*********************************************************************************************
	Protected Member Variables
 *********************************************************************************************/

	protected:

		std::optional<std::string>			_name;
		std::shared_ptr<Color>				_color;
	};
}


#endif /* AVARA3D_LIGHT_H */
