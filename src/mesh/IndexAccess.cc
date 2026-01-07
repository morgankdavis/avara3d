//
//  IndexAccess.cc
//  avara3d
//
//  Created by Morgan Davis on 1/6/2026.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#include "a3d/mesh/IndexAccess.h"

#include "a3d/mesh/MeshElement.h"

using namespace a3d;
using namespace std;

//uint32_t IndexStreamView::stride() const {
//	switch (format) {
//		case IndexFormat::U16: return 2;
//		case IndexFormat::U32: return 4;
//		default: return 0;
//	}
//}


//uint32_t MutableIndexStreamView::stride() const {
//	switch (format) {
//		case IndexFormat::U16: return 2;
//		case IndexFormat::U32: return 4;
//		default: return 0;
//	}
//}

std::optional<IndexStreamView> IndexAccess::GetIndexStreamView(const MeshElement& element) {
	const auto ib = element.indexBytes();
	const uint32_t count = element.indexCount();
	const IndexFormat fmt = element.indexFormat();

	if (count == 0 || ib.empty() || fmt == IndexFormat::None) return std::nullopt;

	const uint16_t s =
			(fmt == IndexFormat::U16) ? uint16_t(2) :
			(fmt == IndexFormat::U32) ? uint16_t(4) : uint16_t(0);

	if (s == 0) return std::nullopt;
	if (ib.size() != size_t(count) * size_t(s)) return std::nullopt;

	return IndexStreamView{ ib.data(), s, count, fmt };
}

std::optional<IndexStreamView> IndexAccess::GetIndexStreamView(const MeshElement& element, IndexFormat expectedFormat) {

	auto vOpt = GetIndexStreamView(element);
	if (!vOpt) return std::nullopt;
	if (vOpt->format != expectedFormat) return std::nullopt;

	return *vOpt;
}

//std::optional<MutableIndexStreamView> IndexAccess::GetMutableIndexStreamView(MeshElement& element) {
//	auto ib = element.indexBytesMutable(); // whatever you call it
//	const uint32_t count = element.indexCount();
//	const IndexFormat fmt = element.indexFormat();
//
//	if (count == 0 || ib.empty() || fmt == IndexFormat::None) return std::nullopt;
//
//	const uint32_t stride =
//			(fmt == IndexFormat::U16) ? 2u :
//			(fmt == IndexFormat::U32) ? 4u : 0u;
//
//	if (stride == 0) return std::nullopt;
//	if (ib.size() != size_t(count) * size_t(stride)) return std::nullopt;
//
//	return MutableIndexStreamView{ ib.data(), count, fmt };
//}

// Read/write as u32 so callers don’t branch on format
uint32_t IndexAccess::ReadIndexU32(const IndexStreamView& v, uint32_t i) {

	if (!v.base) return 0;
	if (i >= v.count) return 0;

	// format/stride consistency checks
	if (v.format == IndexFormat::U16) {
		if (v.stride != 2) return 0;
	} else if (v.format == IndexFormat::U32) {
		if (v.stride != 4) return 0;
	} else {
		return 0;
	}

	const std::byte* p = v.base + size_t(i) * size_t(v.stride);

	if (v.format == IndexFormat::U16) {
		uint16_t x = 0;
		std::memcpy(&x, p, sizeof(x));
		return (uint32_t)x;
	}

	uint32_t x = 0;
	std::memcpy(&x, p, sizeof(x));
	return x;
}


//void IndexAccess::WriteIndexU32(const MutableIndexStreamView& v, uint32_t i, uint32_t value) {
//	const uint32_t s = v.stride();
//	if (!v.base || s == 0 || i >= v.count) return;
//
//	std::byte* p = v.base + size_t(i) * size_t(s);
//
//	if (v.format == IndexFormat::U16) {
//		A3D_ASSERT(value <= 0xFFFFu);
//		const uint16_t x = (uint16_t)value;
//		std::memcpy(p, &x, sizeof(x));
//	}
//
//	if (v.format == IndexFormat::U16) {
//		const uint16_t x = (uint16_t)value; // caller must ensure range
//		std::memcpy(p, &x, sizeof(x));
//	} else { // U32
//		const uint32_t x = value;
//		std::memcpy(p, &x, sizeof(x));
//	}
//}

// Convenience: expand to U32 vector (useful for libs like VHACD, meshopt, etc.)
void IndexAccess::ExpandToU32(const IndexStreamView& v, std::vector<uint32_t>& out) {
	out.clear();
	out.reserve(v.count);

	if (v.format == IndexFormat::U16) {
		for (uint32_t i = 0; i < v.count; ++i) {
			uint16_t x;
			std::memcpy(&x, v.base + size_t(i) * 2u, 2u);
			out.push_back((uint32_t)x);
		}
	} else { // U32
		for (uint32_t i = 0; i < v.count; ++i) {
			uint32_t x;
			std::memcpy(&x, v.base + size_t(i) * 4u, 4u);
			out.push_back(x);
		}
	}
}

// Expand indices to U32. If mesh is non-indexed, generate 0..vertexCount-1.
// Assumes triangles (so vertexCount must be multiple of 3 for non-indexed).
void IndexAccess::GetTrianglesU32(const MeshElement& element, std::vector<uint32_t>& out) {
	out.clear();

	auto ivOpt = GetIndexStreamView(element);
	if (ivOpt) {
		ExpandToU32(*ivOpt, out);
		return;
	}

	// Non-indexed fallback (drawArrays-style meshes)
	const uint32_t vcount = element.vertexCount();
	A3D_ASSERT((vcount % 3u) == 0u);
	if ((vcount % 3u) != 0u) return;

	out.resize(vcount);
	for (uint32_t i = 0; i < vcount; ++i) out[i] = i;
}

