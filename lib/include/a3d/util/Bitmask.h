//
//  Bitmask.h
//  avara3d
//
//  Created by Morgan Davis on 1/6/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_UTIL_BITMASK_H
#define AVARA3D_UTIL_BITMASK_H

#include <type_traits>

namespace a3d::util::bitmask {

	// unsigned underlying type used for bitwise ops.
	template <typename E>
	using U = std::make_unsigned_t<std::underlying_type_t<E>>;

	// converts enum to its underlying integer type (unsigned for bitwise ops).
	template <typename E>
	constexpr U<E> to_uint(E e) noexcept {
		static_assert(std::is_enum_v<E>, "a3d::util::bitmask::to_uint() requires an enum type");
		return static_cast<U<E>>(static_cast<std::underlying_type_t<E>>(e));
	}

	// true if ANY bits in 'bits' are set in 'value'
	template <typename E>
	constexpr bool any(E value, E bits) noexcept {
		return (to_uint(value) & to_uint(bits)) != 0;
	}

	// true if ALL bits in 'bits' are set in 'value'
	template <typename E>
	constexpr bool all(E value, E bits) noexcept {
		const auto v = to_uint(value);
		const auto b = to_uint(bits);
		return (v & b) == b;
	}

	template <typename E>
	constexpr bool contains(E value, E bits) noexcept { return any(value, bits); }

	template <typename E>
	constexpr bool contains_all(E value, E bits) noexcept { return all(value, bits); }

	template <typename E>
	constexpr E add(E value, E bits) noexcept {
		return static_cast<E>(to_uint(value) | to_uint(bits));
	}

	template <typename E>
	constexpr E remove(E value, E bits) noexcept {
		return static_cast<E>(to_uint(value) & ~to_uint(bits));
	}

	template <typename E>
	constexpr void add_inplace(E& value, E bits) noexcept {
		value = add(value, bits);
	}

	template <typename E>
	constexpr void remove_inplace(E& value, E bits) noexcept {
		value = remove(value, bits);
	}

	template <typename E>
	struct enable_ops : std::false_type {};

	template <typename E>
	constexpr bool enable_ops_v = enable_ops<E>::value;

	template <typename E>
	concept MaskEnum = std::is_enum_v<E> && enable_ops_v<E>;

}

namespace a3d {

	template <util::bitmask::MaskEnum E>
	constexpr E operator|(E a, E b) noexcept {
		return static_cast<E>(util::bitmask::to_uint(a) | util::bitmask::to_uint(b));
	}

	template <util::bitmask::MaskEnum E>
	constexpr E operator&(E a, E b) noexcept {
		return static_cast<E>(util::bitmask::to_uint(a) & util::bitmask::to_uint(b));
	}

	template <util::bitmask::MaskEnum E>
	constexpr E operator^(E a, E b) noexcept {
		return static_cast<E>(util::bitmask::to_uint(a) ^ util::bitmask::to_uint(b));
	}

	template <util::bitmask::MaskEnum E>
	constexpr E operator~(E a) noexcept {
		return static_cast<E>(~util::bitmask::to_uint(a));
	}

	template <util::bitmask::MaskEnum E>
	constexpr E& operator|=(E& a, E b) noexcept { return a = (a | b); }

	template <util::bitmask::MaskEnum E>
	constexpr E& operator&=(E& a, E b) noexcept { return a = (a & b); }

	template <util::bitmask::MaskEnum E>
	constexpr E& operator^=(E& a, E b) noexcept { return a = (a ^ b); }
}

#endif //AVARA3D_UTIL_BITMASK_H
