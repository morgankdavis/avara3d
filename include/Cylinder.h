//
//  Cylinder.h
//	avara-engine
//
//  Created by Morgan Davis on 11/8/17.
//  Copyright © 2017 Morgan K Davis. All rights reserved.
//

#ifndef Cylinder_h
#define Cylinder_h


#include "Geometry.h"


namespace ae {

	
	class Cylinder: public Geometry {
		
	public:

/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/
		
		Cylinder(float radius, float height, int slices, int segments);
		
/*********************************************************************************************
	Public
 *********************************************************************************************/
		
		float radius() const;
		float height() const;
		
	private:
		
/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/
		
		float 	_radius;
		float 	_height;
	};
}


#endif /* Cylinder_h */
