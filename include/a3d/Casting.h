//
//  Casting.cc
//  avara3d
//
//  Created by Morgan Davis on 11/24/2025.
//  Copyright © 2025 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_CASTING_H
#define AVARA3D_CASTING_H


#include <cassert>


namespace a3d {

	template<class To, class From>
	inline bool isa(const From &x) {
		return To::classof(x);
	}

	template<class To, class From>
	inline To *dyn_cast(From *x) {
		return x && isa<To>(*x) ? static_cast<To *>(x) : nullptr;
	}

	template<class To, class From>
	inline To &cast(From &x) {
		assert(isa<To>(x));
		return static_cast<To &>(x);
	}
}


#endif //AVARA3D_CASTING_H
