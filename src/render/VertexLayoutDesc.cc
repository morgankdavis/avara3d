//
//  VertexLayoutDesc.cc
//  avara3d
//
//  Created by Morgan Davis on 1/4/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#include "a3d/render/VertexLayoutDesc.h"

#include <cstddef>

#include "a3d/Types.h"
#include "a3d/mesh/VertexTypes.h"

namespace a3d {

	static constexpr VertexAttribDesc PNT_DESC[] = {
			{VertexSemantic::Position,  0, VertexFormat::F32x3, (uint16_t)offsetof(Vertex,   position)},
			{VertexSemantic::Normal,    1, VertexFormat::F32x3, (uint16_t)offsetof(Vertex,   normal)},
			{VertexSemantic::TexCoord0, 2, VertexFormat::F32x2, (uint16_t)offsetof(Vertex,   texCoord)},
	};

	static constexpr VertexAttribDesc PC_DESC[] = {
			{VertexSemantic::Position, 0, VertexFormat::F32x3, (uint16_t)offsetof(VertexPC, pos)},
			{VertexSemantic::Color0,   1, VertexFormat::F32x3, (uint16_t)offsetof(VertexPC, color)},
	};

	const VertexLayoutDesc& GetVertexLayoutDesc(VertexLayout layout) {
		static constexpr VertexLayoutDesc PNT { (uint16_t)sizeof(Vertex),   PNT_DESC };
		static constexpr VertexLayoutDesc PC  { (uint16_t)sizeof(VertexPC), PC_DESC  };
		static constexpr VertexLayoutDesc NONE{ 0, {} };

		switch (layout) {
			case VertexLayout::PNT: return PNT;
			case VertexLayout::PC:  return PC;
			default:                return NONE;
		}
	}
}
