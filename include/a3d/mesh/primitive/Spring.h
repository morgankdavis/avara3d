//
//  Spring.h
//  avara3d
//
//  Created by Morgan Davis on 3/6/24.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_SPRING_H
#define AVARA3D_SPRING_H

#include <memory>

#include "a3d/mesh/MeshElement.h"

namespace a3d {


	class Mesh;
	class Material;


	class Spring: public MeshElement {

/*********************************************************************************************
	Public Static Member Functions
 *********************************************************************************************/

	public:

		static std::shared_ptr<Mesh> Mesh(float minorRadius,
										  float majorRadius,
										  float length,
										  unsigned slices = DEFAULT_SLICES,
										  unsigned segments = DEFAULT_SEGMENTS,
										  std::shared_ptr<Material> material = nullptr);

/*********************************************************************************************
	Public Lifecycle Functions
 *********************************************************************************************/

		Spring(float minorRadius,
			   float majorRadius,
			   float length,
			   unsigned slices = DEFAULT_SLICES,
			   unsigned segments = DEFAULT_SEGMENTS);

/*********************************************************************************************
	Public Member Functions
 *********************************************************************************************/

		float 		minorRadius() const;
		float 		majorRadius() const;
		float 		length() const;
		unsigned 	slices() const;
		unsigned 	segments() const;

/*********************************************************************************************
	Private Constants
 *********************************************************************************************/

	private:

		static constexpr unsigned DEFAULT_SLICES = 8;
		static constexpr unsigned DEFAULT_SEGMENTS = 32;

/*********************************************************************************************
	Private Member Variables
 *********************************************************************************************/

		float 		_minorRadius;
		float 		_majorRadius;
		float 		_length;
		unsigned 	_slices;
		unsigned 	_segments;
	};
}

#endif //AVARA3D_SPRING_H
