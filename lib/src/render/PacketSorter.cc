//
//  PacketSorter.cc
//  avara3d
//
//  Created by Morgan Davis on 1/4/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#include "a3d/render/PacketSorter.h"

#include <algorithm>
#include <type_traits>

#include "a3d/render/DrawPacket.h"
#include "a3d/render/PipelineDesc.h"

using namespace a3d;
using namespace std;

/// Private Static Non-Member Prototypes ///

static uint32_t 	PtrHash32(const void* p);
static uint16_t 	PtrHash16(const void* p);
static uint32_t 	FoldHash32(size_t h);
static void			SortItems(vector<DrawItem>& items);

/// Internal Static Member Functions ///

uint64_t PacketSorter::MakeSortKey(const DrawItem& item) {

	const uint32_t descHash = FoldHash32(PipelineDescHash{}(item.desc));
	uint64_t sortKey = (uint64_t)descHash << 32;
	sortKey |= (uint64_t)PtrHash16(item.material) << 16;
	sortKey |= (uint64_t)PtrHash16(item.element);
	return sortKey;
}

void PacketSorter::SortPacket(DrawPacket& packet) {

	for (auto& item : packet.mainPassItems) {
		item.sortKey = MakeSortKey(item);
	}
	for (auto& item : packet.wireframePassItems) {
		item.sortKey = MakeSortKey(item);
	}

	SortItems(packet.mainPassItems);
	SortItems(packet.wireframePassItems);
}

/// Private Static Non-Member Functions ///

//uint32_t PtrHash32(const void* p) {
//	uintptr_t v = (uintptr_t)p >> 4; // drop alignment bits
//	uint32_t lo = (uint32_t)v;
//	uint32_t hi = (uint32_t)(v >> 32);
//	uint32_t h  = lo ^ hi;
//	h ^= (h >> 16);
//	return h;
//}

uint32_t PtrHash32(const void* p) {
	uintptr_t v = reinterpret_cast<uintptr_t>(p) >> 4; // drop alignment bits

	uint32_t lo = static_cast<uint32_t>(v);
	uint32_t hi = 0;

	if constexpr (sizeof(uintptr_t) > 4) {
		hi = static_cast<uint32_t>(v >> 32);
	}

	uint32_t h = lo ^ hi;
	h ^= (h >> 16);
	return h;
}

uint16_t PtrHash16(const void* p) {
	uint32_t h = PtrHash32(p);
	return (uint16_t)(h ^ (h >> 16));
}

//uint32_t FoldHash32(uint64_t h) {
//	uint32_t x = (uint32_t)h ^ (uint32_t)(h >> 32);
//	// cheap mix (avalanche-ish)
//	x ^= x >> 16;
//	x *= 0x7feb352d;
//	x ^= x >> 15;
//	x *= 0x846ca68b;
//	x ^= x >> 16;
//	return x;
//}

uint32_t FoldHash32(size_t h) {
	uint32_t x = static_cast<uint32_t>(h);

	if constexpr (sizeof(size_t) > 4) {
		x ^= static_cast<uint32_t>(h >> 32);
	}

	// cheap mix (avalanche-ish)
	x ^= x >> 16;
	x *= 0x7feb352d;
	x ^= x >> 15;
	x *= 0x846ca68b;
	x ^= x >> 16;
	return x;
}

void SortItems(vector<DrawItem>& items) {

	std::sort(items.begin(), items.end(),
			  [](const DrawItem& a, const DrawItem& b) {
				  uint8_t pa = static_cast<underlying_type<PassKind>::type>(a.pass);
				  uint8_t pb = static_cast<underlying_type<PassKind>::type>(b.pass);
				  if (pa != pb) return pa < pb;
				  if (a.sortKey != b.sortKey) return a.sortKey < b.sortKey;
				  return a.sequence < b.sequence;
			  });
}
