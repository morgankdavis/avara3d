//
//  Tube.h
//	avara-engine
//
//  Created by Morgan Davis on 11/8/17.
//  Copyright © 2017 Morgan K Davis. All rights reserved.
//

#ifndef Tube_h
#define Tube_h


#include "ae/mesh/Mesh.h"


namespace ae {

	
	class Tube: public Mesh {

/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

	public:

		Tube(float innerRadius, float outerRadius, float height, int slices, int segments);
	};
}


#endif /* Tube_h */
