//
//  Box.h
//	avara3d
//
//  Created by Morgan Davis on 10/31/17.
//  Copyright © 2017 Morgan K Davis. All rights reserved.
//

#ifndef Box_h
#define Box_h


#include "a3d/mesh/MeshElement.h"


namespace a3d {

	class Box: public MeshElement {

/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

	public:

		// z, x, y?
		Box(float length,
			float width,
			float height,
			unsigned lengthSegments = 1,
			unsigned widthSegments = 1,
			unsigned heightSegments = 1);
		
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
