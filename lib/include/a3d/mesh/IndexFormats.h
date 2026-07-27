//
//  IndexFormats.h
//  avara3d
//
//  Created by Morgan Davis on 1/6/2026.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_MESH_INDEXTYPES_H
#define AVARA3D_MESH_INDEXTYPES_H

#include <cstdint>

namespace a3d {

	enum class IndexFormat : uint8_t {
		None,
		U16,
		U32
	};

	constexpr uint16_t IndexStride(IndexFormat format) {
		switch (format) {
			case a3d::IndexFormat::U16: return 2;
			case a3d::IndexFormat::U32: return 4;
			default: return 0;
		}
	}
}

#endif //AVARA3D_MESH_INDEXTYPES_H
