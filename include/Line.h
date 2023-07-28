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

#include "Aliases.h"
#include "Geometry.h"


namespace ae {
	
	
	class Color;

	
	class Line: public std::enable_shared_from_this<Line> {

/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

	public:

		Line(glm::vec3 fromLocation, glm::vec3 toLocation);
		Line(glm::vec3 fromLocation, glm::vec3 toLocation,
			 ColorSPtr color);
		Line(glm::vec3 fromLocation, glm::vec3 toLocation,
			 ColorSPtr fromColor, ColorSPtr toColor);
		
/*********************************************************************************************
	Public
 *********************************************************************************************/
		
		glm::vec3 			fromLocation() const;
		void 				fromLocation(glm::vec3 location);
		glm::vec3 			toLocation() const;
		void 				toLocation(glm::vec3 location);
		ColorSPtr 			fromColor() const;
		void 				fromColor(ColorSPtr color);
		ColorSPtr 			toColor() const;
		void 				toColor(ColorSPtr color);
		
/*********************************************************************************************
	Private
 *********************************************************************************************/

	private:

		glm::vec3 			_fromLocation;
		glm::vec3 			_toLocation;
		ColorSPtr 			_fromColor;
		ColorSPtr 			_toColor;
	};
}


#endif /* Line_h */
