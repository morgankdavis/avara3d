//
//  Sphere.h
//	avara3d
//
//  Created by Morgan Davis on 11/6/17.
//  Copyright © 2017 Morgan K Davis. All rights reserved.
//

#ifndef Sphere_h
#define Sphere_h


#include <memory>

#include "a3d/mesh/MeshElement.h"


namespace a3d {


	class Mesh;
	class Material;


	class Sphere: public MeshElement {

	public:

		static std::shared_ptr<Mesh> Mesh(float radius,
										  int segments,
										  std::shared_ptr<Material> material
										  = std::make_shared<Material>());

/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

		Sphere(float radius,
			   int segments);
		
/*********************************************************************************************
 	Public
 *********************************************************************************************/

		float 	radius() const;
		int 	segments() const;

/*********************************************************************************************
	Private
 *********************************************************************************************/

	private:

		float	_radius;
		int		_segments;
	};
}


#endif /* Sphere_h */
