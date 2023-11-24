//
//  Capsule.h
//	avara-engine
//
//  Created by Morgan Davis on 11/8/17.
//  Copyright © 2017 Morgan K Davis. All rights reserved.
//

#ifndef Capsule_h
#define Capsule_h


#include "geometry/Geometry.h"


namespace ae {

	
	class Capsule: public Geometry {

/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

	public:

		Capsule(float radius, float height, int slices, int segments, int rings);
		
/*********************************************************************************************
	Public
 *********************************************************************************************/
		
		float 	radius() const;
		float 	height() const;
		
/*********************************************************************************************
	Private
 *********************************************************************************************/

	private:

		float	_radius;
		float	_height;
	};
}


#endif /* Capsule_h */
