//
//  Box.h
//	avara-engine
//
//  Created by Morgan Davis on 10/31/17.
//  Copyright © 2017 Morgan K Davis. All rights reserved.
//

#ifndef Box_h
#define Box_h


#include "Geometry.h"


namespace ae {

	
	class Box: public Geometry {
		
	public:

/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/
		
		Box(float width, float height, float length);
		
/*********************************************************************************************
	Public
 *********************************************************************************************/
		
		float width() const;
		float height() const;
		float length() const;
		
	private:
		
/*********************************************************************************************
	 Private
 *********************************************************************************************/
		
		float	_width;
		float	_height;
		float	_length;
	};
}


#endif /* Box_h */
