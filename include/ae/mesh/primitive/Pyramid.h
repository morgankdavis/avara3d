//
//  Pyramid.h
//	avara-engine
//
//  Created by Morgan Davis on 11/8/17.
//  Copyright © 2017 Morgan K Davis. All rights reserved.
//

#ifndef Pyramid_h
#define Pyramid_h


#include "ae/mesh/Mesh.h"


namespace ae {

	
	class Pyramid: public Mesh {

/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

	public:

		Pyramid(float width, float height);
	};
	
}


#endif /* Pyramid_h */
