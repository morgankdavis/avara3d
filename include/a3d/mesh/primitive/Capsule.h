//
//  Capsule.h
//	avara3d
//
//  Created by Morgan Davis on 11/8/17.
//  Copyright © 2017 Morgan K Davis. All rights reserved.
//

#ifndef Capsule_h
#define Capsule_h


#include "a3d/mesh/Mesh.h"


namespace a3d {

	class Capsule: public Mesh {

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
