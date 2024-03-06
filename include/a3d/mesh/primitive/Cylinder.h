//
//  Cylinder.h
//	avara3d
//
//  Created by Morgan Davis on 11/8/17.
//  Copyright © 2017 Morgan K Davis. All rights reserved.
//

#ifndef Cylinder_h
#define Cylinder_h


#include <memory>

#include "a3d/mesh/MeshElement.h"


namespace a3d {


	class Mesh;
	class Material;

	
	class Cylinder: public MeshElement {

	public:

		static std::shared_ptr<Mesh> Mesh(float radius,
										  float height,
										  int slices,
										  int segments,
										  const std::shared_ptr<Material> material
										  = std::make_shared<Material>());

/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

		Cylinder(float radius,
				 float height,
				 int slices,
				 int segments);
		
/*********************************************************************************************
	Public
 *********************************************************************************************/
		
		float 	radius() const;
		float 	height() const;
		int 	slices() const;
		int 	segments() const;
		
/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

	private:

		float 	_radius;
		float 	_height;
		int 	_slices;
		int 	_segments;
	};
}


#endif /* Cylinder_h */
