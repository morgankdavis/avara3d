//
//  Capsule.h
//	avara-engine
//
//  Created by Morgan Davis on 11/8/17.
//  Copyright © 2017 Morgan K Davis. All rights reserved.
//

#ifndef Capsule_h
#define Capsule_h


#include "Geometry.h"


namespace ae {

	
	class Capsule: public Geometry {
		
	public:

/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/
		
		Capsule(float radius, float height, int slices, int segments, int rings);
		
/*********************************************************************************************
	Public
 *********************************************************************************************/
		
		float radius() const;
		float height() const;
		
	private:
		
/*********************************************************************************************
	Private
 *********************************************************************************************/
		
		float	_radius;
		float	_height;
	};
}


#endif /* Capsule_h */
