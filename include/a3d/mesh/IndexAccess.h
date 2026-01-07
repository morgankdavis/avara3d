//
//  IndexAccess.h
//  avara3d
//
//  Created by Morgan Davis on 1/6/2026.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_MESH_INDEXACCESS_H
#define AVARA3D_MESH_INDEXACCESS_H

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <optional>
#include <span>
#include <type_traits>
#include <vector>

#include "a3d/Assert.h"
#include "a3d/mesh/IndexTypes.h"

namespace a3d {

	class MeshElement;

	struct IndexStreamView {
		const std::byte* base   = nullptr;
		uint16_t         stride = 0;
		uint32_t         count  = 0; // number of indices
		IndexFormat      format = IndexFormat::None;
	};

	struct IndexAccess {

		// basic views
		static std::optional<IndexStreamView> GetIndexStreamView(const MeshElement& element);
		static std::optional<IndexStreamView> GetIndexStreamView(const MeshElement& element, IndexFormat expectedFormat);

		// read/write as u32 so callers don’t branch on format
		static uint32_t ReadIndexU32(const IndexStreamView& v, uint32_t i);

		// convenience: expand to U32 vector (useful for libs like VHACD, meshopt, etc.)
		static void ExpandToU32(const IndexStreamView& v, std::vector<uint32_t>& out);

		// expand indices to U32. If mesh is non-indexed, generate 0..vertexCount-1
		// assumes triangles (so vertexCount must be multiple of 3 for non-indexed)
		static void GetTrianglesU32(const MeshElement& element, std::vector<uint32_t>& out);

		// Convenience: triangle iteration (assumes triangles, 3 indices per face)
		template<class F>
		static void ForEachTriangle(const MeshElement& element, F&& fn) {
			auto vOpt = GetIndexStreamView(element);
			if (!vOpt) return;
			const auto v = *vOpt;

			A3D_ASSERT((v.count % 3u) == 0u);
			if ((v.count % 3u) != 0u) return; // IMPORTANT: safe in release

			for (uint32_t i = 0; (i + 2u) < v.count; i += 3u) {
				const uint32_t a = ReadIndexU32(v, i + 0);
				const uint32_t b = ReadIndexU32(v, i + 1);
				const uint32_t c = ReadIndexU32(v, i + 2);
				fn(a, b, c);
			}
		}
	};
}

#endif //AVARA3D_MESH_INDEXACCESS_H
