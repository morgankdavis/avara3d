//
//  Line.h
//	avara-engine
//
//  Created by Morgan Davis on 5/20/18.
//  Copyright © 2018 Morgan K Davis. All rights reserved.
//

#ifndef Line_h
#define Line_h


#include <memory>

#include <glm/glm.hpp>

#include "Geometry.h"


namespace ae {
	
	
	class Color;

	
	class Line: public std::enable_shared_from_this<Line> {
		
	public:

/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/
		
		Line(glm::vec3 fromLocation, glm::vec3 toLocation);
		Line(glm::vec3 fromLocation, glm::vec3 toLocation,
			 std::shared_ptr<Color> color);
		Line(glm::vec3 fromLocation, glm::vec3 toLocation,
			 std::shared_ptr<Color> fromColor, std::shared_ptr<Color> toColor);
		
/*********************************************************************************************
	Public
 *********************************************************************************************/
		
		glm::vec3 fromLocation() const;
		void fromLocation(glm::vec3 location);
		glm::vec3 toLocation() const;
		void toLocation(glm::vec3 location);
		std::shared_ptr<Color> fromColor() const;
		void fromColor(std::shared_ptr<Color> color);
		std::shared_ptr<Color> toColor() const;
		void toColor(std::shared_ptr<Color> color);
		
	private:
		
/*********************************************************************************************
	Private
 *********************************************************************************************/
		
		glm::vec3 					m_fromLocation;
		glm::vec3 					m_toLocation;
		std::shared_ptr<Color> 		m_fromColor;
		std::shared_ptr<Color> 		m_toColor;
	};
}


#endif /* Line_h */
