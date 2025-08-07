#pragma once

#include <concepts>
#include <type_traits>

namespace snir {

template<typename Enum>
concept ScopedEnum = std::is_scoped_enum_v<Enum>;

}  // namespace snir
