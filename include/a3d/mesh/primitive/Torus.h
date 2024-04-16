//
//  Torus.h
//	avara3d
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

	private:

		static constexpr int DEFAULT_SLICES = 16;
		static constexpr int DEFAULT_SEGMENTS = 32;

	public:

		static std::shared_ptr<Mesh> Mesh(float minorRadius,
										  float majorRadius,
										  int slices = DEFAULT_SLICES,
										  int segments = DEFAULT_SEGMENTS,
										  std::shared_ptr<Material> material = nullptr);

/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

		Torus(float minorRadius,
			  float majorRadius,
			  int slices = DEFAULT_SLICES,
			  int segments = DEFAULT_SEGMENTS);

/*********************************************************************************************
 	Public
 *********************************************************************************************/

		float 	minorRadius() const;
		float 	majorRadius() const;
		int 	slices() const;
		int 	segments() const;

/*********************************************************************************************
	Private
 *********************************************************************************************/

	private:

		float	_minorRadius;
		float	_majorRadius;
		int 	_slices;
		int		_segments;
	};
}


#endif /* AVARA3D_TORUS_H */
