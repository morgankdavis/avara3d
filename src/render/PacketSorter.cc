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
#include "a3d/render/PipelineKey.h"

using namespace a3d;
using namespace std;

/// Private Static Non-Member Prototypes ///

static inline uint32_t 	PtrHash32(const void* p);
static inline uint16_t 	PtrHash16(const void* p);
static inline uint32_t 	FoldHash32(size_t h);
static void				SortItems(vector<DrawItem>& items);

/// Internal Static Member Functions ///

uint64_t PacketSorter::MakeSortKey(const DrawItem& item) {

	const uint32_t hk = FoldHash32(PipelineKeyHash{}(item.key));
	uint64_t k = (uint64_t)hk << 32;
	k |= (uint64_t)PtrHash16(item.material) << 16;
	k |= (uint64_t)PtrHash16(item.element);
	return k;
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

uint32_t PtrHash32(const void* p) {
	uintptr_t v = (uintptr_t)p >> 4; // drop alignment bits
	uint32_t lo = (uint32_t)v;
	uint32_t hi = (uint32_t)(v >> 32);
	uint32_t h  = lo ^ hi;
	h ^= (h >> 16);
	return h;
}

uint16_t PtrHash16(const void* p) {
	uint32_t h = PtrHash32(p);
	return (uint16_t)(h ^ (h >> 16));
}

uint32_t FoldHash32(size_t h) {
	uint32_t x = (uint32_t)h ^ (uint32_t)(h >> 32);
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
