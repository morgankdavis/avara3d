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

#include "glm/glm.hpp"

#include "ae/mesh/Mesh.h"


namespace ae {
	
	
	class Color;

	
	class Line {

/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

	public:

		Line(glm::vec3 fromLocation, glm::vec3 toLocation);
		Line(glm::vec3 fromLocation, glm::vec3 toLocation,
			 std::shared_ptr<Color> color);
		Line(glm::vec3 fromLocation, glm::vec3 toLocation,
			 std::shared_ptr<Color> fromColor, std::shared_ptr<Color> toColor);
		
/*********************************************************************************************
	Public
 *********************************************************************************************/
		
		glm::vec3 							fromLocation() const;
		void 								fromLocation(glm::vec3 location);
		glm::vec3 							toLocation() const;
		void 								toLocation(glm::vec3 location);
		std::shared_ptr<Color> 				fromColor() const;
		void 								fromColor(std::shared_ptr<Color> color);
		std::shared_ptr<Color> 				toColor() const;
		void 								toColor(std::shared_ptr<Color> color);
		
/*********************************************************************************************
	Private
 *********************************************************************************************/

	private:

		glm::vec3 							_fromLocation;
		glm::vec3 							_toLocation;
		std::shared_ptr<Color> 				_fromColor;
		std::shared_ptr<Color> 				_toColor;
	};
}


#endif /* Line_h */
