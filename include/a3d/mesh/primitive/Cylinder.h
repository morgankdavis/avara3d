//
//  Cylinder.h
//	avara3d
//
//  Created by Morgan Davis on 11/8/17.
//  Copyright © 2017 Morgan K Davis. All rights reserved.
//

#ifndef Cylinder_h
#define Cylinder_h


#include "a3d/mesh/Mesh.h"


namespace a3d {
	
	class Cylinder: public Mesh {

/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

	public:

		Cylinder(float radius, float height, int slices, int segments);
		
/*********************************************************************************************
	Public
 *********************************************************************************************/
		
		float 	radius() const;
		float 	height() const;
		
/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

	private:

		float 	_radius;
		float 	_height;
	};
}


#endif /* Cylinder_h */
