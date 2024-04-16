//
//  Cylinder.h
//  avara3d
//
//  Created by Morgan Davis on 11/8/17.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_CYLINDER_H
#define AVARA3D_CYLINDER_H


#include <memory>

#include "a3d/mesh/MeshElement.h"


namespace a3d {


	class Mesh;
	class Material;

	
	class Cylinder: public MeshElement {

	private:

		static constexpr int DEFAULT_SLICES = 32;
		static constexpr int DEFAULT_SEGMENTS = 8;
		static constexpr int DEFAULT_RINGS = 4;

	public:

		static std::shared_ptr<Mesh> Mesh(float radius,
										  float height,
										  int slices = DEFAULT_SLICES,
										  int segments = DEFAULT_SEGMENTS,
										  int rings = DEFAULT_RINGS,
										  std::shared_ptr<Material> material = nullptr);

/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

		Cylinder(float radius,
				 float height,
				 int slices = DEFAULT_SLICES,
				 int segments = DEFAULT_SEGMENTS,
				 int rings = DEFAULT_RINGS);
		
/*********************************************************************************************
	Public
 *********************************************************************************************/
		
		float 	radius() const;
		float 	height() const;
		int 	slices() const;
		int 	segments() const;
		int 	rings() const;
		
/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

	private:

		float 	_radius;
		float 	_height;
		int 	_slices;
		int 	_segments;
		int 	_rings;
	};
}


#endif /* AVARA3D_CYLINDER_H */
