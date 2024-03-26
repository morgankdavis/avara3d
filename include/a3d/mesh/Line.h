//
//  Line.h
//	avara3d
//
//  Created by Morgan Davis on 5/20/18.
//  Copyright © 2018 Morgan K Davis. All rights reserved.
//

#ifndef Line_h
#define Line_h


#include <memory>

#include "glm/glm.hpp"

#include "a3d/mesh/Mesh.h"


namespace a3d {
	
	
	class Color;

	
	class Line {

/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

	public:

		Line(const glm::vec3& fromLocation,
			 const glm::vec3& toLocation);
		Line(const glm::vec3& fromLocation,
			 const glm::vec3& toLocation,
			 const std::shared_ptr<Color>& color);
		Line(const glm::vec3& fromLocation,
			 const glm::vec3& toLocation,
			 const std::shared_ptr<Color>& fromColor,
			 const std::shared_ptr<Color>& toColor);

//		Line(const Line& other); // copy
//		Line& operator=(const Line& other); // copy assignment
//		Line(Line&& other) noexcept; // move
//		Line& operator=(Line&& other) noexcept; // move assignment
		
/*********************************************************************************************
	Public
 *********************************************************************************************/

		const glm::vec3& 					fromLocation() const;
		void 								fromLocation(const glm::vec3& location);
		const glm::vec3& 					toLocation() const;
		void 								toLocation(const glm::vec3& location);
		const std::shared_ptr<Color>& 		fromColor() const;
		void 								fromColor(const std::shared_ptr<Color>& color);
		const std::shared_ptr<Color>& 		toColor() const;
		void 								toColor(const std::shared_ptr<Color>& color);
		
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
