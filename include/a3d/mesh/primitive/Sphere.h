//
//  Sphere.h
//  avara3d
//
//  Created by Morgan Davis on 11/6/17.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_SPHERE_H
#define AVARA3D_SPHERE_H

#include <memory>

#include "a3d/mesh/MeshElement.h"

namespace a3d {

	class Mesh;
	class Material;

	class Sphere: public MeshElement {

	public:
		/// Public Static Member Functions ///

		static std::shared_ptr<Mesh> Mesh(float radius,
										  unsigned segments = DEFAULT_SEGMENTS,
										  std::shared_ptr<Material> material = nullptr);

		/// Public Lifecycle Functions ///

		explicit Sphere(float radius,
						unsigned segments = DEFAULT_SEGMENTS);

		/// Public Member Functions ///

		float 		radius() const;
		unsigned 	segments() const;

	private:
		/// Private Constants ///

		static constexpr unsigned DEFAULT_SEGMENTS = 4;

		/// Private Member Variables ///

		float		_radius;
		unsigned	_segments;
	};
}

#endif /* AVARA3D_SPHERE_H */
