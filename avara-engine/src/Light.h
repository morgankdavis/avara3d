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
#include <string>

#include <boost/optional.hpp>

#include "Types.h"


namespace ae {


	class Color;
	
	
	class Light {
		
	public:
		
		/***************************************************************************************
		     MARK:   Lifecycle
		 **************************************************************************************/
		
		Light();
		
		/***************************************************************************************
		     MARK:   Public
		 **************************************************************************************/
		
		boost::optional<std::string> name() const;
		void name(const std::string& name);
		
		/***************************************************************************************
		     MARK:   Private
		 **************************************************************************************/
		
		boost::optional<std::string>			m_name;
		
//		std::string		name;
//
//		Color			color;
//
//		float			attenuationStartDistance;
//		float			attenuationEndDistance;
//		float			attenuationFalloffExponent;
//
//		float			spotInnerAngle;
//		float			spotOuterAngle;
	};
	
}


#endif /* Light_h */
