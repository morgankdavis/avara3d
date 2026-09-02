//
//  Chrono.cc
//  avara3d
//
//  Created by Morgan Davis on 1/1/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#include "a3d/util/Chrono.h"

//
// double a3d::util::chrono::Time() {
// 	auto now = std::chrono::steady_clock::now();
// 	return std::chrono::duration<double>(now.time_since_epoch()).count();
// }

// std::chrono::milliseconds a3d::util::chrono::milliseconds(std::chrono::seconds sec) {
// 	return std::chrono::duration_cast<std::chrono::milliseconds>(sec);
// }
//
// std::chrono::milliseconds a3d::util::chrono::milliseconds(std::chrono::nanoseconds ns) {
// 	// truncates toward zero (fast)
// 	// return std::chrono::duration_cast<std::chrono::milliseconds>(ns)
// 	// rounds to nearest millisecond
// 	return std::chrono::round<std::chrono::milliseconds>(ns);
// }
//
// std::chrono::milliseconds a3d::util::chrono::sec_f_to_ms(float secF) {
// 	// round to nearest ms
// 	return std::chrono::round<std::chrono::milliseconds>(std::chrono::duration<float>(secF));
// }
//
// float a3d::util::chrono::ns_to_ms_f(std::chrono::nanoseconds ns) {
// 	return std::chrono::duration<float, std::milli>(ns).count();
// }
//
// int a3d::util::chrono::ns_to_ms_i(std::chrono::nanoseconds ns) {
// 	// ns -> ms (int) - truncates toward zero, then clamp to int range
// 	const auto ms_ll = std::chrono::duration_cast<std::chrono::milliseconds>(ns).count();
// 	if (ms_ll > static_cast<long long>(std::numeric_limits<int>::max()))
// 		return std::numeric_limits<int>::max();
// 	if (ms_ll < static_cast<long long>(std::numeric_limits<int>::min()))
// 		return std::numeric_limits<int>::min();
// 	return static_cast<int>(ms_ll);
// }
