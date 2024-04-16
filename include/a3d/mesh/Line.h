//
//  Line.h
//	avara3d
//
//  Created by Morgan Davis on 5/20/18.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_LINE_H
#define AVARA3D_LINE_H


#include <memory>

#include "glm/glm.hpp"

#include "a3d/Color.h"


namespace a3d {

	class Line {

/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

	public:

		Line(const glm::vec3& fromLocation,
			 const glm::vec3& toLocation);
		Line(const glm::vec3& fromLocation,
			 const glm::vec3& toLocation,
			 const Color& color);
		Line(const glm::vec3& fromLocation,
			 const glm::vec3& toLocation,
			 const Color& fromColor,
			 const Color& toColor);

/*********************************************************************************************
	Public
 *********************************************************************************************/

		const glm::vec3& 		fromLocation() const;
		void 					fromLocation(const glm::vec3& location);

		const glm::vec3& 		toLocation() const;
		void 					toLocation(const glm::vec3& location);

		const Color& 			fromColor() const;
		void 					fromColor(const Color& color);

		const Color& 			toColor() const;
		void 					toColor(const Color& color);
		
/*********************************************************************************************
	Private
 *********************************************************************************************/

	private:

		glm::vec3 				_fromLocation;
		glm::vec3 				_toLocation;
		Color 					_fromColor;
		Color 					_toColor;
	};
}


#endif /* AVARA3D_LINE_H */
