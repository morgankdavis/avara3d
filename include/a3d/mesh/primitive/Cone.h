//
//  Cone.h
//	avara3d
//
//  Created by Morgan Davis on 11/8/17.
//  Copyright © 2017 Morgan K Davis. All rights reserved.
//

#ifndef Cone_h
#define Cone_h


#include "a3d/mesh/Mesh.h"


namespace a3d {

	
	class Cone: public Mesh {

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
