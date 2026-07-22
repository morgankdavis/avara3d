//
//  chrono.h
//  avara3d
//
//  Created by Morgan Davis on 1/1/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_UTIL_CHRONO_H
#define AVARA3D_UTIL_CHRONO_H

#include <chrono>

namespace a3d::util::chrono {

	double Time(); // TODO: CHANGE THIS?

	std::chrono::milliseconds milliseconds(std::chrono::seconds sec);
	std::chrono::milliseconds milliseconds(std::chrono::nanoseconds ns);
	std::chrono::milliseconds sec_f_to_ms(float secF);
	float ns_to_ms_f(std::chrono::nanoseconds ns);
	int ns_to_ms_i(std::chrono::nanoseconds ns);

}

#endif //AVARA3D_UTIL_CHRONO_H
