//
//  Enum.h
//  avara3d
//
//  Created by Morgan Davis on 8/17/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_UTIL_ENUM_H
#define AVARA3D_UTIL_ENUM_H

#include <array>
#include <cstddef>
#include <optional>
#include <string_view>
#include <type_traits>
#include <utility>

namespace a3d::util::enums {

    namespace detail {

        // matches magic_enum's default reflection range.
        static constexpr int ENUM_RANGE_MIN = -128;
        static constexpr int ENUM_RANGE_MAX = 127;

        template<typename E>
        constexpr int range_min() noexcept {
            if constexpr (std::is_signed_v<std::underlying_type_t<E>>) {
                return ENUM_RANGE_MIN;
            }
            else {
                return 0;
            }
        }

        template<typename E>
        constexpr int range_max() noexcept {
            return ENUM_RANGE_MAX;
        }

        constexpr std::string_view unqualified_name(std::string_view name) noexcept {

            const auto scope = name.rfind("::");
            if (scope != std::string_view::npos) {
                name.remove_prefix(scope + 2);
            }

            return name;
        }

        template<auto V>
        consteval std::string_view enum_name_impl() noexcept {

            using E = decltype(V);
            static_assert(std::is_enum_v<E>);

#if defined(__clang__) || defined(__GNUC__)

            constexpr std::string_view signature = __PRETTY_FUNCTION__;
            constexpr std::string_view prefix = "V = ";

            const auto prefixPos = signature.find(prefix);
            if (prefixPos == std::string_view::npos) {
                return {};
            }

            const auto begin = prefixPos + prefix.size();

    #if defined(__clang__)
            const auto end = signature.find(']', begin);
    #else
            auto end = signature.find(';', begin);
            if (end == std::string_view::npos) {
                end = signature.find(']', begin);
            }
    #endif

            if (end == std::string_view::npos) {
                return {};
            }

            auto name = signature.substr(begin, end - begin);

#elif defined(_MSC_VER)

            constexpr std::string_view signature = __FUNCSIG__;
            constexpr std::string_view prefix = "enum_name_impl<";

            const auto prefixPos = signature.find(prefix);
            if (prefixPos == std::string_view::npos) {
                return {};
            }

            const auto begin = prefixPos + prefix.size();
            const auto end = signature.find(">(void)", begin);
            if (end == std::string_view::npos) {
                return {};
            }

            auto name = signature.substr(begin, end - begin);

#else
    #error Unsupported compiler for a3d::util::enums
#endif

            // unnamed enum values are rendered by the supported compilers as
            // casts or integer values rather than symbolic enumerator names.
            if (name.empty() || name.front() == '(' || name.front() == '-'
                || (name.front() >= '0' && name.front() <= '9')) {
                return {};
            }

            return unqualified_name(name);
        }

        template<typename E, int Min, std::size_t... I>
        consteval auto make_names(std::index_sequence<I...>) noexcept {

            return std::array<std::string_view, sizeof...(I)> {
                enum_name_impl<static_cast<E>(Min + static_cast<int>(I))>()...
            };
        }

        template<typename E>
        struct EnumData {
            static_assert(std::is_enum_v<E>);

            static constexpr int min = range_min<E>();
            static constexpr int max = range_max<E>();
            static constexpr std::size_t count = static_cast<std::size_t>(max - min + 1);

            static constexpr auto names = make_names<E, min>(std::make_index_sequence<count> {});
        };

    }

    template<typename E>
    constexpr std::string_view enum_name(E value) noexcept {

        static_assert(std::is_enum_v<E>, "a3d::util::enums::enum_name() requires an enum type");

        using U = std::underlying_type_t<E>;

        constexpr int min = detail::EnumData<E>::min;
        constexpr int max = detail::EnumData<E>::max;
        constexpr auto& names = detail::EnumData<E>::names;

        const U raw = static_cast<U>(value);
        if (raw < static_cast<U>(min) || raw > static_cast<U>(max)) {
            return {};
        }

        return names[static_cast<std::size_t>(raw - static_cast<U>(min))];
    }

    template<typename E>
    constexpr std::optional<E> enum_cast(std::string_view name) noexcept {

        static_assert(std::is_enum_v<E>, "a3d::util::enums::enum_cast() requires an enum type");

        constexpr int min = detail::EnumData<E>::min;
        constexpr auto& names = detail::EnumData<E>::names;

        for (std::size_t i = 0; i < names.size(); ++i) {
            if (!names[i].empty() && names[i] == name) {
                return static_cast<E>(min + static_cast<int>(i));
            }
        }

        return {};
    }

    template<typename E>
    constexpr std::underlying_type_t<E> to_underlying(E value) noexcept {

        static_assert(std::is_enum_v<E>, "a3d::util::enums::to_underlying() requires an enum type");
        return static_cast<std::underlying_type_t<E>>(value);
    }

}

#endif // AVARA3D_UTIL_ENUM_H
