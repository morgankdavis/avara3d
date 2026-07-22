//
//  Attenuation.h
//  avara3d
//
//  Created by Morgan Davis on 1/7/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_VISUAL_LIGHT_ATTENUATION_H
#define AVARA3D_VISUAL_LIGHT_ATTENUATION_H

namespace a3d {

	struct Attenuation {

		static Attenuation FromRange(float range, float p = 0.01f);

		// I = 1 / (Kc + (Kl * d) + (Kq * d^2))
		float	constant = 	1.0f;
		float	linear =	0.0f;
		float	quadratic =	0.1f;
	};
}

#endif //AVARA3D_VISUAL_LIGHT_ATTENUATION_H
