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

/*********************************************************************************************
 	Public
 *********************************************************************************************/
	public:

		static std::shared_ptr<Mesh> Mesh(unsigned p,
										  unsigned q,
										  unsigned slices = DEFAULT_SLICES,
										  unsigned segments = DEFAULT_SEGMENTS,
										  std::shared_ptr<Material> material = nullptr);

		TorusKnot(unsigned p,
				  unsigned q,
				  unsigned slices = DEFAULT_SLICES,
				  unsigned segments = DEFAULT_SEGMENTS);

		unsigned 	p() const;
		unsigned 	q() const;
		unsigned 	slices() const;
		unsigned 	segments() const;

/*********************************************************************************************
	Private
 *********************************************************************************************/
	private:

		static constexpr unsigned DEFAULT_SLICES = 8;
		static constexpr unsigned DEFAULT_SEGMENTS = 96;

		unsigned	_p;
		unsigned	_q;
		unsigned 	_slices;
		unsigned	_segments;
	};
}


#endif //AVARA3D_TORUSKNOT_H
