//
//  RoundedBox.h
//  avara3d
//
//  Created by Morgan Davis on 3/6/24.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_MESH_PRIMITIVE_ROUNDEDBOX_H
#define AVARA3D_MESH_PRIMITIVE_ROUNDEDBOX_H

#include <memory>

#include "a3d/mesh/MeshElement.h"

namespace a3d {

	class Mesh;
	class Material;

	class RoundedBox: public MeshElement {

	public:
		/// Public Static Member Functions ///

		static std::shared_ptr<Mesh> Mesh(float radius,
										  float length,
										  float width,
										  float height,
										  unsigned slices = DEFAULT_SLICES,
										  unsigned lengthSegments = DEFAULT_SEGMENTS,
										  unsigned widthSegments = DEFAULT_SEGMENTS,
										  unsigned heightSegments = DEFAULT_SEGMENTS,
										  std::shared_ptr<Material> material = nullptr);

		/// Public Lifecycle Functions ///

		RoundedBox(float radius,
				   float length,
				   float width,
				   float height,
				   unsigned slices = DEFAULT_SLICES,
				   unsigned lengthSegments = DEFAULT_SEGMENTS,
				   unsigned widthSegments = DEFAULT_SEGMENTS,
				   unsigned heightSegments = DEFAULT_SEGMENTS);

		/// Public Member Functions ///

		float 		radius() const;
		float 		length() const;
		float 		width() const;
		float 		height() const;
		unsigned 	slices() const;
		unsigned 	lengthSegments() const;
		unsigned 	widthSegments() const;
		unsigned 	heightSegments() const;

	private:
		/// Private Constants ///

		static constexpr unsigned DEFAULT_SLICES = 8;
		static constexpr unsigned DEFAULT_SEGMENTS = 8;

		/// Private Member Variables ///

		float 		_radius;
		float		_length;
		float		_width;
		float		_height;
		unsigned 	_slices;
		unsigned 	_lengthSegments;
		unsigned 	_widthSegments;
		unsigned 	_heightSegments;
	};
}

#endif //AVARA3D_MESH_PRIMITIVE_ROUNDEDBOX_H
