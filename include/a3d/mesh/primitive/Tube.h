//
//  Tube.h
//  avara3d
//
//  Created by Morgan Davis on 11/8/17.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_TUBE_H
#define AVARA3D_TUBE_H


#include <memory>

#include "a3d/mesh/MeshElement.h"


namespace a3d {


	class Mesh;
	class Material;


	class Tube: public MeshElement {

/*********************************************************************************************
 	Public
 *********************************************************************************************/
	public:

		static std::shared_ptr<Mesh> Mesh(float innerRadius,
										  float outerRadius,
										  float height,
										  unsigned slices = DEFAULT_SLICES,
										  unsigned segments = DEFAULT_SEGMENTS,
										  unsigned rings = DEFAULT_RINGS,
										  std::shared_ptr<Material> material = nullptr);

		Tube(float innerRadius,
			 float outerRadius,
			 float height,
			 unsigned slices = DEFAULT_SLICES,
			 unsigned segments = DEFAULT_SEGMENTS,
			 unsigned rings = DEFAULT_RINGS);

		float 		innerRadius() const;
		float 		outerRadius() const;
		float 		height() const;
		unsigned 	slices() const;
		unsigned 	segments() const;
		unsigned 	rings() const;

/*********************************************************************************************
	Private
 *********************************************************************************************/
	private:

		static constexpr unsigned DEFAULT_SLICES = 32;
		static constexpr unsigned DEFAULT_SEGMENTS = 8;
		static constexpr unsigned DEFAULT_RINGS = 1;

		float		_innerRadius;
		float		_outerRadius;
		float		_height;
		unsigned 	_slices;
		unsigned	_segments;
		unsigned	_rings;
	};
}


#endif /* AVARA3D_TUBE_H */
