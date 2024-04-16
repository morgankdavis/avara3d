//
//  TorusKnot.h
//  avara3d
//
//  Created by Morgan Davis on 3/6/24.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_TORUSKNOT_H
#define AVARA3D_TORUSKNOT_H


#include <memory>

#include "a3d/mesh/MeshElement.h"


namespace a3d {


	class Mesh;
	class Material;


	class TorusKnot: public MeshElement {

	private:

		static constexpr int DEFAULT_SLICES = 8;
		static constexpr int DEFAULT_SEGMENTS = 96;

	public:

		static std::shared_ptr<Mesh> Mesh(int p,
										  int q,
										  int slices = DEFAULT_SLICES,
										  int segments = DEFAULT_SEGMENTS,
										  std::shared_ptr<Material> material = nullptr);

/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

		TorusKnot(int p,
				  int q,
				  int slices = DEFAULT_SLICES,
				  int segments = DEFAULT_SEGMENTS);

/*********************************************************************************************
 	Public
 *********************************************************************************************/

		int 	p() const;
		int 	q() const;
		int 	slices() const;
		int 	segments() const;

/*********************************************************************************************
	Private
 *********************************************************************************************/

	private:

		int		_p;
		int		_q;
		int 	_slices;
		int		_segments;
	};
}


#endif //AVARA3D_TORUSKNOT_H
