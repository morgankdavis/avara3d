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
	class Node;
	
	
	class Light {
		
/*********************************************************************************************
	Public Static Member Functions
 *********************************************************************************************/

	public:

		static std::shared_ptr<Light> 		DefaultAmbient();
		static std::shared_ptr<Light> 		DefaultPoint();

/*********************************************************************************************
	Public Lifecycle Functions
 *********************************************************************************************/

		explicit Light(LightType type);
		Light(LightType type, const std::shared_ptr<Color>& color);
		virtual ~Light();

/*********************************************************************************************
	Public Member Functions
 *********************************************************************************************/

		const std::optional<std::string>&	name() const;
		void 								name(const std::string& name);
		
		LightType 							type() const;
		void 								type(LightType type);
		
		const std::shared_ptr<Color>&		color() const;
		void 								color(const std::shared_ptr<Color>& color);

		float 								attenuationFactor() const;
		void 								attenuationFactor(float factor);

/*********************************************************************************************
	Private Member Variables
 *********************************************************************************************/

	private:

		std::optional<std::string>			_name;
		
		LightType							_type;
		std::shared_ptr<Color>				_color;

		float								_attenuationFactor; // = 1/(1-k(d^2))
		
//		std::weak_ptr<Node>					_node;

//		float								_attenuationStartDistance;
//		float								_attenuationEndDistance;
//		float								_attenuationFalloffExponent;
//
//		float								_spotInnerAngle;
//		float								_spotOuterAngle;
	};
}


#endif /* AVARA3D_LIGHT_H */
