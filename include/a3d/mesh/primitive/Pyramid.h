//
//  Pyramid.h
//	avara3d
//
//  Created by Morgan Davis on 11/8/17.
//  Copyright © 2017 Morgan K Davis. All rights reserved.
//

#ifndef Pyramid_h
#define Pyramid_h


#include "a3d/mesh/Mesh.h"


namespace a3d {

	class Pyramid: public Mesh {

/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

	public:

		Pyramid(float width, float height);
	};
	
}


#endif /* Pyramid_h */
