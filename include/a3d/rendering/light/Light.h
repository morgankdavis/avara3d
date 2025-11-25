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


	class AmbientLight;
	class Color;
	class DirectionalLight;
	class PointLight;
	class SpotLight;


	class Light {

/*********************************************************************************************
	Public Types
 *********************************************************************************************/

	public:

		enum class Kind {
			Ambient,
			Directional,
			Point,
			Spot
		};

/*********************************************************************************************
	Public Static Member Functions
 *********************************************************************************************/

	public:

		static bool classof(const Light* l);
		static bool classof(const Light& l);

		static std::shared_ptr<a3d::AmbientLight> 		AmbientLight();
		static std::shared_ptr<a3d::AmbientLight> 		AmbientLight(const std::shared_ptr<Color>& color);

		static std::shared_ptr<a3d::DirectionalLight>	DirectionalLight();
		static std::shared_ptr<a3d::DirectionalLight>	DirectionalLight(const std::shared_ptr<Color>& color);

		static std::shared_ptr<a3d::PointLight> 		PointLight();
		static std::shared_ptr<a3d::PointLight> 		PointLight(const std::shared_ptr<Color>& color);

		static std::shared_ptr<a3d::SpotLight> 			SpotLight();
		static std::shared_ptr<a3d::SpotLight> 			SpotLight(const std::shared_ptr<Color>& color);

/*********************************************************************************************
	Public Lifecycle Functions
 *********************************************************************************************/

//		Light();
//		explicit Light(const std::string& name) = 0;
//		explicit Light(const std::shared_ptr<Color>& color);
//		Light(const std::string& name, const std::shared_ptr<Color>& color);
		virtual ~Light();

/*********************************************************************************************
	Public Member Functions
 *********************************************************************************************/

		const std::optional<std::string>&	name() const;
		void 								name(const std::string& name);

		const std::shared_ptr<Color>&		color() const;
		void 								color(const std::shared_ptr<Color>& color);

		Kind 								kind() const;

/*********************************************************************************************
	Protected Lifecycle Functions
 *********************************************************************************************/

	protected:

		explicit Light(Kind k);

/*********************************************************************************************
	Protected Member Variables
 *********************************************************************************************/

	protected:

		Kind							_kind;
		std::optional<std::string>		_name;
		std::shared_ptr<Color>			_color;
	};
}


#endif /* AVARA3D_LIGHT_H */
