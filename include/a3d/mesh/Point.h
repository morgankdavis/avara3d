//
//  Point.h
//  avara3d
//
//  Created by Morgan Davis on 5/20/18.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_POINT_H
#define AVARA3D_POINT_H

#include <memory>

#include "glm/glm.hpp"

#include "a3d/Color.h"

namespace a3d {
	
	class Color;
	
	class Point {

/*********************************************************************************************
	Internal Lifecycle Functions
 *********************************************************************************************/

	public:

		explicit Point(const glm::vec3& location);
		Point(const glm::vec3& location, const Color& color);

/*********************************************************************************************
	Internal Member Functions
 *********************************************************************************************/

		const glm::vec3& 		location() const;
		void 					location(const glm::vec3& point);
		const Color&			color() const;
		void 					color(const Color& color);
		
/*********************************************************************************************
	Private Member Variables
 *********************************************************************************************/

	private:

		glm::vec3 				_location;
		Color 					_color;
	};
}

#endif /* AVARA3D_POINT_H */
