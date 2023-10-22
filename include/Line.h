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

/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

	public:

		Line(glm::vec3 fromLocation, glm::vec3 toLocation);
		Line(glm::vec3 fromLocation, glm::vec3 toLocation,
			 std::shared_ptr<ae::Color> color);
		Line(glm::vec3 fromLocation, glm::vec3 toLocation,
			 std::shared_ptr<ae::Color> fromColor, std::shared_ptr<ae::Color> toColor);
		
/*********************************************************************************************
	Public
 *********************************************************************************************/
		
		glm::vec3 							fromLocation() const;
		void 								fromLocation(glm::vec3 location);
		glm::vec3 							toLocation() const;
		void 								toLocation(glm::vec3 location);
		std::shared_ptr<ae::Color> 			fromColor() const;
		void 								fromColor(std::shared_ptr<ae::Color> color);
		std::shared_ptr<ae::Color> 			toColor() const;
		void 								toColor(std::shared_ptr<ae::Color> color);
		
/*********************************************************************************************
	Private
 *********************************************************************************************/

	private:

		glm::vec3 							_fromLocation;
		glm::vec3 							_toLocation;
		std::shared_ptr<ae::Color> 			_fromColor;
		std::shared_ptr<ae::Color> 			_toColor;
	};
}


#endif /* Line_h */
