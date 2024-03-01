//
//  Torus.h
//	avara3d
//
//  Created by Morgan Davis on 11/8/17.
//  Copyright © 2017 Morgan K Davis. All rights reserved.
//

#ifndef Torus_h
#define Torus_h


#include "a3d/mesh/Mesh.h"


namespace a3d {

	
	class Torus: public Mesh {

/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

	public:

		Torus(float minorRadius, float majorRadius, int slices, int segments);
	};
}


#endif /* Torus_h */
