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
		     Public Static
		 ***************************************************************************************/
		
		static std::shared_ptr<Light> DefaultAmbient();
		static std::shared_ptr<Light> DefaultPoint();
		static std::shared_ptr<Node> AmbientNode();
		static std::shared_ptr<Node> PointNode();
		
		/***************************************************************************************
		     Lifecycle
		 ***************************************************************************************/
		
		Light(LIGHT_TYPE type);
		Light(LIGHT_TYPE type, const std::shared_ptr<Color> color);
		
		/***************************************************************************************
		     Public
		 ***************************************************************************************/
		
		boost::optional<std::string> name() const;
		void name(const std::string& name);
		
		LIGHT_TYPE type() const;
		void type(LIGHT_TYPE type);
		
		std::shared_ptr<Color> color() const;
		void color(std::shared_ptr<Color> color);

		float attenuationFactor() const;
		void attenuationFactor(float factor);
		
		/***************************************************************************************
		     Internal
		 ***************************************************************************************/
		
		std::weak_ptr<Node> node() const;
		
		void attachedToNode(std::shared_ptr<Node> node);
		
	private:
		
		/***************************************************************************************
		     Private
		 ***************************************************************************************/
		
		boost::optional<std::string>		m_name;
		
		LIGHT_TYPE							m_type;
		std::shared_ptr<Color>				m_color;

		float								m_attenuationFactor; // att = 1/(1-k(d^2))
		
		std::weak_ptr<Node>					m_node;

//		float			attenuationStartDistance;
//		float			attenuationEndDistance;
//		float			attenuationFalloffExponent;
//
//		float			spotInnerAngle;
//		float			spotOuterAngle;
	};
	
}


#endif /* Light_h */
