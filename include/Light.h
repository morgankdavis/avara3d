//
//  Light.h
//	avara-engine
//
//  Created by Morgan Davis on 10/21/16.
//  Copyright © 2016 Morgan K Davis. All rights reserved.
//

#ifndef Light_h
#define Light_h


#include <memory>
#include <optional>
#include <string>

//#include <boost/optional.hpp>

#include "Types.h"


namespace ae {


	class Color;
	class Node;
	
	
	class Light {
		
/*********************************************************************************************
	Public Static
 *********************************************************************************************/

	public:

		static std::shared_ptr<ae::Light> DefaultAmbient();
		static std::shared_ptr<ae::Light> DefaultPoint();
		static std::shared_ptr<ae::Node> AmbientNode();
		static std::shared_ptr<ae::Node> PointNode();
		
/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/
		
		Light(LIGHT_TYPE type);
		Light(LIGHT_TYPE type, const std::shared_ptr<ae::Color> color);
		
/*********************************************************************************************
	Public
 *********************************************************************************************/
		
		std::optional<std::string> 		name() const;
		void 							name(const std::string& name);
		
		LIGHT_TYPE 						type() const;
		void 							type(LIGHT_TYPE type);
		
		std::shared_ptr<ae::Color> 						color() const;
		void 							color(std::shared_ptr<ae::Color> color);

		float 							attenuationFactor() const;
		void 							attenuationFactor(float factor);
		
/*********************************************************************************************
	Internal
 *********************************************************************************************/

	// protected

		std::weak_ptr<ae::Node> 						node() const;
		
		void 							attachedToNode(std::shared_ptr<ae::Node> node);
		
/*********************************************************************************************
	Private
 *********************************************************************************************/

	private:

		std::optional<std::string>		_name;
		
		LIGHT_TYPE						_type;
		std::shared_ptr<ae::Color>						_color;

		float							_attenuationFactor; // att = 1/(1-k(d^2))
		
		std::weak_ptr<Node>				_node;

//		float							_attenuationStartDistance;
//		float							_attenuationEndDistance;
//		float							_attenuationFalloffExponent;
//
//		float							_spotInnerAngle;
//		float							_spotOuterAngle;
	};
	
}


#endif /* Light_h */
