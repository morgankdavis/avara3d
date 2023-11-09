//
//  Plane.h
//	avara-engine
//
//  Created by Morgan Davis on 10/31/17.
//  Copyright © 2017 Morgan K Davis. All rights reserved.
//

#ifndef Plane_h
#define Plane_h


#include "geometry/Geometry.h"


namespace ae {

	
	class Plane: public Geometry {

/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

	public:

		Plane(float width, float height);
		
/*********************************************************************************************
 	Public
 *********************************************************************************************/
		
		float 		width() const;
		float 		height() const;
		
/*********************************************************************************************
 	Private
 *********************************************************************************************/

	private:

		float		_width;
		float		_height;
	};
	
}


#endif /* Plane_h */
