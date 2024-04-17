//
//  Torus.h
//  avara3d
//
//  Created by Morgan Davis on 11/8/17.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_TORUS_H
#define AVARA3D_TORUS_H


#include <memory>

#include "a3d/mesh/MeshElement.h"


namespace a3d {


	class Mesh;
	class Material;


	class Torus: public MeshElement {

/*********************************************************************************************
 	Public
 *********************************************************************************************/
	public:

		static std::shared_ptr<Mesh> Mesh(float minorRadius,
										  float majorRadius,
										  unsigned slices = DEFAULT_SLICES,
										  unsigned segments = DEFAULT_SEGMENTS,
										  std::shared_ptr<Material> material = nullptr);

		Torus(float minorRadius,
			  float majorRadius,
			  unsigned slices = DEFAULT_SLICES,
			  unsigned segments = DEFAULT_SEGMENTS);

		float 		minorRadius() const;
		float 		majorRadius() const;
		unsigned 	slices() const;
		unsigned 	segments() const;

/*********************************************************************************************
	Private
 *********************************************************************************************/
	private:

		static constexpr unsigned DEFAULT_SLICES = 16;
		static constexpr unsigned DEFAULT_SEGMENTS = 32;

		float		_minorRadius;
		float		_majorRadius;
		unsigned 	_slices;
		unsigned	_segments;
	};
}


#endif /* AVARA3D_TORUS_H */
