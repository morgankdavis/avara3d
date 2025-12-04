//
//  Disk.h
//  avara3d
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

/*********************************************************************************************
	Public Static Member Functions
 *********************************************************************************************/

	public:

		static std::shared_ptr<Mesh> Mesh(float radius,
										  float innerRadius,
										  unsigned slices = DEFAULT_SLICES,
										  unsigned rings = DEFAULT_RINGS,
										  std::shared_ptr<Material> material = nullptr);

/*********************************************************************************************
	Public Lifecycle Functions
 *********************************************************************************************/

		Disk(float radius,
			 float innerRadius,
			 unsigned slices = DEFAULT_SLICES,
			 unsigned rings = DEFAULT_RINGS);

/*********************************************************************************************
	Public Member Functions
 *********************************************************************************************/

		float 		radius() const;
		float 		innerRadius() const;
		unsigned 	slices() const;
		unsigned 	rings() const;

/*********************************************************************************************
	Private Constants
 *********************************************************************************************/

	private:

		static constexpr unsigned DEFAULT_SLICES = 32;
		static constexpr unsigned DEFAULT_RINGS = 4;

/*********************************************************************************************
	Private Member Variables
 *********************************************************************************************/

		float		_radius;
		float		_innerRadius;
		unsigned 	_slices;
		unsigned 	_rings;
	};
}

#endif //AVARA3D_DISK_H
