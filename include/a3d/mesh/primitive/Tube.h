//
//  Tube.h
//	avara3d
//
//  Created by Morgan Davis on 11/8/17.
//  Copyright © 2017 Morgan K Davis. All rights reserved.
//

#ifndef Tube_h
#define Tube_h


#include "a3d/mesh/Mesh.h"


namespace a3d {
	
	class Tube: public Mesh {

/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

	public:

		Tube(float innerRadius, float outerRadius, float height, int slices, int segments);
	};
}


#endif /* Tube_h */
