//
//  Box.h
//	avara-engine
//
//  Created by Morgan Davis on 10/31/17.
//  Copyright © 2017 Morgan K Davis. All rights reserved.
//

#ifndef Box_h
#define Box_h


#include "geometry/Geometry.h"


namespace ae {

	
	class Box: public Geometry {

/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

	public:

		// z, x, y?
		Box(float length, float width, float height,
			float lengthSegments = 1, float widthSegments = 1, float heightSegments = 1);
		
/*********************************************************************************************
	Public
 *********************************************************************************************/

		float 		length() const;
		float 		width() const;
		float 		height() const;

/*********************************************************************************************
	 Private
 *********************************************************************************************/

	private:

		float		_length;
		float		_width;
		float		_height;
	};
}


#endif /* Box_h */
