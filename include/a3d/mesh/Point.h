//
//  Point.h
//	avara3d
//
//  Created by Morgan Davis on 5/20/18.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_POINT_H
#define AVARA3D_POINT_H


#include <memory>

#include "glm/glm.hpp"


namespace a3d {
	
	
	class Color;

	
	class Point {

/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

	public:

		Point(const glm::vec3& location);
		Point(const glm::vec3& location, const std::shared_ptr<Color>& color);
		
/*********************************************************************************************
	Public
 *********************************************************************************************/
		
		const glm::vec3& 				location() const;
		void 							location(const glm::vec3& point);
		const std::shared_ptr<Color>&	color() const;
		void 							color(const std::shared_ptr<Color>& color);
		
/*********************************************************************************************
	Private
 *********************************************************************************************/

	private:

		glm::vec3 						_location;
		std::shared_ptr<Color> 			_color;
	};
}


#endif /* AVARA3D_POINT_H */
