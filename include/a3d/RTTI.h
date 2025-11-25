//
// Created by mkd on 11/24/25.
//

#ifndef AVARA3D_RTTI_H
#define AVARA3D_RTTI_H


#include <cassert>
#include <memory>


namespace a3d {

	template<class To, class From>
	inline std::unique_ptr<To> dyn_cast(std::unique_ptr<From>&& p) {
		if (p && isa<To>(*p)) {
			return std::unique_ptr<To>(static_cast<To*>(p.release()));
		}
		return nullptr;
	}

	template<class To, class From>
	inline std::shared_ptr <To> dyn_cast(const std::shared_ptr <From> &p) {
		return isa<To>(p.get()) ? std::static_pointer_cast<To>(p) : std::shared_ptr < To > {};
	}

	template<class To, class From>
	inline std::weak_ptr<To> dyn_cast(const std::weak_ptr<From>& wp) {
		if (auto sp = wp.lock()) {
			if (isa<To>(*sp)) {
				return std::static_pointer_cast<To>(sp);
			}
		}
		return std::weak_ptr<To>{};
	}
}


#endif //AVARA3D_RTTI_H
