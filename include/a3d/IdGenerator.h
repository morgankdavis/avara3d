//
// Created by mkd on 12/21/25.
//

#ifndef AVARA3D_IDGENERATOR_H
#define AVARA3D_IDGENERATOR_H

#include <atomic>
#include <cstdint>

namespace a3d {

	template<typename IdT>
	struct IdGenerator {

		static IdT next() {
			static std::atomic<uint32_t> counter{1};
			return static_cast<IdT>(counter.fetch_add(1, std::memory_order_relaxed));
		}
	};
}

#endif //AVARA3D_IDGENERATOR_H
