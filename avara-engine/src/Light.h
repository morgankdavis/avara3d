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
	class Node;
	
	
	class Light {
		
	public:
		
		/***************************************************************************************
		     MARK:   Lifecycle
		 **************************************************************************************/
		
		Light(LightType type);
		Light(LightType type, const std::shared_ptr<Color> color);
		
		/***************************************************************************************
		     MARK:   Public
		 **************************************************************************************/
		
		boost::optional<std::string> name() const;
		void name(const std::string& name);
		
		LightType type() const;
		void type(LightType type);
		
		std::shared_ptr<Color> color() const;
		void color(std::shared_ptr<Color> color);

		float attenuationFactor() const;
		void attenuationFactor(float factor);
		
		/***************************************************************************************
		     MARK:   Internal
		 **************************************************************************************/
		
		Node* node() const;
		void node(Node* node);
		
	private:
		
		/***************************************************************************************
		     MARK:   Private
		 **************************************************************************************/
		
		boost::optional<std::string>		m_name;
		
		LightType							m_type;
		std::shared_ptr<Color>				m_color;
		
		Node*								m_node;

		float								m_attenuationFactor; // att = 1/(1-k(d^2))

//		float			attenuationStartDistance;
//		float			attenuationEndDistance;
//		float			attenuationFalloffExponent;
//
//		float			spotInnerAngle;
//		float			spotOuterAngle;
	};
	
}


#endif /* Light_h */
