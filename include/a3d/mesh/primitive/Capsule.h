//
//  Capsule.h
//  avara3d
//
//  Created by Morgan Davis on 11/8/17.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_CAPSULE_H
#define AVARA3D_CAPSULE_H


#include <memory>

#include "a3d/mesh/MeshElement.h"


namespace a3d {


	class Mesh;
	class Material;


	class Capsule: public MeshElement {

	private:

		static constexpr unsigned DEFAULT_SLICES = 32;
		static constexpr unsigned DEFAULT_SEGMENTS = 4;
		static constexpr unsigned DEFAULT_RINGS = 8;

	public:

		static std::shared_ptr<Mesh> Mesh(float radius,
										  float height,
										  unsigned slices = DEFAULT_SLICES,
										  unsigned segments = DEFAULT_SEGMENTS,
										  unsigned rings = DEFAULT_RINGS,
										  std::shared_ptr<Material> material = nullptr);

/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

		Capsule(float radius,
				float height,
				unsigned slices = DEFAULT_SLICES,
				unsigned segments = DEFAULT_SEGMENTS,
				unsigned rings = DEFAULT_RINGS);
		
/*********************************************************************************************
	Public
 *********************************************************************************************/
		
		float 		radius() const;
		float 		height() const;
		unsigned 	slices() const;
		unsigned 	segments() const;
		unsigned 	rings() const;
		
/*********************************************************************************************
	Private
 *********************************************************************************************/

	private:

		float		_radius;
		float		_height;
		unsigned 	_slices;
		unsigned 	_segments;
		unsigned 	_rings;
	};
}


#endif /* AVARA3D_CAPSULE_H */
