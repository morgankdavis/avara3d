//
//  Cone.h
//	avara-engine
//
//  Created by Morgan Davis on 11/8/17.
//  Copyright © 2017 Morgan K Davis. All rights reserved.
//

#ifndef Cone_h
#define Cone_h


#include "Geometry.h"


namespace ae {

	
	class Cone: public Geometry {

/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

	public:

		Cone(float radius, float height, int slices, int segments);
		
/*********************************************************************************************
	Public
 *********************************************************************************************/
		
		float 	radius() const;
		float 	height() const;
		
/*********************************************************************************************
	Private
 *********************************************************************************************/

	private:

		float 	_radius;
		float 	_height;
	};
}


#endif /* Cone_h */
