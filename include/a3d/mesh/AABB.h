//
//  Types.h
//  avara3d
//
//  Created by Morgan Davis on 1/6/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_AABB_H
#define AVARA3D_AABB_H

#include "a3d/Math.h"

namespace a3d {

	struct AABB {

		math::vec3 min;
		math::vec3 max;

		bool valid() const { return min.x <= max.x && min.y <= max.y && min.z <= max.z; }

		static AABB Zero() {
			return {{0,0,0}, {0,0,0}};
		}

		static AABB Invalid() {
			return {{1,1,1}, {-1,-1,-1}};
		}

		static AABB Union(const AABB& a, const AABB& b) {
			if (!a.valid()) return b;
			if (!b.valid()) return a;
			return { math::min(a.min, b.min),
					 math::max(a.max, b.max) };
		}

		static void Expand(AABB& a, const math::vec3& p) {
			a.min = math::min(a.min, p);
			a.max = math::max(a.max, p);
		}

		static math::vec3 Center(const AABB& a) {
			return (a.min + a.max) * 0.5f;
		}
	};
}

#endif //AVARA3D_AABB_H
