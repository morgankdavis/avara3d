//
//  Tube.h
//	avara3d
//
//  Created by Morgan Davis on 11/8/17.
//  Copyright © 2017 Morgan K Davis. All rights reserved.
//

#ifndef Tube_h
#define Tube_h


#include <memory>

#include "a3d/mesh/MeshElement.h"


namespace a3d {


	class Mesh;
	class Material;


	class Tube: public MeshElement {

	public:

		static std::shared_ptr<Mesh> Mesh(float innerRadius,
										  float outerRadius,
										  float height,
										  int slices,
										  int segments,
										  const std::shared_ptr<Material> material
										  = std::make_shared<Material>());

/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

		Tube(float innerRadius,
			 float outerRadius,
			 float height,
			 int slices,
			 int segments);

/*********************************************************************************************
 	Public
 *********************************************************************************************/

		float 	innerRadius() const;
		float 	outerRadius() const;
		float 	height() const;
		int 	slices() const;
		int 	segments() const;

/*********************************************************************************************
	Private
 *********************************************************************************************/

	private:

		float	_innerRadius;
		float	_outerRadius;
		float	_height;
		int 	_slices;
		int		_segments;
	};
}


#endif /* Tube_h */
