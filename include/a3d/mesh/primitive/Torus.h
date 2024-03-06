//
//  Torus.h
//	avara3d
//
//  Created by Morgan Davis on 11/8/17.
//  Copyright © 2017 Morgan K Davis. All rights reserved.
//

#ifndef Torus_h
#define Torus_h


#include "a3d/mesh/MeshElement.h"


namespace a3d {

	class Torus: public MeshElement {

/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

	public:

		Torus(float minorRadius,
			  float majorRadius,
			  int slices,
			  int segments);

/*********************************************************************************************
 	Public
 *********************************************************************************************/

		float 	minorRadius() const;
		float 	majorRadius() const;
		int 	slices() const;
		int 	segments() const;

/*********************************************************************************************
	Private
 *********************************************************************************************/

	private:

		float	_minorRadius;
		float	_majorRadius;
		int 	_slices;
		int		_segments;
	};
}


#endif /* Torus_h */
