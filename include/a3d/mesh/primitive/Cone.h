//
//  Cone.h
//	avara3d
//
//  Created by Morgan Davis on 11/8/17.
//  Copyright © 2017 Morgan K Davis. All rights reserved.
//

#ifndef Cone_h
#define Cone_h


#include "a3d/mesh/MeshElement.h"


namespace a3d {

	class Cone: public MeshElement {

/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

	public:

		Cone(float radius,
			 float height,
			 int slices,
			 int segments);
		
/*********************************************************************************************
	Public
 *********************************************************************************************/
		
		float 	radius() const;
		float 	height() const;
		int		slices() const;
		int		segments() const;
		
/*********************************************************************************************
	Private
 *********************************************************************************************/

	private:

		float 	_radius;
		float 	_height;
		int		_slices;
		int		_segments;
	};
}


#endif /* Cone_h */
