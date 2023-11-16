#pragma once

#include <concepts>
#include <type_traits>

namespace snir {

namespace detail {

template<typename E>
inline constexpr bool IsScopedEnum = requires {
    requires std::is_enum_v<E>;
    requires not std::is_convertible_v<E, std::underlying_type_t<E>>;
};

}  // namespace detail

template<typename Enum>
concept ScopedEnum = detail::IsScopedEnum<Enum>;

}  // namespace snir
