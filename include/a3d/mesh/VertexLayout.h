//
//  VertexLayout.h
//  avara3d
//
//  Created by Morgan Davis on 1/4/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_RENDER_VERTEXLAYOUT_H
#define AVARA3D_RENDER_VERTEXLAYOUT_H

#include <cstdint>

namespace a3d {

	/// Internal Types ///

	using VertexLayoutKey = uint32_t;

	enum class VertexLayout : uint32_t {
		None  = 0,
		PNT   = 1, // Position/Normal/UV0
		Lines = 2  // Position/Color (debug lines)
		// PNTT - Position/Normal/UV0/Tangent
		// PNTC - Position/Normal/UV0/Color
		// Skinned
		// Instanced
		// Morph Targets
	};
}

#endif //AVARA3D_RENDER_VERTEXLAYOUT_H
