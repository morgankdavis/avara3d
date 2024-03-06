//
//  Plane.h
//	avara3d
//
//  Created by Morgan Davis on 10/31/17.
//  Copyright © 2017 Morgan K Davis. All rights reserved.
//

#ifndef Plane_h
#define Plane_h


#include "a3d/mesh/MeshElement.h"


namespace a3d {
	
	class Plane: public MeshElement {

/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

	public:

		Plane(float width,
			  float height,
			  int widthSegements = 1,
			  int heightSegments = 1);
		
/*********************************************************************************************
 	Public
 *********************************************************************************************/
		
		float 		width() const;
		float 		height() const;
		int			widthSegements() const;
		int			heightSegments() const;
		
/*********************************************************************************************
 	Private
 *********************************************************************************************/

	private:

		float		_width;
		float		_height;
		int			_widthSegements;
		int			_heightSegments;
	};
}


#endif /* Plane_h */
