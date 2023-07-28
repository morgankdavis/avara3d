//
//  Point.h
//	avara-engine
//
//  Created by Morgan Davis on 5/20/18.
//  Copyright © 2018 Morgan K Davis. All rights reserved.
//

#ifndef Point_h
#define Point_h


#include <memory>

#include <glm/glm.hpp>

#include "Aliases.h"


namespace ae {
	
	
	class Color;

	
	class Point : public std::enable_shared_from_this<Point> {

/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

	public:

		Point(glm::vec3 location);
		Point(glm::vec3 location, ColorSPtr color);
		
/*********************************************************************************************
	Public
 *********************************************************************************************/
		
		glm::vec3 		location() const;
		void 			location(glm::vec3 point);
		ColorSPtr 		color() const;
		void 			color(ColorSPtr color);
		
/*********************************************************************************************
	Private
 *********************************************************************************************/

	private:

		glm::vec3 		_location;
		ColorSPtr 		_color;
	};
}


#endif /* Point_h */
