//
//  Disk.h
//	avara3d
//
//  Created by Morgan Davis on 3/6/24.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_DISK_H
#define AVARA3D_DISK_H

#include <memory>


#include "a3d/mesh/MeshElement.h"


namespace a3d {


	class Mesh;
	class Material;


	class Disk: public MeshElement {

	private:

		static constexpr int DEFAULT_SLICES = 32;
		static constexpr int DEFAULT_RINGS = 4;

	public:

		static std::shared_ptr<Mesh> Mesh(float radius,
										  float innerRadius,
										  int slices = DEFAULT_SLICES,
										  int rings = DEFAULT_RINGS,
										  std::shared_ptr<Material> material = nullptr);

/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

		Disk(float radius,
			 float innerRadius,
			 int slices = DEFAULT_SLICES,
			 int rings = DEFAULT_RINGS);

/*********************************************************************************************
	Public
 *********************************************************************************************/

		float 	radius() const;
		float 	innerRadius() const;
		int 	slices() const;
		int 	rings() const;

/*********************************************************************************************
	Private
 *********************************************************************************************/

	private:

		float	_radius;
		float	_innerRadius;
		int 	_slices;
		int 	_rings;
	};
}


#endif //AVARA3D_DISK_H
