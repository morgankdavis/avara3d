//
//  Capsule.h
//	avara3d
//
//  Created by Morgan Davis on 11/8/17.
//  Copyright © 2017 Morgan K Davis. All rights reserved.
//

#ifndef Capsule_h
#define Capsule_h

#include <memory>


#include "a3d/mesh/MeshElement.h"


namespace a3d {


	class Mesh;
	class Material;


	class Capsule: public MeshElement {

	public:

		static std::shared_ptr<Mesh> Mesh(float radius,
										  float height,
										  int slices,
										  int segments,
										  int rings,
										  std::shared_ptr<Material> material
										  = std::make_shared<Material>());

/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

		Capsule(float radius,
				float height,
				int slices,
				int segments,
				int rings);
		
/*********************************************************************************************
	Public
 *********************************************************************************************/
		
		float 	radius() const;
		float 	height() const;
		int 	slices() const;
		int 	segments() const;
		int 	rings() const;
		
/*********************************************************************************************
	Private
 *********************************************************************************************/

	private:

		float	_radius;
		float	_height;
		int 	_slices;
		int 	_segments;
		int 	_rings;
	};
}


#endif /* Capsule_h */
