//
//  Plane.h
//	avara-engine
//
//  Created by Morgan Davis on 10/31/17.
//  Copyright © 2017 Morgan K Davis. All rights reserved.
//

#ifndef Plane_h
#define Plane_h


#include "Geometry.h"


namespace ae {

	
	class Plane: public Geometry {
		
	public:

/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/
		
		Plane(float width, float height);
		
/*********************************************************************************************
 	Public
 *********************************************************************************************/
		
		float width() const;
		float height() const;
		
	private:
		
/*********************************************************************************************
 	Private
 *********************************************************************************************/
		
		float		_width;
		float		_height;
	};
	
}


#endif /* Plane_h */
